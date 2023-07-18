// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxDurangoLiveLobby.h"

#if USE_DURANGOLIVE && USE_DRX_MATCHMAKING
	#using <windows.winmd>
	#using <platform.winmd>
	#using <Microsoft.Xbox.Services.winmd>
	#using <Microsoft.Xbox.GameChat.winmd>

	<DinrusX/CoreX/TypeInfo_impl.h>

	#include "DrxDurangoLiveMatchMaking.h"
	#include "DrxDurangoLiveLobbyPacket.h"

	<DinrusX/CoreX/Platform/IPlatformOS.h>

	#define TDM_SESSION_CREATE_SESSION_DATA     0
	#define TDM_SESSION_CREATE_USERDATA         1
	#define TDN_SESSION_CREATE_NUMUSERDATA      1

	#define TDM_SESSION_UPDATE_USERDATA         0
	#define TDN_SESSION_UPDATE_NUMUSERDATA      0

	#define TDM_SESSION_SEARCH_PARAM            0
	#define TDM_SESSION_SEARCH_PARAMDATA        1
	#define TDN_SESSION_SEARCH_PARAMNUMDATA     1

	#define TDN_UPDATE_SLOTS_PUBLIC             0
	#define TDN_UPDATE_SLOTS_PRIVATE            1

	#define TDM_SESSION_JOIN_CONNECTION_COUNTER 0
	#define CONNECTION_JOIN_SEND_INTERVAL       1000
	#define CONNECTION_JOIN_MAX_SEND_TIMES      30

	#define GET_REMOTE_SDA_TIMEOUT              20000

	#define SESSION_ID_ZERO                     0

	#pragma warning(push)
	#pragma warning(disable:6011 6102 6246)

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace ABI::Windows::Xbox::Networking;
using namespace ABI::Windows::Xbox::System;

// XXX - ATG
	#include "DurangoChat\ChatIntegrationLayer.h"

static const GUID SERVICECONFIG_GUID =
{
	0xe1039253, 0x2550, 0x49c7, { 0xb7, 0x85, 0x49, 0x34, 0xf0, 0x78, 0xc6, 0x85 }
};

CDrxDurangoLiveMatchMaking::CDrxDurangoLiveMatchMaking(CDrxLobby* pLobby, CDrxLobbyService* pService, EDrxLobbyService serviceType) : CDrxMatchMaking(pLobby, pService, serviceType), m_user(nullptr)
{
	m_AssociationIncoming.value = 0;

	// Make the CDrxMatchMaking base pointers point to our data so we can use the common code in CDrxMatchMaking
	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		CDrxMatchMaking::m_sessions[i] = &m_sessions[i];
		CDrxMatchMaking::m_sessions[i]->localConnection = &m_sessions[i].localConnection;

		for (uint32 j = 0; j < MAX_LOBBY_CONNECTIONS; j++)
		{
			CDrxMatchMaking::m_sessions[i]->remoteConnection[j] = &m_sessions[i].remoteConnection[j];
		}
	}

	for (uint32 i = 0; i < MAX_MATCHMAKING_TASKS; i++)
	{
		CDrxMatchMaking::m_task[i] = &m_task[i];
	}

}

DrxLobbySessionHandle CDrxDurangoLiveMatchMaking::FindSessionFromID(GUID const& sessionID)
{
	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		if (m_sessions[i].localFlags & DRXSESSION_LOCAL_FLAG_USED)
		{
			if (IsEqualGUID(m_sessions[i].sessionID, sessionID))
			{
				return i;
			}
		}
	}

	return DrxLobbyInvalidSessionHandle;
}

void CDrxDurangoLiveMatchMaking::StartAsyncActionTask(DrxMatchMakingTaskID mmTaskID, IAsyncAction* pAsyncAction)
{
	STask* pTask = &m_task[mmTaskID];

	pTask->asyncTaskState = STask::eATS_Pending;

	StartConcurrencyTaskTask(mmTaskID, ABI::Concurrency::task_from_async(pAsyncAction));
}

void CDrxDurangoLiveMatchMaking::StartConcurrencyTaskTask(DrxMatchMakingTaskID mmTaskID, concurrency::task<HRESULT> pConcTask)
{
	STask* pTask = &m_task[mmTaskID];

	pTask->asyncTaskState = STask::eATS_Pending;

	concurrency::cancellation_token_source src;
	pConcTask.then([this, mmTaskID](HRESULT res)
	{
		STask* pTask = &m_task[mmTaskID];
		if (SUCCEEDED(res))
		{
		  pTask->asyncTaskState = STask::eATS_Success;
		  return;
		}

		pTask->asyncTaskState = STask::eATS_Failed;
		UpdateTaskError(mmTaskID, DrxDurangoLiveLobbyGetErrorFromDurangoLive(res));
	}, src.get_token(), concurrency::task_continuation_context::use_current());
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::StartTask(ETask etask, bool startRunning, DrxMatchMakingTaskID* pMMTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCB, void* pCBArg)
{
	DrxMatchMakingTaskID tmpMMTaskID;
	DrxMatchMakingTaskID* pUseMMTaskID = pMMTaskID ? pMMTaskID : &tmpMMTaskID;
	EDrxLobbyError error = CDrxMatchMaking::StartTask(etask, startRunning, pUseMMTaskID, pLTaskID, h, pCB, pCBArg);

	if (error == eCLE_Success)
	{
		ResetTask(*pUseMMTaskID);
	}

	return error;
}

void CDrxDurangoLiveMatchMaking::ResetTask(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::StartSubTask(ETask etask, DrxMatchMakingTaskID mmTaskID)
{
	CDrxMatchMaking::StartSubTask(etask, mmTaskID);
}

void CDrxDurangoLiveMatchMaking::StartTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
	#if ENABLE_DRXLOBBY_DEBUG_TESTS
		EDrxLobbyError error;

		if (!m_lobby->DebugOKToStartTaskRunning(pTask->lTaskID))
		{
			return;
		}

		if (m_lobby->DebugGenerateError(pTask->lTaskID, error))
		{
			UpdateTaskError(mmTaskID, error);
			StopTaskRunning(mmTaskID);

			if ((pTask->startedTask == eT_SessionCreate) || (pTask->startedTask == eT_SessionJoin))
			{
				FreeSessionHandle(pTask->session);
			}

			return;
		}
	#endif

		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_SessionRegisterUserData:
		case eT_SessionMigrate:
			StopTaskRunning(mmTaskID);
			break;

		case eT_SessionGetUsers:
			StartSessionGetUsers(mmTaskID);
			break;

		case eT_SessionCreate:
			StartSessionCreate(mmTaskID);
			break;

		case eT_SessionUpdate:
			StartSessionUpdate(mmTaskID);
			break;

		case eT_SessionUpdateSlots:
			StartSessionUpdateSlots(mmTaskID);
			break;

		case eT_SessionStart:
			StartSessionStart(mmTaskID);
			break;

		case eT_SessionEnd:
			StartSessionEnd(mmTaskID);
			break;

		case eT_SessionDelete:
			StartSessionDelete(mmTaskID);
			break;

		case eT_SessionSearch:
			StartSessionSearch(mmTaskID);
			break;

		case eT_SessionJoin:
			StartSessionJoin(mmTaskID);
			break;

	#if NETWORK_HOST_MIGRATION
		case eT_SessionMigrateHostStart:
			HostMigrationStartNT(mmTaskID);
			break;

		case eT_SessionMigrateHostServer:
			HostMigrationServerNT(mmTaskID);
			break;
	#endif

		case eT_SessionSetLocalUserData:
			StartSessionSetLocalUserData(mmTaskID);
			break;

		case eT_SessionQuery:
			StartSessionQuery(mmTaskID);
			break;
		}
	}
}

void CDrxDurangoLiveMatchMaking::EndTask(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
	#if NETWORK_HOST_MIGRATION
		if (pTask->startedTask & HOST_MIGRATION_TASK_ID_FLAG)
		{
			m_sessions[pTask->session].hostMigrationInfo.m_taskID = DrxLobbyInvalidTaskID;
		}
	#endif

		if (pTask->cb)
		{
			switch (pTask->startedTask)
			{
			case eT_SessionRegisterUserData:
			case eT_SessionUpdate:
			case eT_SessionUpdateSlots:
			case eT_SessionStart:
			case eT_SessionEnd:
			case eT_SessionSetLocalUserData:
			case eT_SessionDelete:
	#if NETWORK_HOST_MIGRATION
			case eT_SessionMigrateHostStart:
	#endif
				((DrxMatchmakingCallback)pTask->cb)(pTask->lTaskID, pTask->error, pTask->cbArg);
				break;

			case eT_SessionGetUsers:
				EndSessionGetUsers(mmTaskID);
				break;

			case eT_SessionMigrate:                                                           // From the games point of view a migrate is the same as a create
				((DrxMatchmakingSessionCreateCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, m_sessions[pTask->session].localConnection.uid), pTask->cbArg);
				break;

			case eT_SessionCreate:
				EndSessionCreate(mmTaskID);
				break;

			case eT_SessionSearch:
				EndSessionSearch(mmTaskID);
				break;

			case eT_SessionJoin:
				EndSessionJoin(mmTaskID);
				break;

			case eT_SessionQuery:
				EndSessionQuery(mmTaskID);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] EndTask %d (%d) error %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		// Clear LIVE specific task state so that base class tasks can use this slot
		ResetTask(mmTaskID);

		FreeTask(mmTaskID);
	}
}

void CDrxDurangoLiveMatchMaking::StopTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
		pTask->running = false;
		TO_GAME_FROM_LOBBY(&CDrxDurangoLiveMatchMaking::EndTask, this, mmTaskID);
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::CreateSessionHandle(DrxLobbySessionHandle* pH, bool host, uint32 createFlags, int numUsers)
{
	EDrxLobbyError error = CDrxMatchMaking::CreateSessionHandle(pH, host, createFlags, numUsers);

	if (error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[*pH];

		pSession->localConnection.name[0] = 0;
		memset(pSession->localConnection.userData, 0, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
		memset(&pSession->sessionID, 0, sizeof(pSession->sessionID));

		if (!m_user)
		{
			return eCLE_UserNotSignedIn;
		}

		pSession->localConnection.xuid = m_user->Xuid();
		pSession->localConnection.uid.m_uid = static_cast<uint16>(m_user->Xuid());
	}

	return error;
}

void CDrxDurangoLiveMatchMaking::CreateSecureDeviceAssociationHandler(ISecureDeviceAssociation* pSecureDeviceAssociation, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

	pConnection->pSecureDeviceAssociation = pSecureDeviceAssociation;
	if (!pConnection->pSecureDeviceAssociation)
	{
		return;
	}

	pConnection->state = SSession::SRConnection::eRCS_CreateAssociationSuccess;

	SOCKADDR_STORAGE remoteSocketAddress;
	pConnection->pSecureDeviceAssociation->GetRemoteSocketAddressBytes(sizeof(remoteSocketAddress), reinterpret_cast<BYTE*>(&remoteSocketAddress));

	TNetAddress addr = TNetAddress(SSockAddrStorageAddr(remoteSocketAddress));
	NetLog("[Lobby] Create Secure Device Association Handler from: " PRFORMAT_ADDR, PRARG_ADDR(addr));

	DrxLobbyConnectionID connectionID = m_lobby->FindConnection(addr);

	if (connectionID == DrxLobbyInvalidConnectionID)
	{
		connectionID = m_lobby->CreateConnection(addr);
	}

	if (connectionID != DrxLobbyInvalidConnectionID)
	{
		m_lobby->ConnectionAddRef(connectionID);

		pConnection->connectionID = connectionID;
	}
}

void CDrxDurangoLiveMatchMaking::StartCreateSecureDeviceAssociation(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

	ComPtr<IAsyncOperation<SecureDeviceAssociation*>> pCreateAssociationOperation;
	if (!m_pSecureDeviceAssociationTemplate)
	{
		pConnection->state = SSession::SRConnection::eRCS_CreateAssociationFailed;
		return;
	}

	NetLog("[Lobby] StartCreateSecureDeviceAssociation creating association");
	m_pSecureDeviceAssociationTemplate->CreateAssociationAsync(pSession->remoteConnection[id].pSecureDeviceAddress.Get(), CreateSecureDeviceAssociationBehavior_Default, &pCreateAssociationOperation);

	pConnection->state = SSession::SRConnection::eRCS_CreatingAssociation;

	enum
	{
		Result = 0,
		Association
	};

	ABI::Concurrency::task_from_async(pCreateAssociationOperation.Get()).then(
	  [this, h, id](std::tuple<HRESULT, ComPtr<ISecureDeviceAssociation>> res)
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];
		NetLog("[Lobby] StartCreateSecureDeviceAssociation create association result 0x%08x", std::get<Result>(res));

		if (FAILED(std::get<Result>(res)))
		{
		  pConnection->state = SSession::SRConnection::eRCS_CreateAssociationFailed;
		  return;
		}
		CreateSecureDeviceAssociationHandler(std::get<Association>(res).Get(), h, id);
	});
}

DrxMatchMakingConnectionID CDrxDurangoLiveMatchMaking::AddRemoteConnection(DrxLobbySessionHandle h, ISecureDeviceAddress* pSecureDeviceAddress, ISecureDeviceAssociation* pSecureDeviceAssociation, Live::Xuid xuid)
{
	SSession* pSession = &m_sessions[h];

	SDrxMatchMakingConnectionUID remoteUID;
	{
		remoteUID.m_sid = GetSIDFromSessionHandle(h);
		remoteUID.m_uid = static_cast<uint16>(xuid);
	}

	DrxMatchMakingConnectionID id = CDrxMatchMaking::AddRemoteConnection(h, DrxLobbyInvalidConnectionID, remoteUID, 1);

	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

		pConnection->flags |= CMMRC_FLAG_PLATFORM_DATA_VALID;
		pConnection->xuid = xuid;

		if (pSecureDeviceAssociation)
		{
			SOCKADDR_STORAGE remoteSocketAddress;

			pConnection->state = SSession::SRConnection::eRCS_CreateAssociationSuccess;
			pSecureDeviceAssociation->get_RemoteSecureDeviceAddress(&pConnection->pSecureDeviceAddress);
			pConnection->pSecureDeviceAssociation = pSecureDeviceAssociation;
			pConnection->pSecureDeviceAssociation->GetRemoteSocketAddressBytes(sizeof(remoteSocketAddress), reinterpret_cast<BYTE*>(&remoteSocketAddress));

			TNetAddress addr = TNetAddress(SSockAddrStorageAddr(remoteSocketAddress));
			NetLog("[Lobby] Adding Remote connection address: " PRFORMAT_ADDR, PRARG_ADDR(addr));

			DrxLobbyConnectionID connectionID = m_lobby->FindConnection(addr);

			if (connectionID == DrxLobbyInvalidConnectionID)
			{
				connectionID = m_lobby->CreateConnection(addr);
			}

			if (connectionID != DrxLobbyInvalidConnectionID)
			{
				m_lobby->ConnectionAddRef(connectionID);

				pConnection->connectionID = connectionID;

				return id;
			}
		}
		else
		{
			if (pSecureDeviceAddress)
			{
				pConnection->pSecureDeviceAddress = pSecureDeviceAddress;

				return id;
			}
		}
	}

	if (id != DrxMatchMakingInvalidConnectionID)
	{
		FreeRemoteConnection(h, id);
	}

	return DrxMatchMakingInvalidConnectionID;
}

void CDrxDurangoLiveMatchMaking::FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

		if (pConnection->used)
		{
			SessionUserDataEvent(eCLSE_SessionUserLeave, h, id);

			if (pConnection->pSecureDeviceAssociation)
			{
				ComPtr<IAsyncAction> action;
				pConnection->pSecureDeviceAssociation->DestroyAsync(&action);
			}

			pConnection->pSecureDeviceAddress = nullptr;
			pConnection->pSecureDeviceAssociation = nullptr;

			pConnection->xuid = 0;
			pConnection->name[0] = 0;

			CDrxMatchMaking::FreeRemoteConnection(h, id);
		}
	}
}

concurrency::task<HRESULT> CDrxDurangoLiveMatchMaking::FreeRemoteConnectionAsync(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	if (id == DrxMatchMakingInvalidConnectionID)
		return concurrency::create_task([](){ return S_OK; });

	SSession* pSession = &m_sessions[h];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

	if (!pConnection->used)
		return concurrency::create_task([](){ return S_OK; });

	// XXX - ATG
	GetChatIntegrationLayer()->RemoveRemoteConnection(h, id, pConnection->uid.m_uid);

	SessionUserDataEvent(eCLSE_SessionUserLeave, h, id);

	ComPtr<IAsyncAction> action;
	if (pConnection->pSecureDeviceAssociation)
	{
		pConnection->pSecureDeviceAssociation->DestroyAsync(&action);
	}

	pConnection->pSecureDeviceAddress = nullptr;
	pConnection->pSecureDeviceAssociation = nullptr;

	pConnection->xuid = 0;
	pConnection->name[0] = 0;

	CDrxMatchMaking::FreeRemoteConnection(h, id);
	if (action.Get())
	{
		return ABI::Concurrency::task_from_async(action);
	}

	return concurrency::create_task([](){ return S_OK; });
}

uint64 CDrxDurangoLiveMatchMaking::GetSIDFromSessionHandle(DrxLobbySessionHandle h)
{

	DRX_ASSERT_MESSAGE((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED), "CDrxDurangoLiveMatchMaking::GetSIDFromSessionHandle: invalid session handle");

	uint64 temp;

	memcpy(&temp, &m_sessions[h].sessionID.Data4, sizeof(temp));

	return temp;

}

void CDrxDurangoLiveMatchMaking::Tick(CTimeValue tv)
{
	CDrxMatchMaking::Tick(tv);

	for (uint32 i = 0; i < MAX_MATCHMAKING_TASKS; ++i)
	{
		STask* pTask = &m_task[i];

	#if ENABLE_DRXLOBBY_DEBUG_TESTS
		if (pTask->used)
		{
			if (m_lobby->DebugTickCallStartTaskRunning(pTask->lTaskID))
			{
				StartTaskRunning(i);
				continue;
			}
			if (!m_lobby->DebugOKToTickTask(pTask->lTaskID, pTask->running))
			{
				continue;
			}
		}
	#endif

		if (pTask->used)
		{
			if (pTask->running)
			{
				switch (pTask->subTask)
				{
				case eT_SessionCreate:
					TickSessionCreate(i);
					break;

				case eT_SessionUpdate:
					TickSessionUpdate(i);
					break;

				case eT_SessionUpdateSlots:
					TickSessionUpdateSlots(i);
					break;

				case eT_SessionStart:
					TickSessionStart(i);
					break;

				case eT_SessionEnd:
					TickSessionEnd(i);
					break;

				case eT_SessionDelete:
					TickSessionDelete(i);
					break;

				case eT_SessionSearch:
					TickSessionSearch(i);
					break;

				case eT_SessionJoin:
					TickSessionJoin(i);
					break;

				case eT_SessionJoinCreateAssociation:
					TickSessionJoinCreateAssociation(i);
					break;

				case eT_SessionRequestJoin:
					TickSessionRequestJoin(i);
					break;

	#if NETWORK_HOST_MIGRATION
				case eT_SessionMigrateHostStart:
					TickHostMigrationStartNT(i);
					break;

				case eT_SessionMigrateHostServer:
					TickHostMigrationServerNT(i);
					break;
	#endif
				default:
					TickBaseTask(i);
					break;
				}
			}
		}
	}

	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		SSession* session = &m_sessions[i];

		if (session->localFlags & DRXSESSION_LOCAL_FLAG_USED)
		{
			// Check what Live thinks the status of our connections is
			for (uint32 j = 0; j < MAX_LOBBY_CONNECTIONS; j++)
			{
				SSession::SRConnection* pConnection = &session->remoteConnection[j];

				if (pConnection->used)
				{
					if (m_lobby->ConnectionGetState(pConnection->connectionID) != eCLCS_NotConnected)
					{
						SecureDeviceAssociationState state = SecureDeviceAssociationState_Invalid;
						pConnection->pSecureDeviceAssociation->get_State(&state);
						if ((state == SecureDeviceAssociationState_DestroyingLocal) ||
						    (state == SecureDeviceAssociationState_DestroyingRemote) ||
						    (state == SecureDeviceAssociationState_Destroyed))
						{
							m_lobby->ConnectionSetState(pConnection->connectionID, eCLCS_NotConnected);
						}
					}
				}
			}
		}
	}

	if (CLobbyCVars::Get().p2pShowConnectionStatus)
	{
		LogConnectionStatus();
	}
}

void CDrxDurangoLiveMatchMaking::LogConnectionStatus()
{
	static float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float xpos = 50;
	DrxFixedStringT<128> buffer;

	for (uint32 sessionIndex = 0; sessionIndex < MAX_MATCHMAKING_SESSIONS; ++sessionIndex)
	{
		SSession* pSession = &m_sessions[sessionIndex];
		float ypos = 50;

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USED)
		{
			for (uint32 connectionIndex = 0; connectionIndex < MAX_LOBBY_CONNECTIONS; connectionIndex++)
			{
				SSession::SRConnection* pConnection = &pSession->remoteConnection[connectionIndex];

				if (pConnection->used)
				{
					SecureDeviceAssociationState state = SecureDeviceAssociationState_Invalid;
					pConnection->pSecureDeviceAssociation->get_State(&state);
					switch (state)
					{
					case SecureDeviceAssociationState_Invalid:
						buffer.Format(PRFORMAT_UID " : Invalid", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_CreatingOutbound:
						buffer.Format(PRFORMAT_UID " : CreatingOutbound", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_CreatingInbound:
						buffer.Format(PRFORMAT_UID " : CreatingInbound", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_Ready:
						buffer.Format(PRFORMAT_UID " : Ready", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_DestroyingLocal:
						buffer.Format(PRFORMAT_UID " : DestroyingLocal", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_DestroyingRemote:
						buffer.Format(PRFORMAT_UID " : DestroyingRemote", PRARG_UID(pConnection->uid));
						break;

					case SecureDeviceAssociationState_Destroyed:
						buffer.Format(PRFORMAT_UID " : Destroyed", PRARG_UID(pConnection->uid));
						break;
					}

					if (gEnv->pRenderer)
					{
						gEnv->pRenderer->Draw2dLabel(xpos, ypos, 1.25f, white, false, buffer.c_str());
					}

					ypos += 20.0f;
				}
			}
		}

		xpos += 250.0f;
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::Initialise()
{
	EDrxLobbyError error = CDrxMatchMaking::Initialise();

	if (error == eCLE_Success)
	{
		m_registeredUserData.num = 0;

		HRESULT hr = Interface::Statics<ISecureDeviceAssociationTemplate>()->GetTemplateByName(Microsoft::WRL::Wrappers::HStringReference(L"PeerTraffic").Get(), &m_pSecureDeviceAssociationTemplate);

		// XXX - ATG
		error = GetChatIntegrationLayer()->Initialize(true, this);
	}

	return error;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::Terminate()
{
	// XXX - ATG
	ShutdownChatIntegrationLayer();

	EDrxLobbyError error = CDrxMatchMaking::Terminate();

	return error;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SetINetworkingUser(Live::State::INetworkingUser* user)
{
	m_user = user;

	if (user == nullptr)
	{
		return eCLE_InvalidParam;
	}

	return eCLE_Success;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionRegisterUserData(SDrxSessionUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	if (numData < MAX_MATCHMAKING_SESSION_USER_DATA)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionRegisterUserData, false, &mmTaskID, pTaskID, DrxLobbyInvalidSessionHandle, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			memcpy(m_registeredUserData.data, pData, numData * sizeof(pData[0]));
			m_registeredUserData.num = numData;

			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_OutOfSessionUserData;
	}

	NetLog("[Lobby] Start SessionRegisterUserData error %d", error);

	return error;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionMigrate(DrxSessionHandle gh, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	// Because we simply want to re-use the session that is already available, we don't need to do much here

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionMigrate, false, &mmTaskID, pTaskID, h, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionMigrate error %d", error);

	return error;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionQuery(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionQueryCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionQuery, false, &mmTaskID, pTaskID, h, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionQuery error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionQuery(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

void CDrxDurangoLiveMatchMaking::EndSessionQuery(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	((DrxMatchmakingSessionQueryCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionGetUsers(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetUsersCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionGetUsers, false, &tid, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[Lobby] Start SessionGetUsers error %d", error);
	}

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionGetUsers(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->error == eCLE_Success)
	{
		SDrxUserInfoResult temp;
		int a;

		// Glue in local user
		SDrxDurangoLiveUserID* pID = new SDrxDurangoLiveUserID(m_sessions[pTask->session].localConnection.xuid);

		if (pID)
		{
			temp.m_userID = pID;
			temp.m_conID = m_sessions[pTask->session].localConnection.uid;
			temp.m_isDedicated = false;           // Live does not support dedicated servers
			drx_strcpy(temp.m_userName, m_sessions[pTask->session].localConnection.name);
			memcpy(temp.m_userData, m_sessions[pTask->session].localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
			((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &temp, pTask->cbArg);
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
		}

		for (a = 0; (a < MAX_LOBBY_CONNECTIONS) && (pTask->error == eCLE_Success); a++)
		{
			if (m_sessions[pTask->session].remoteConnection[a].used)
			{
				pID = new SDrxDurangoLiveUserID(m_sessions[pTask->session].remoteConnection[a].xuid);

				if (pID)
				{
					temp.m_userID = pID;
					temp.m_conID = m_sessions[pTask->session].remoteConnection[a].uid;
					drx_strcpy(temp.m_userName, m_sessions[pTask->session].remoteConnection[a].name);
					memcpy(temp.m_userData, m_sessions[pTask->session].remoteConnection[a].userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
					((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &temp, pTask->cbArg);
				}
				else
				{
					UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
				}
			}
		}
	}

	((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionSetLocalUserData(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		if (dataSize <= DRXLOBBY_USER_DATA_SIZE_IN_BYTES)
		{
			SSession* pSession = &m_sessions[h];
			SSession::SLConnection* pLConnection = &pSession->localConnection;
			DrxMatchMakingTaskID mmTaskID;

			memcpy(pLConnection->userData, pData, dataSize);

			error = StartTask(eT_SessionSetLocalUserData, false, &mmTaskID, pTaskID, h, (void*)pCB, pCBArg);

			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}
		else
		{
			error = eCLE_OutOfUserData;
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionCreate(uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;
	EDrxLobbyError error = CreateSessionHandle(&h, true, flags, numUsers);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionCreate, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[mmTaskID];

			error = CreateTaskParamMem(mmTaskID, TDM_SESSION_CREATE_SESSION_DATA, pData, sizeof(SDrxSessionData));

			if (error == eCLE_Success)
			{
				error = CreateTaskParamMem(mmTaskID, TDM_SESSION_CREATE_USERDATA, pData->m_data, pData->m_numData * sizeof(pData->m_data[0]));

				if (error == eCLE_Success)
				{
					pTask->numParams[TDN_SESSION_CREATE_NUMUSERDATA] = pData->m_numData;
					FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
				}
			}

			if (error != eCLE_Success)
			{
				FreeTask(mmTaskID);
				FreeSessionHandle(h);
			}
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	NetLog("[Lobby] Start SessionCreate error %d", error);

	return error;
}

// ~^~^~TODO: We should set up the Incoming SDA listener here.
void CDrxDurangoLiveMatchMaking::StartSessionCreate(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionCreate(DrxMatchMakingTaskID mmTaskID)
{
	using namespace ABI::Microsoft::Xbox::Services::Matchmaking;
	STask* pTask = &m_task[mmTaskID];

	// If we don't have an async running... {
	// If we have not looked in 5 seconds:
	if (m_user)
	{
		Live::Xuid xuids;
		HRESULT hr = m_user->GetRemoteXuids(1, &xuids, nullptr);

		SSession* pSession = &m_sessions[pTask->session];

		pSession->sessionID = m_user->GetSessionName();

		pSession->localConnection.uid.m_sid = GetSIDFromSessionHandle(pTask->session);
		pSession->localConnection.uid.m_uid = static_cast<uint16>(m_user->Xuid());

		if (m_AssociationIncoming.value == 0)
		{
			m_pSecureDeviceAssociationTemplate->add_AssociationIncoming(Callback<ITypedEventHandler<SecureDeviceAssociationTemplate*, SecureDeviceAssociationIncomingEventArgs*>>(
			                                                              [](ISecureDeviceAssociationTemplate* sender, ISecureDeviceAssociationIncomingEventArgs* args) -> HRESULT
			{
				UNREFERENCED_PARAMETER(sender);

				ComPtr<ISecureDeviceAssociation> SDAssoc;
				args->get_Association(&SDAssoc);

				SOCKADDR_STORAGE remoteSocketAddress;
				SDAssoc->GetRemoteSocketAddressBytes(sizeof(remoteSocketAddress), reinterpret_cast<BYTE*>(&remoteSocketAddress));
				TNetAddress addr = TNetAddress(SSockAddrStorageAddr(remoteSocketAddress));

				NetLog("[Lobby] Incoming Secure Device Association from: " PRFORMAT_ADDR, PRARG_ADDR(addr));

				return S_OK;
			}).Get(), &m_AssociationIncoming);
		}

		StopTaskRunning(mmTaskID);
	}
}

void CDrxDurangoLiveMatchMaking::EndSessionCreate(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	((DrxMatchmakingSessionCreateCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, m_sessions[pTask->session].localConnection.uid), pTask->cbArg);

	if (pTask->error == eCLE_Success)
	{
		InitialUserDataEvent(pTask->session);
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionUpdate(DrxSessionHandle gh, SDrxSessionUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			DrxMatchMakingTaskID mmTaskID;

			error = StartTask(eT_SessionUpdate, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

			if (error == eCLE_Success)
			{
				STask* pTask = &m_task[mmTaskID];

				error = CreateTaskParamMem(mmTaskID, TDM_SESSION_UPDATE_USERDATA, pData, numData * sizeof(pData[0]));

				if (error == eCLE_Success)
				{
					pTask->numParams[TDN_SESSION_UPDATE_NUMUSERDATA] = numData;
					FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
				}
				else
				{
					FreeTask(mmTaskID);
				}
			}
		}
		else
		{
			error = eCLE_InvalidRequest;
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionUpdate error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionUpdate(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionUpdate(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionUpdateSlots(DrxSessionHandle gh, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionUpdateSlots, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[mmTaskID];

			pTask->numParams[TDN_UPDATE_SLOTS_PUBLIC] = numPublic;
			pTask->numParams[TDN_UPDATE_SLOTS_PRIVATE] = numPrivate;

			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionUpdateSlots return %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionStart(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionStart, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags |= DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionStart error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionStart(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionStart(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionEnd(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionEnd, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags &= ~DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	NetLog("[Lobby] Start SessionEnd error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionEnd(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionEnd(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionDelete(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionDelete, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	NetLog("[Lobby] Start SessionDelete error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionDelete(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	// Disconnect our local connection
	SessionDisconnectRemoteConnectionViaNub(pTask->session, DrxMatchMakingInvalidConnectionID, eDS_Local, DrxMatchMakingInvalidConnectionID, eDC_UserRequested, "Session deleted");
	SessionUserDataEvent(eCLSE_SessionUserLeave, pTask->session, DrxMatchMakingInvalidConnectionID);

	auto tasks = std::make_shared<std::vector<concurrency::task<HRESULT>>>();

	// Free any remaining remote connections
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

		if (pConnection->used)
		{
			tasks->emplace_back(FreeRemoteConnectionAsync(pTask->session, i));
		}
	}

	FreeSessionHandle(pTask->session);

	// XXX - ATG
	GetChatIntegrationLayer()->Reset();

	if (m_user)
	{
		ComPtr<IAsyncAction> sessionDelete;
		HRESULT res = m_user->DeleteSessionAsync(&sessionDelete);
		if (SUCCEEDED(res))
		{
			tasks->emplace_back(ABI::Concurrency::task_from_async(sessionDelete.Get()).then(
			                      [sessionDelete](HRESULT res)
			{
				return res;
			}));
		}
	}

	m_pSecureDeviceAssociationTemplate->remove_AssociationIncoming(m_AssociationIncoming);
	m_AssociationIncoming.value = 0;

	StartConcurrencyTaskTask(mmTaskID, concurrency::when_all(tasks->begin(), tasks->end()).then(
	                           [tasks](std::vector<HRESULT> results)
	{
		for (auto const& result : results)
		{
		  if (FAILED(result))
				return result;
		}

		return S_OK;
	}));
}

void CDrxDurangoLiveMatchMaking::TickSessionDelete(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	if (pTask->asyncTaskState != STask::eATS_Pending)
	{
		StopTaskRunning(mmTaskID);
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionSearch(uint32 user, SDrxSessionSearchParam* pParam, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionSearchCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	DrxMatchMakingTaskID mmTaskID;

	EDrxLobbyError error = StartTask(eT_SessionSearch, false, &mmTaskID, pTaskID, DrxLobbyInvalidSessionHandle, pCB, pCBArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[mmTaskID];

		error = CreateTaskParamMem(mmTaskID, TDM_SESSION_SEARCH_PARAM, pParam, sizeof(SDrxSessionSearchParam));

		if (error == eCLE_Success)
		{
			error = CreateTaskParamMem(mmTaskID, TDM_SESSION_SEARCH_PARAMDATA, pParam->m_data, pParam->m_numData * sizeof(pParam->m_data[0]));

			if (error == eCLE_Success)
			{
				pTask->numParams[TDN_SESSION_SEARCH_PARAMNUMDATA] = pParam->m_numData;
				FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}

		if (error != eCLE_Success)
		{
			FreeTask(mmTaskID);
		}
	}

	NetLog("[Lobby] Start SessionSearch error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

void CDrxDurangoLiveMatchMaking::TickSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}

void CDrxDurangoLiveMatchMaking::EndSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	SDrxDurangoLiveSessionID* id = nullptr;
	{
		if (m_user)
		{
			id = new SDrxDurangoLiveSessionID(m_user->GetSessionName());
		}
	}

	if (id)
	{
		SDrxSessionSearchResult result;
		SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA];

		result.m_id = id;

		result.m_numFilledSlots = 0;
		result.m_data.m_numPublicSlots = 0;
		result.m_data.m_numPrivateSlots = 0;
		drx_strcpy(result.m_data.m_name, "Test");
		result.m_data.m_ranked = false;

		result.m_data.m_data = userData;
		result.m_data.m_numData = m_registeredUserData.num;

		result.m_ping = 0;
		result.m_numFriends = 0;
		result.m_flags = 0;

		for (uint32 i = 0; i < m_registeredUserData.num; i++)
		{
			result.m_data.m_data[i] = m_registeredUserData.data[i];
		}

		if (pTask->cb)
		{
			((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &result, pTask->cbArg);
		}
	}

	// Need to check again as task is cancelled if result from previous call is automatically accepted,
	// in which case cb will be nulled out by the previous callback.
	if (pTask->cb)
	{
		((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionJoin(uint32* pUsers, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionJoinCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;
	EDrxLobbyError error = CreateSessionHandle(&h, false, flags, numUsers);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionJoin, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			SDrxDurangoLiveSessionID* pDurangoLiveID = (SDrxDurangoLiveSessionID*)id.get();
			SSession* pSession = &m_sessions[h];

			pSession->sessionID = pDurangoLiveID->m_sessionID;
			OLECHAR* bstrGuid;
			StringFromCLSID(pSession->sessionID, &bstrGuid);
			DrxLogAlways("SessionJoin: SID: %s", bstrGuid);
			::CoTaskMemFree(bstrGuid);

			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	NetLog("[Lobby] Start SessionJoin error %d", error);

	return error;
}

void CDrxDurangoLiveMatchMaking::StartSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	using namespace ABI::Microsoft::Xbox::Services::Matchmaking;
	STask* pTask = &m_task[mmTaskID];

	// If we don't have an async running... {
	// If we have not looked in 5 seconds:
	if (m_user)
	{
		Live::Xuid xuids;
		HRESULT hr = m_user->GetHostXuid(xuids);

		unsigned int addresses = 1;
		ComPtr<ISecureDeviceAddress> sdaddr;
		hr = m_user->GetHostSecureDeviceAddress(&sdaddr, &addresses);

		SSession* pSession = &m_sessions[pTask->session];
		pSession->localConnection.uid.m_sid = GetSIDFromSessionHandle(pTask->session);

		DrxMatchMakingConnectionID const remoteId =
		  [&]()
			{
				pSession->hostConnectionID = AddRemoteConnection(pTask->session, sdaddr.Get(), nullptr, xuids);
				return pSession->hostConnectionID;
		  } ();

		if (remoteId != DrxMatchMakingInvalidConnectionID)
		{
			StartSubTask(eT_SessionJoinCreateAssociation, mmTaskID);
			StartCreateSecureDeviceAssociation(pTask->session, remoteId);
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
			StopTaskRunning(mmTaskID);
		}
	}
	else
	{
		UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
		StopTaskRunning(mmTaskID);
	}
}

void CDrxDurangoLiveMatchMaking::TickSessionJoinCreateAssociation(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[pSession->hostConnectionID];

	if (pConnection->state == SSession::SRConnection::eRCS_CreateAssociationSuccess)
	{
		StartSubTask(eT_SessionRequestJoin, mmTaskID);
		pTask->timerStarted = false;
		pTask->numParams[TDM_SESSION_JOIN_CONNECTION_COUNTER] = 0;
	}

	if (pConnection->state == SSession::SRConnection::eRCS_CreateAssociationFailed)
	{
		UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
		StopTaskRunning(mmTaskID);
	}
}

void CDrxDurangoLiveMatchMaking::TickSessionRequestJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (!pTask->TimerStarted() || (pTask->GetTimer() > CONNECTION_JOIN_SEND_INTERVAL))
	{
		if (pTask->numParams[TDM_SESSION_JOIN_CONNECTION_COUNTER] < CONNECTION_JOIN_MAX_SEND_TIMES)
		{
			SSession* pSession = &m_sessions[pTask->session];
			const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT8Size + DrxLobbyPacketUINT64Size;
			uint8 buffer[MaxBufferSize];
			CDrxDurangoLiveLobbyPacket packet;

			pTask->StartTimer();
			pTask->numParams[TDM_SESSION_JOIN_CONNECTION_COUNTER]++;

			Live::Xuid xuid = m_user->Xuid();

			packet.SetWriteBuffer(buffer, MaxBufferSize);
			packet.StartWrite(eDurangoLivePT_SessionRequestJoin, false);
			packet.WriteUINT8(mmTaskID);
			packet.WriteUINT64(xuid);

			TNetAddress addr;
			m_lobby->AddressFromConnection(addr, pSession->remoteConnection[pSession->hostConnectionID].connectionID);
			NetLog("[Lobby] TickSessionRequestJoin sending to " PRFORMAT_ADDR, PRARG_ADDR(addr));

			if (Send(mmTaskID, &packet, pTask->session, pSession->hostConnectionID) != eSE_Ok)
			{
				UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
			}
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
			StopTaskRunning(mmTaskID);
		}
	}
}

void CDrxDurangoLiveMatchMaking::EndSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[pTask->session];

		((DrxMatchmakingSessionJoinCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, pSession->localConnection.uid), 0, 0, pTask->cbArg);
		InitialUserDataEvent(pTask->session);

		return;
	}

	((DrxMatchmakingSessionJoinCallback)pTask->cb)(pTask->lTaskID, pTask->error, DrxSessionInvalidHandle, 0, 0, pTask->cbArg);
}

void CDrxDurangoLiveMatchMaking::ProcessSessionRequestJoin(const TNetAddress& addr, CDrxDurangoLiveLobbyPacket* pPacket)
{
	NetLog("[Lobby] ProcessSessionRequestJoin from " PRFORMAT_ADDR, PRARG_ADDR(addr));

	DrxMatchMakingTaskID returnTaskID;

	returnTaskID = pPacket->ReadUINT8();
	Live::Xuid xuid = pPacket->ReadUINT64();

	if (!m_user)
		return;

	SSockAddrStorageAddr const* pSockAddrStorageAddr = boost::get<SSockAddrStorageAddr>(&addr);
	if (!pSockAddrStorageAddr)
		return;

	IDatagramSocketPtr pSocket = m_lobby->GetInternalSocket(m_serviceType);
	if (!pSocket)
		return;

	ComPtr<ISecureDeviceAssociation> pSecureDeviceAssociation;
	{
		sockaddr_storage localAddr;
		int localAddrLen = sizeof(localAddr);

		if (getsockname(pSocket->GetSysSocket(), (DRXSOCKADDR*)&localAddr, &localAddrLen) == 0)
		{
			HRESULT hr = Interface::Statics<ISecureDeviceAssociationStatics>()->GetAssociationBySocketAddressBytes(
			  sizeof(pSockAddrStorageAddr->addr),
			  reinterpret_cast<uint8*>(&const_cast<SSockAddrStorageAddr*>(pSockAddrStorageAddr)->addr),
			  localAddrLen,
			  reinterpret_cast<uint8*>(&localAddr),
			  &pSecureDeviceAssociation);
			DrxLogAlways("CDrxDurangoLiveMatchMaking::ProcessSessionRequestJoin - GetAssociationBySocketAddressBytes returned 0x%08x", hr);
		}
	}

	if (!pSecureDeviceAssociation)
		return;

	DrxLobbySessionHandle h = FindSessionFromID(m_user->GetSessionName());
	if (h == DrxLobbyInvalidSessionHandle)
		return;

	//Live::Xuid xuid;
	//m_user->GetRemoteXuids(1, &xuid, nullptr);

	SSession* pSession = &m_sessions[h];
	DrxMatchMakingConnectionID id = AddRemoteConnection(h, nullptr, pSecureDeviceAssociation.Get(), xuid);

	if (id == DrxMatchMakingInvalidConnectionID)
		return;

	SSession::SLConnection* pLocalConnection = &pSession->localConnection;
	SSession::SRConnection* pConnection = &pSession->remoteConnection[id];
	const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT8Size + DrxLobbyPacketErrorSize + DrxLobbyPacketConnectionUIDSize + DrxLobbyPacketConnectionUIDSize;
	uint8 buffer[MaxBufferSize];
	CDrxDurangoLiveLobbyPacket packet;

	packet.SetWriteBuffer(buffer, MaxBufferSize);
	packet.StartWrite(eDurangoLivePT_SessionRequestJoinResult, true);
	packet.WriteUINT8(returnTaskID);
	packet.WriteError(eCLE_Success);

	// Add clients connection info.
	packet.WriteConnectionUID(pConnection->uid);

	// Add servers connection info.
	packet.WriteConnectionUID(pLocalConnection->uid);

	Send(DrxMatchMakingInvalidTaskID, &packet, h, id);

	ConnectionEstablishedUserDataEvent(h, id);

	return;
}

void CDrxDurangoLiveMatchMaking::ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxDurangoLiveLobbyPacket* pPacket)
{
	DrxMatchMakingTaskID mmTaskID = pPacket->ReadUINT8();

	mmTaskID = FindTaskFromTaskTaskID(eT_SessionRequestJoin, mmTaskID);

	if (mmTaskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[mmTaskID];
		EDrxLobbyError error = pPacket->ReadError();

		UpdateTaskError(mmTaskID, error);
		StopTaskRunning(mmTaskID);
	}
}

uint32 CDrxDurangoLiveMatchMaking::GetSessionIDSizeInPacket() const
{
	return 0;
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const
{
	return eCLE_Success;
}

DrxSessionID CDrxDurangoLiveMatchMaking::ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const
{
	return DrxSessionInvalidID;
}

DrxSessionID CDrxDurangoLiveMatchMaking::SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle gh)
{
	return DrxSessionInvalidID;
}

void CDrxDurangoLiveMatchMaking::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	CDrxDurangoLiveLobbyPacket* pDLPacket = (CDrxDurangoLiveLobbyPacket*)pPacket;

	switch (pDLPacket->StartRead())
	{
	case eDurangoLivePT_SessionRequestJoin:
		ProcessSessionRequestJoin(addr, pDLPacket);
		break;

	case eDurangoLivePT_SessionRequestJoinResult:
		ProcessSessionRequestJoinResult(addr, pDLPacket);
		break;

	// XXX - ATG
	case eDurangoLivePT_Chat:
		GetChatIntegrationLayer()->ReceivePacket(pDLPacket);
		break;

	default:
		CDrxMatchMaking::OnPacket(addr, pDLPacket);
		break;
	}
}

	#if NETWORK_HOST_MIGRATION
void CDrxDurangoLiveMatchMaking::HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause)
{
	SSession* pSession = &m_sessions[h];

	pSession->hostMigrationInfo.Reset();
}

void CDrxDurangoLiveMatchMaking::HostMigrationFinalise(DrxLobbySessionHandle h)
{
	SSession* pSession = &m_sessions[h];

	HostMigrationReset(h);

	if ((pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST) == 0)
	{
		MarkHostHintInformationDirty(h);
	}
}

EDrxLobbyError CDrxDurangoLiveMatchMaking::HostMigrationServer(SHostMigrationInfo* pInfo)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(pInfo->m_session);

	if (h != DrxLobbyInvalidSessionHandle)
	{
		SSession* pSession = &m_sessions[h];

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USED)
		{
			DrxMatchMakingTaskID mmTaskID;

			pSession->localFlags |= DRXSESSION_LOCAL_FLAG_HOST;
			pSession->hostConnectionID = DrxMatchMakingInvalidConnectionID;
			error = StartTask(eT_SessionMigrateHostServer, false, &mmTaskID, NULL, h, NULL, NULL);

			if (error == eCLE_Success)
			{
				STask* pTask = &m_task[mmTaskID];
		#if HOST_MIGRATION_SOAK_TEST_BREAK_DETECTION
				DetectHostMigrationTaskBreak(h, "HostMigrationServer()");
		#endif
				pSession->hostMigrationInfo.m_taskID = pTask->lTaskID;

				FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}
		else
		{
			error = eCLE_InvalidSession;
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		m_hostMigration.Terminate((SHostMigrationInfo_Private*)pInfo);
		NetLog("[Host Migration]: CDrxLiveMatchMaking::HostMigrationServer(): " PRFORMAT_SH ", error %d", PRARG_SH(h), error);
	}

	return error;
}

void CDrxDurangoLiveMatchMaking::HostMigrationServerNT(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickHostMigrationServerNT(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	pSession->hostMigrationInfo.m_matchMakingFinished = true;
	StopTaskRunning(mmTaskID);
}

bool CDrxDurangoLiveMatchMaking::GetNewHostAddress(char* pAddress, SHostMigrationInfo* pInfo)
{
	return false;
}

void CDrxDurangoLiveMatchMaking::HostMigrationStartNT(DrxMatchMakingTaskID mmTaskID)
{
}

void CDrxDurangoLiveMatchMaking::TickHostMigrationStartNT(DrxMatchMakingTaskID mmTaskID)
{
	StopTaskRunning(mmTaskID);
}
	#endif

EDrxLobbyError CDrxDurangoLiveMatchMaking::SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError rc = eCLE_Success;

	#if NETWORK_HOST_MIGRATION
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if (h != DrxLobbyInvalidSessionHandle)
	{
		DrxMatchMakingTaskID mmTaskID;
		SSession* pSession = &m_sessions[h];
		rc = StartTask(eT_SessionMigrateHostStart, false, &mmTaskID, pTaskID, h, pCB, pCBArg);

		if (rc == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	#endif

	return rc;
}

void CDrxDurangoLiveMatchMaking::SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];

	if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED)
	{
		ComPtr<IUser> pUser;
		ISystem* pSystem = GetISystem();

		if (pSystem)
		{
			int userIndex = pSystem->GetPlatformOS()->GetFirstSignedInUser();
			if (userIndex != IPlatformOS::Unknown_User)
			{
				int userId = pSystem->GetPlatformOS()->UserGetId(userIndex);
				Interface::Statics<IUser>()->GetUserById(userId, &pUser);
			}
		}

		if (id == DrxMatchMakingInvalidConnectionID)
		{
			SSession::SLConnection* pConnection = &pSession->localConnection;
			SDrxDurangoLiveUserID* pID = new SDrxDurangoLiveUserID(pConnection->xuid);

			if (pID)
			{
				SDrxUserInfoResult userInfo;

				userInfo.m_conID = pConnection->uid;
				userInfo.m_isDedicated = false;           // Live does not support dedicated servers
				userInfo.m_userID = pID;
				drx_strcpy(userInfo.m_userName, pConnection->name);
				memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

				CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
			}

			if (event == eCLSE_SessionUserJoin)
			{
				if (pUser.Get())
				{
					// XXX - ATG
					auto user = reinterpret_cast<Windows::Xbox::System::User^>(pUser.Get());
					GetChatIntegrationLayer()->AddLocalUserToChatChannel(0, user);
				}
			}
			else if (event == eCLSE_SessionUserLeave)
			{
				if (pUser.Get())
				{
					// XXX - ATG
					auto user = reinterpret_cast<Windows::Xbox::System::User^>(pUser.Get());
					GetChatIntegrationLayer()->RemoveUserFromChatChannel(0, user);
				}
			}
		}
		else
		{
			SSession::SRConnection* pConnection = &pSession->remoteConnection[id];
			SDrxDurangoLiveUserID* pID = new SDrxDurangoLiveUserID(pConnection->xuid);

			if (pID)
			{
				SDrxUserInfoResult userInfo;

				userInfo.m_conID = pConnection->uid;
				userInfo.m_isDedicated = false;           // Live does not support dedicated servers
				userInfo.m_userID = pID;
				drx_strcpy(userInfo.m_userName, pConnection->name);
				memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

				CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
			}

			if (event == eCLSE_SessionUserJoin)
			{
				// XXX - ATG
				GetChatIntegrationLayer()->AddRemoteConnection(h, id, pConnection->uid.m_uid);
			}
			else if (event == eCLSE_SessionUserLeave)
			{
				// XXX - ATG
				GetChatIntegrationLayer()->RemoveRemoteConnection(h, id, pConnection->uid.m_uid);
			}
		}
	}
}

void CDrxDurangoLiveMatchMaking::ConnectionEstablishedUserDataEvent(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

	SessionUserDataEvent(eCLSE_SessionUserJoin, h, id);
}

void CDrxDurangoLiveMatchMaking::InitialUserDataEvent(DrxLobbySessionHandle h)
{
	SSession* pSession = &m_sessions[h];

	pSession->localFlags |= DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED;

	SessionUserDataEvent(eCLSE_SessionUserJoin, h, DrxMatchMakingInvalidConnectionID);

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; ++i)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

		if (pConnection->used)
		{
			SessionUserDataEvent(eCLSE_SessionUserJoin, h, i);
		}
	}
}

const char* CDrxDurangoLiveMatchMaking::SSession::GetLocalUserName(uint32 localUserIndex) const
{
	return NULL;
}

void CDrxDurangoLiveMatchMaking::SSession::Reset()
{
	CDrxMatchMaking::SSession::Reset();

	sessionID = NULL_GUID;
	hostConnectionID = DrxMatchMakingInvalidConnectionID;
}

const char* CDrxDurangoLiveMatchMaking::GetConnectionName(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const
{
	SSession::SRConnection* pPlatformConnection = reinterpret_cast<SSession::SRConnection*>(pConnection);

	if (pPlatformConnection->uid.m_uid == DEDICATED_SERVER_CONNECTION_UID)
	{
		return "DedicatedServer";
	}

	if (pPlatformConnection->used)
	{
		return pPlatformConnection->name;
	}

	return NULL;
}

uint64 CDrxDurangoLiveMatchMaking::GetConnectionUserID(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const
{
	return INVALID_USER_ID;
}

TNetAddress CDrxDurangoLiveMatchMaking::GetHostAddressFromSessionHandle(DrxSessionHandle gh)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);
	SSession* pSession = &m_sessions[h];

	if (pSession->serverConnectionID != DrxMatchMakingInvalidConnectionID)
	{
		const TNetAddress* pDedicatedServerAddr = m_lobby->GetNetAddress(pSession->remoteConnection[pSession->serverConnectionID].connectionID);

		if (pDedicatedServerAddr)
		{
			return *pDedicatedServerAddr;
		}
	}

	if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
	{
		return TNetAddress(TLocalNetAddress(m_lobby->GetInternalSocketPort(eCLS_Online)));
	}

	if (pSession->hostConnectionID != DrxMatchMakingInvalidConnectionID)
	{
		const TNetAddress* pHostAddr = m_lobby->GetNetAddress(pSession->remoteConnection[pSession->hostConnectionID].connectionID);

		if (pHostAddr)
		{
			return *pHostAddr;
		}
	}

	return TNetAddress(SNullAddr());
}

	#pragma warning(pop)

#endif // USE_DURANGOLIVE && USE_DRX_MATCHMAKING
