// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxLANLobby.h"
#include "DrxLANMatchMaking.h"
#include "DrxSharedLobbyPacket.h"

#if USE_LAN

	#define LAN_SEARCH_INTERVAL          1000
	#define MAX_SEARCH_RETRIES           5

	#define SEARCH_PARAM_GAME_PARAM      0      // ptr
	#define SEARCH_PARAM_GAME_PARAM_DATA 1      // ptr

static uint64 GetLANUserID(SDrxMatchMakingConnectionUID uid)
{
	return uid.m_uid;
}

CDrxLANMatchMaking::CDrxLANMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, EDrxLobbyService serviceType) : CDrxMatchMaking(lobby, service, serviceType)
{
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

EDrxLobbyError CDrxLANMatchMaking::StartTask(ETask etask, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg)
{
	DrxMatchMakingTaskID tmpMMTaskID;
	DrxMatchMakingTaskID* useMMTaskID = mmTaskID ? mmTaskID : &tmpMMTaskID;
	EDrxLobbyError error = CDrxMatchMaking::StartTask(etask, false, useMMTaskID, lTaskID, h, cb, cbArg);

	if (error == eCLE_Success)
	{
		ResetTask(*useMMTaskID);
	}

	return error;
}

void CDrxLANMatchMaking::ResetTask(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	pTask->search = DrxLANInvalidSearchHandle;
	pTask->ticks = 0;
}

void CDrxLANMatchMaking::StartTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* task = &m_task[mmTaskID];

	if (task->used)
	{
	#if ENABLE_DRXLOBBY_DEBUG_TESTS
		EDrxLobbyError error;

		if (!m_lobby->DebugOKToStartTaskRunning(task->lTaskID))
		{
			return;
		}

		if (m_lobby->DebugGenerateError(task->lTaskID, error))
		{
			UpdateTaskError(mmTaskID, error);
			StopTaskRunning(mmTaskID);

			if ((task->startedTask == eT_SessionCreate) || (task->startedTask == eT_SessionJoin))
			{
				FreeSessionHandle(task->session);
			}

			if (task->startedTask == eT_SessionSearch)
			{
				m_search[task->search].used = false;
			}

			return;
		}
	#endif

		task->running = true;

		switch (task->startedTask)
		{
		case eT_SessionCreate:
		case eT_SessionMigrate:
		case eT_SessionRegisterUserData:
		case eT_SessionUpdate:
		case eT_SessionUpdateSlots:
		case eT_SessionStart:
		case eT_SessionEnd:
		case eT_SessionGetUsers:
			// All of these tasks are either synchronous or not needed for LAN but forcing them to wait a tick before succeeding
			// should ensure less surprises when the asynchronous online versions come along
			StopTaskRunning(mmTaskID);
			break;

		case eT_SessionDelete:
			StartSessionDelete(mmTaskID);
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

		case eT_SessionMigrateHostClient:
			TickHostMigrationClientNT(mmTaskID);
			break;
	#endif

		case eT_SessionSetLocalUserData:
			StartSessionSetLocalUserData(mmTaskID);
			break;
		}
	}
}

void CDrxLANMatchMaking::EndTask(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* task = &m_task[mmTaskID];

	if (task->used)
	{
	#if NETWORK_HOST_MIGRATION
		//-- For host migration tasks, ensure we clear the lobby task handle in the host migration info when the task is ended.
		if (task->startedTask & HOST_MIGRATION_TASK_ID_FLAG)
		{
			m_sessions[task->session].hostMigrationInfo.m_taskID = DrxLobbyInvalidTaskID;
		}
	#endif

		if (task->cb)
		{
			switch (task->startedTask)
			{
			case eT_SessionRegisterUserData:
			case eT_SessionUpdate:
			case eT_SessionUpdateSlots:
			case eT_SessionStart:
			case eT_SessionEnd:
			case eT_SessionDelete:
			case eT_SessionSetLocalUserData:
	#if NETWORK_HOST_MIGRATION
			case eT_SessionMigrateHostStart:
	#endif
				((DrxMatchmakingCallback)task->cb)(task->lTaskID, task->error, task->cbArg);
				break;

			case eT_SessionQuery:
				((DrxMatchmakingSessionQueryCallback)task->cb)(task->lTaskID, task->error, NULL, task->cbArg);
				break;

			case eT_SessionGetUsers:
				EndSessionGetUsers(mmTaskID);
				break;

			case eT_SessionCreate:
				((DrxMatchmakingSessionCreateCallback)task->cb)(task->lTaskID, task->error, CreateGameSessionHandle(task->session, m_sessions[task->session].localConnection.uid), task->cbArg);

				if (task->error == eCLE_Success)
				{
					InitialUserDataEvent(task->session);
				}

				break;

			case eT_SessionMigrate:
				((DrxMatchmakingSessionCreateCallback)task->cb)(task->lTaskID, task->error, CreateGameSessionHandle(task->session, m_sessions[task->session].localConnection.uid), task->cbArg);
				break;

			case eT_SessionSearch:
				((DrxMatchmakingSessionSearchCallback)task->cb)(task->lTaskID, task->error, NULL, task->cbArg);
				break;

			case eT_SessionJoin:
				((DrxMatchmakingSessionJoinCallback)task->cb)(task->lTaskID, task->error, CreateGameSessionHandle(task->session, m_sessions[task->session].localConnection.uid), m_sessions[task->session].id.m_ip, m_sessions[task->session].id.m_port, task->cbArg);

				if (task->error == eCLE_Success)
				{
					InitialUserDataEvent(task->session);
				}

				break;
			}
		}

		// Clear LAN specific task state so that base class tasks can use this slot
		ResetTask(mmTaskID);

		FreeTask(mmTaskID);

		if (task->error != eCLE_Success)
		{
			NetLog("[Lobby] EndTask %u (%u) error %d", task->startedTask, task->subTask, task->error);
		}
	}
}

void CDrxLANMatchMaking::StopTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	STask* task = &m_task[mmTaskID];

	if (task->used)
	{
		task->running = false;
		TO_GAME_FROM_LOBBY(&CDrxLANMatchMaking::EndTask, this, mmTaskID);
	}
}

EDrxLobbyError CDrxLANMatchMaking::CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32 createFlags, int numUsers)
{
	EDrxLobbyError error = CDrxMatchMaking::CreateSessionHandle(h, host, createFlags, numUsers);

	if (error == eCLE_Success)
	{
		SSession* session = &m_sessions[*h];

		session->data.m_data = session->userData;
		session->numFilledSlots = numUsers;
		session->data.m_numData = m_registeredUserData.num;

		for (uint32 j = 0; j < m_registeredUserData.num; j++)
		{
			session->data.m_data[j] = m_registeredUserData.data[j];
		}
	}

	return error;
}

DrxMatchMakingConnectionID CDrxLANMatchMaking::AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, uint32 ip, uint16 port, uint32 numUsers, const char* name, uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES], bool isDedicated)
{
	if (connectionID == DrxLobbyInvalidConnectionID)
	{
		TNetAddress netAddr = TNetAddress(SIPv4Addr(ntohl(ip), ntohs(port)));

		connectionID = m_lobby->FindConnection(netAddr);

		if (connectionID == DrxLobbyInvalidConnectionID)
		{
			connectionID = m_lobby->CreateConnection(netAddr);
		}
	}

	DrxMatchMakingConnectionID id = CDrxMatchMaking::AddRemoteConnection(h, connectionID, uid, numUsers);

	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* connection = &pSession->remoteConnection[id];

		connection->flags |= CMMRC_FLAG_PLATFORM_DATA_VALID;
		connection->ip = ip;
		connection->port = port;
	#if NETWORK_HOST_MIGRATION
		connection->m_migrated = (pSession->hostMigrationInfo.m_state != eHMS_Idle) ? true : false;
	#endif
		connection->m_isDedicated = isDedicated;
		drx_strcpy(connection->name, name);
		memcpy(connection->userData, userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
		pSession->numFilledSlots += numUsers;
	}

	return id;
}

void CDrxLANMatchMaking::FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

		if (pConnection->used)
		{
			SessionUserDataEvent(eCLSE_SessionUserLeave, h, id);
			pSession->numFilledSlots -= pConnection->numUsers;
			CDrxMatchMaking::FreeRemoteConnection(h, id);
		}
	}
}

uint64 CDrxLANMatchMaking::GetSIDFromSessionHandle(DrxLobbySessionHandle h)
{
	DRX_ASSERT_MESSAGE((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED), "CDrxLANMatchMaking::GetSIDFromSessionHandle: invalid session handle");

	return m_sessions[h].sid;
}

DrxLobbySessionHandle CDrxLANMatchMaking::FindSessionFromServerID(DrxLobbySessionHandle h)
{
	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		if (m_sessions[i].localFlags & DRXSESSION_LOCAL_FLAG_USED)
		{
			if (m_sessions[i].id.m_h == h)
			{
				return i;
			}
		}
	}

	return DrxLobbyInvalidSessionHandle;
}

EDrxLobbyError CDrxLANMatchMaking::CreateSessionSearchHandle(DrxLANSearchHandle* h)
{
	for (uint32 i = 0; i < MAX_LAN_SEARCHES; i++)
	{
		if (!m_search[i].used)
		{
			m_search[i].used = true;
			m_search[i].numServers = 0;

			*h = i;

			return eCLE_Success;
		}
	}

	return eCLE_TooManyTasks;
};

EDrxLobbyError CDrxLANMatchMaking::SetSessionUserData(DrxLobbySessionHandle h, SDrxSessionUserData* data, uint32 numData)
{
	if (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		for (uint32 i = 0; i < numData; i++)
		{
			uint32 j;

			for (j = 0; j < m_registeredUserData.num; j++)
			{
				if (data[i].m_id == m_registeredUserData.data[j].m_id)
				{
					if (data[i].m_type == m_registeredUserData.data[j].m_type)
					{
						m_sessions[h].data.m_data[j] = data[i];
						break;
					}
					else
					{
						return eCLE_UserDataTypeMissMatch;
					}
				}
			}

			if (j == m_registeredUserData.num)
			{
				return eCLE_UserDataNotRegistered;
			}
		}

		return eCLE_Success;
	}

	return eCLE_InvalidSession;
}

EDrxLobbyError CDrxLANMatchMaking::SetSessionData(DrxLobbySessionHandle h, SDrxSessionData* data)
{
	if (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		m_sessions[h].data.m_numPublicSlots = data->m_numPublicSlots;
		m_sessions[h].data.m_numPrivateSlots = data->m_numPrivateSlots;
		drx_strcpy(m_sessions[h].data.m_name, data->m_name);
		m_sessions[h].data.m_ranked = data->m_ranked;

		return SetSessionUserData(h, data->m_data, data->m_numData);
	}

	return eCLE_InvalidSession;
}

void CDrxLANMatchMaking::SendSessionQuery(CTimeValue tv, uint32 index, bool broadcast)
{
	STask* pTask = &m_task[index];

	// Request session information from server
	if (pTask->GetTimer() > LAN_SEARCH_INTERVAL)
	{
		pTask->StartTimer();

		if ((pTask->ticks == MAX_SEARCH_RETRIES) || pTask->canceled)
		{
			NetLog("[Lobby] Stop LAN search ticks %u canceled %d", pTask->ticks, pTask->canceled);

			if (pTask->search != DrxLANInvalidSearchHandle)
			{
				m_search[pTask->search].used = false;
			}

			StopTaskRunning(index);
		}
		else
		{
			const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT8Size;
			CDrxSharedLobbyPacket packet;

			if (packet.CreateWriteBuffer(MaxBufferSize))
			{
				const SDrxLobbyParameters& lobbyParams = m_lobby->GetLobbyParameters();
				packet.StartWrite(eLANPT_MM_RequestServerData, false);
				packet.WriteUINT8(index);

				if (broadcast)
				{
					for (uint32 port = lobbyParams.m_connectPort; port < lobbyParams.m_connectPort + MAX_SOCKET_PORTS_TRY; ++port)
					{
						Send(DrxMatchMakingInvalidTaskID, &packet, pTask->session, TNetAddress(SIPv4Addr(BROADCAST_ADDRESS, port)));
					}
				}
				else
				{
					if (pTask->session != DrxLobbyInvalidSessionHandle)
					{
						if ((m_sessions[pTask->session].localFlags & DRXSESSION_LOCAL_FLAG_HOST))
						{
							Send(DrxMatchMakingInvalidTaskID, &packet, pTask->session, TNetAddress(SIPv4Addr(LOOPBACK_ADDRESS, lobbyParams.m_listenPort)));
						}
						else
						{
							Send(DrxMatchMakingInvalidTaskID, &packet, pTask->session, m_sessions[pTask->session].hostConnectionID);
						}
					}
				}
			}
		}

		pTask->ticks++;
	}
}

void CDrxLANMatchMaking::Tick(CTimeValue tv)
{
	CDrxMatchMaking::Tick(tv);

	for (uint32 i = 0; i < MAX_MATCHMAKING_TASKS; i++)
	{
		STask* task = &m_task[i];

	#if ENABLE_DRXLOBBY_DEBUG_TESTS
		if (task->used)
		{
			if (m_lobby->DebugTickCallStartTaskRunning(task->lTaskID))
			{
				StartTaskRunning(i);
				continue;
			}
			if (!m_lobby->DebugOKToTickTask(task->lTaskID, task->running))
			{
				continue;
			}
		}
	#endif

		if (task->used && task->running)
		{
			switch (task->subTask)
			{
			case eT_SessionQuery:

				SendSessionQuery(tv, i, false);

				break;

			case eT_SessionSearch:

				SendSessionQuery(tv, i, true);

				break;

			case eT_SessionJoin:
				TickSessionJoin(i);
				break;

	#if NETWORK_HOST_MIGRATION
			case eT_SessionMigrateHostStart:
				TickHostMigrationStartNT(i);
				break;

			case eT_SessionMigrateHostClient:
				TickHostMigrationClientNT(i);
				break;

			case eT_SessionMigrateHostFinish:
				TickHostMigrationFinishNT(i);
				break;
	#endif
			default:
				TickBaseTask(i);
				break;
			}
		}
	}
}

EDrxLobbyError CDrxLANMatchMaking::Initialise()
{
	EDrxLobbyError error = CDrxMatchMaking::Initialise();

	if (error == eCLE_Success)
	{
		m_registeredUserData.num = 0;

		for (uint32 i = 0; i < MAX_LAN_SEARCHES; i++)
		{
			m_search[i].used = false;
		}
	}

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionRegisterUserData(SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	if (numData < MAX_MATCHMAKING_SESSION_USER_DATA)
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionRegisterUserData, &tid, taskID, 0, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			for (int i = 0; i < numData; i++)
			{
				m_registeredUserData.data[i] = data[i];
			}
			m_registeredUserData.num = numData;

			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_OutOfSessionUserData;
	}

	NetLog("[Lobby] Start SessionRegisterUserData error %d", error);

	return error;
}

const char* CDrxLANMatchMaking::SSession::GetLocalUserName(uint32 localUserIndex) const
{
	if (localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		return localConnection.name;
	}

	return NULL;
}

void CDrxLANMatchMaking::SSession::Reset()
{
	PARENT::Reset();

	sid = gEnv->pTimer->GetAsyncTime().GetValue();
	hostConnectionID = DrxMatchMakingInvalidConnectionID;
	memset(localConnection.userData, 0, sizeof(localConnection.userData));
}

void CDrxLANMatchMaking::SetLocalUserName(DrxLobbySessionHandle h, uint32 localUserIndex)
{
	SSession* pSession = &m_sessions[h];
	SConfigurationParams neededInfo[1] = {
		{ CLCC_LAN_USER_NAME, { 0 }
		}
	};

	m_lobby->GetConfigurationInformation(neededInfo, 1);

	drx_strcpy(pSession->localConnection.name, (const char*)neededInfo[0].m_pData);
}

const char* CDrxLANMatchMaking::GetConnectionName(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const
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

EDrxLobbyError CDrxLANMatchMaking::SessionCreate(uint32* users, int numUsers, uint32 flags, SDrxSessionData* data, DrxLobbyTaskID* taskID, DrxMatchmakingSessionCreateCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;

	if (gEnv->IsDedicated())
	{
		DRX_ASSERT_MESSAGE(numUsers == 1, "Dedicated Server, but number of users on create != 1 - Being forced to 0");
		numUsers = 0;
	}

	EDrxLobbyError error = CreateSessionHandle(&h, true, flags, numUsers);

	if (error == eCLE_Success)
	{
		SSession* session = &m_sessions[h];

		error = SetSessionData(h, data);

		if (error == eCLE_Success)
		{
			DrxMatchMakingTaskID tid;

			error = StartTask(eT_SessionCreate, &tid, taskID, h, (void*)cb, cbArg);

			if (error == eCLE_Success)
			{
				SetLocalUserName(h, 0);

				session->localConnection.uid = CreateConnectionUID(h);
				session->localConnection.pingToServer = DRXLOBBY_INVALID_PING;
				session->localConnection.used = true;
	#if NETWORK_HOST_MIGRATION
				session->newHostPriorityCount = 0;
				session->newHostUID = DrxMatchMakingInvalidConnectionUID;
	#endif
				NetLog("[Lobby] Created local connection " PRFORMAT_SH " " PRFORMAT_UID, PRARG_SH(h), PRARG_UID(session->localConnection.uid));
				FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
			}
			else
			{
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

EDrxLobbyError CDrxLANMatchMaking::SessionMigrate(DrxSessionHandle gh, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if (h != DrxLobbyInvalidSessionHandle)
	{
		SSession* session = &m_sessions[h];

		error = SetSessionData(h, pData);

		if (error == eCLE_Success)
		{
			DrxMatchMakingTaskID tid;

			error = StartTask(eT_SessionMigrate, &tid, pTaskID, h, (void*)pCB, pCBArg);

			if (error == eCLE_Success)
			{
				session->localFlags |= DRXSESSION_LOCAL_FLAG_HOST;
				session->createFlags = flags;
				FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
			}
			else
			{
				FreeSessionHandle(h);
			}
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	NetLog("[Lobby] Start SessionMigrate error %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionQuery(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionQueryCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionQuery, &tid, pTaskID, h, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionQuery error %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionGetUsers(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetUsersCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionGetUsers, &tid, pTaskID, h, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[Lobby] Start SessionQuery error %d", error);
	}

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionSetLocalUserData(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		if (dataSize <= DRXLOBBY_USER_DATA_SIZE_IN_BYTES)
		{
			SSession* pSession = &m_sessions[h];
			SSession::SLConnection* pLConnection = &pSession->localConnection;
			DrxMatchMakingTaskID mmTaskID;

			memcpy(pLConnection->userData, pData, dataSize);

			error = StartTask(eT_SessionSetLocalUserData, &mmTaskID, pTaskID, h, (void*)pCB, pCBArg);

			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, mmTaskID);
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

void CDrxLANMatchMaking::StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT16Size + DRXLOBBY_USER_DATA_SIZE_IN_BYTES;
	CDrxSharedLobbyPacket packet;

	SessionUserDataEvent(eCLSE_SessionUserUpdate, pTask->session, DrxMatchMakingInvalidConnectionID);

	if (packet.CreateWriteBuffer(MaxBufferSize))
	{
		SSession* pSession = &m_sessions[pTask->session];
		SSession::SLConnection* pLConnection = &pSession->localConnection;

		packet.StartWrite(eLANPT_UserData, true);
		packet.WriteUINT16(pLConnection->uid.m_uid);
		packet.WriteData(pLConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			SendToAll(DrxMatchMakingInvalidTaskID, &packet, pTask->session, DrxMatchMakingInvalidConnectionID);
		}
		else
		{
			Send(DrxMatchMakingInvalidTaskID, &packet, pTask->session, pSession->hostConnectionID);
		}

		packet.FreeWriteBuffer();
	}
	else
	{
		UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
	}

	StopTaskRunning(mmTaskID);
}

void CDrxLANMatchMaking::ProcessLocalUserData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	DrxLobbySessionHandle h;
	SDrxMatchMakingConnectionUID uid;
	DrxMatchMakingConnectionID id;

	pPacket->StartRead();
	uid = pPacket->GetFromConnectionUID();
	uid.m_uid = pPacket->ReadUINT16();

	if (FindConnectionFromUID(uid, &h, &id))
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pRConnection = &pSession->remoteConnection[id];

		pPacket->ReadData(pRConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
		SessionUserDataEvent(eCLSE_SessionUserUpdate, h, id);

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			SendToAll(DrxMatchMakingInvalidTaskID, pPacket, h, id);
		}
	}
}

EDrxLobbyError CDrxLANMatchMaking::SessionUpdate(DrxSessionHandle gh, SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		if (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			error = SetSessionUserData(h, data, numData);

			if (error == eCLE_Success)
			{
				DrxMatchMakingTaskID tid;

				error = StartTask(eT_SessionUpdate, &tid, taskID, h, (void*)cb, cbArg);

				if (error == eCLE_Success)
				{
					FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
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

EDrxLobbyError CDrxLANMatchMaking::SessionUpdateSlots(DrxSessionHandle gh, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionUpdateSlots, &tid, pTaskID, h, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionUpdateSlots return %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionStart(DrxSessionHandle gh, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionStart, &tid, taskID, h, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags |= DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	NetLog("[Lobby] Start SessionStart error %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionEnd(DrxSessionHandle gh, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionEnd, &tid, taskID, h, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags &= ~DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	NetLog("[Lobby] Start SessionEnd error %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionDelete(DrxSessionHandle gh, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionDelete, &tid, taskID, h, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);

	#if NETWORK_HOST_MIGRATION
			// Since we're deleting this session, terminate any host migration
			if (pSession->hostMigrationInfo.m_state != eHMS_Idle)
			{
				m_hostMigration.Terminate(&pSession->hostMigrationInfo);
			}
	#endif
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	NetLog("[Lobby] Start SessionDelete error %d", error);

	return error;
}

void CDrxLANMatchMaking::StartSessionDelete(DrxMatchMakingTaskID mmTaskID)
{
	STask* task = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[task->session];

	#if NETWORK_HOST_MIGRATION
	// Since we're deleting this session, terminate any host migration
	if (pSession->hostMigrationInfo.m_state != eHMS_Idle)
	{
		m_hostMigration.Terminate(&pSession->hostMigrationInfo);
	}
	#endif
	pSession->createFlags &= ~DRXSESSION_CREATE_FLAG_MIGRATABLE;

	task->subTask = eT_SessionDelete;

	// Disconnect our local connection
	SessionDisconnectRemoteConnectionViaNub(task->session, DrxMatchMakingInvalidConnectionID, eDS_Local, DrxMatchMakingInvalidConnectionID, eDC_UserRequested, "Session deleted");
	SessionUserDataEvent(eCLSE_SessionUserLeave, task->session, DrxMatchMakingInvalidConnectionID);

	// Free any remaining remote connections
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SSession::SRConnection* connection = &pSession->remoteConnection[i];

		if (connection->used)
		{
			FreeRemoteConnection(task->session, i);
		}
	}

	FreeSessionHandle(task->session);
	StopTaskRunning(mmTaskID);
}

EDrxLobbyError CDrxLANMatchMaking::SessionSearch(uint32 user, SDrxSessionSearchParam* param, DrxLobbyTaskID* taskID, DrxMatchmakingSessionSearchCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	DrxLANSearchHandle h;
	EDrxLobbyError error = CreateSessionSearchHandle(&h);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionSearch, &tid, taskID, DrxLobbyInvalidSessionHandle, (void*)cb, cbArg);

		if ((error == eCLE_Success) && param && param->m_data)
		{
			error = CreateTaskParam(tid, SEARCH_PARAM_GAME_PARAM, param, 1, sizeof(SDrxSessionSearchParam));

			if (error == eCLE_Success)
			{
				error = CreateTaskParam(tid, SEARCH_PARAM_GAME_PARAM_DATA, param->m_data, param->m_numData, param->m_numData * sizeof(param->m_data[0]));
			}
		}

		if (error == eCLE_Success)
		{
			m_task[tid].search = h;
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
		else
		{
			m_search[h].used = false;
		}
	}

	NetLog("[Lobby] Start SessionSearch error %d", error);

	return error;
}

EDrxLobbyError CDrxLANMatchMaking::SessionJoin(uint32* users, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* taskID, DrxMatchmakingSessionJoinCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;
	EDrxLobbyError error = CreateSessionHandle(&h, false, flags, numUsers);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionJoin, &tid, taskID, h, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			m_sessions[h].id = *((SDrxLANSessionID*)id.get());
			SetLocalUserName(h, 0);
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, tid);
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	NetLog("[Lobby] Start SessionJoin error %d", error);

	return error;
}

uint32 CDrxLANMatchMaking::GetSessionIDSizeInPacket() const
{
	return SDrxLANSessionID::GetSizeInPacket();
}

EDrxLobbyError CDrxLANMatchMaking::WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const
{
	if (pPacket)
	{
		SDrxLANSessionID blank;

		if (sessionId != DrxSessionInvalidID)
		{
			SDrxLANSessionID* pLanID = (SDrxLANSessionID*)sessionId.get();
			if (pLanID)
			{
				pLanID->WriteToPacket(pPacket);
			}
			else
			{
				blank.WriteToPacket(pPacket);
			}
		}
		else
		{
			blank.WriteToPacket(pPacket);
		}

		return eCLE_Success;
	}

	return eCLE_InternalError;
}

DrxSessionID CDrxLANMatchMaking::ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const
{
	if (pPacket)
	{
		SDrxLANSessionID* pLanID = new SDrxLANSessionID();
		DrxSessionID returnId = pLanID;

		if (pLanID)
		{
			pLanID->ReadFromPacket(pPacket);

			SDrxLANSessionID blank;

			//-- Safe to use the equals operator here, since the fields tested in the operator are the same ones
			//-- written to the packet.
			if (!(*pLanID == blank))
			{
				//-- assume it's a valid session id
				return returnId;
			}
		}
	}

	return DrxSessionInvalidID;
}

DrxSessionID CDrxLANMatchMaking::GetSessionIDFromConsole(void)
{
	if (gEnv->pNetwork)
	{
		IConsole* pConsole = gEnv->pConsole;
		DrxFixedStringT<32> host;
		uint16 port = CLobbyCVars::Get().lobbyDefaultPort;

		if (pConsole)
		{
			ICVar* pCVar = pConsole->GetCVar("cl_serveraddr");
			if (pCVar)
			{
				host = pCVar->GetString();
			}

			pCVar = pConsole->GetCVar("cl_serverport");
			if (pCVar)
			{
				port = pCVar->GetIVal();
			}
			else
			{
				uint16 conPort, lisPort;
				m_pService->GetSocketPorts(conPort, lisPort);
				port = conPort;
			}
		}

		if (host.find("<session>") == DrxStringT<char>::npos)
		{
			CNetAddressResolver* pResolver = m_lobby->GetResolver();

			if (pResolver)
			{
				CNameRequestPtr pReq = pResolver->RequestNameLookup(host.c_str());
				pReq->Wait();
				if (pReq->GetResult() != eNRR_Succeeded)
				{
					DrxLogAlways("Name resolution for '%s' failed", host.c_str());
					return DrxSessionInvalidID;
				}

				SIPv4Addr* pAddr = stl::get_if<SIPv4Addr>(&pReq->GetAddrs()[0]);
				if (pAddr)
				{
					SDrxLANSessionID* pId = new SDrxLANSessionID;
					PREFAST_ASSUME(pId);
					pId->m_ip = htonl(pAddr->addr);
					pId->m_port = port;
					pId->m_h = DrxLobbyInvalidSessionHandle;

					return pId;
				}
			}
		}
	}

	return DrxSessionInvalidID;
}

EDrxLobbyError CDrxLANMatchMaking::GetSessionIDFromIP(const char* const pAddress, DrxSessionID* const pSessionID)
{
	EDrxLobbyError error = eCLE_InvalidParam;

	if (pSessionID)
	{
		DrxSessionID sessionID = DrxSessionInvalidID;

		if (pAddress)
		{
			DrxFixedStringT<32> host = pAddress;
			uint16 port = CLobbyCVars::Get().lobbyDefaultPort;

			CNetAddressResolver* pResolver = m_lobby->GetResolver();
			if (pResolver)
			{
				CNameRequestPtr pReq = pResolver->RequestNameLookup(host.c_str());
				pReq->Wait();
				if (pReq->GetResult() == eNRR_Succeeded)
				{
					SIPv4Addr* pAddr = stl::get_if<SIPv4Addr>(&pReq->GetAddrs()[0]);
					if (pAddr)
					{
						SDrxLANSessionID* pId = new SDrxLANSessionID;
						if (pId)
						{
							pId->m_ip = htonl(pAddr->addr);
							pId->m_port = port;
							pId->m_h = DrxLobbyInvalidSessionHandle;

							sessionID = pId;

							error = eCLE_Success;
						}
						else
						{
							error = eCLE_OutOfMemory;
						}
					}
				}
				else
				{
					DrxLogAlways("Name resolution for '%s' failed", host.c_str());
					error = eCLE_InternalError;
				}
			}
		}

		*pSessionID = sessionID;
	}

	return error;
}

void CDrxLANMatchMaking::StartSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketLobbySessionHandleSize + DrxLobbyPacketUINT8Size + DrxLobbyPacketUINT8Size + DrxLobbyPacketBoolSize + DRXLOBBY_USER_NAME_LENGTH + DRXLOBBY_USER_DATA_SIZE_IN_BYTES;
	CDrxSharedLobbyPacket packet;
	if (packet.CreateWriteBuffer(MaxBufferSize))
	{
		packet.StartWrite(eLANPT_SessionRequestJoin, true);
		packet.WriteLobbySessionHandle(pSession->id.m_h);
		packet.WriteUINT8(mmTaskID);
		packet.WriteUINT8(pSession->localConnection.numUsers);
		packet.WriteString(pSession->localConnection.name, DRXLOBBY_USER_NAME_LENGTH);
		packet.WriteData(pSession->localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

		if (Send(mmTaskID, &packet, pTask->session, TNetAddress(SIPv4Addr(ntohl(pSession->id.m_ip), pSession->id.m_port))) != eSE_Ok)
		{
			UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
		}
	}
	else
	{
		UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
	}

	if (pTask->error != eCLE_Success)
	{
		StopTaskRunning(mmTaskID);
	}
}

void CDrxLANMatchMaking::TickSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* task = &m_task[mmTaskID];

	if (task->sendStatus != eCLE_Pending)
	{
		if (task->sendStatus == eCLE_Success)
		{
			// The request to join has been sent so wait for result
			if (task->TimerStarted())
			{
				if (task->GetTimer() > DrxMatchMakingConnectionTimeOut)
				{
					// No response so fail connection attempt
					NetLog("[Lobby] SessionJoin request session join packet sent no response received");
					UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
					StartSessionDelete(mmTaskID);
				}
			}
			else
			{
				task->StartTimer();
				NetLog("[Lobby] SessionJoin request session join packet sent waiting for response");
			}
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
			NetLog("[Lobby] SessionJoin error sending request session join packet error %d", task->error);
			StartSessionDelete(mmTaskID);
		}
	}
}

void CDrxLANMatchMaking::ProcessSessionRequestJoin(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	EDrxLobbyError error = eCLE_Success;
	uint32 bufferPos = 0;
	DrxLobbySessionHandle h;
	DrxLobbyConnectionID c;
	DrxMatchMakingTaskID returnTaskID;

	pPacket->StartRead();
	h = pPacket->ReadLobbySessionHandle();
	returnTaskID = pPacket->ReadUINT8();

	if (m_lobby->ConnectionFromAddress(&c, addr))
	{
		DRXSOCKADDR_IN saddr;

		if (m_lobby->ConvertAddr(addr, &saddr))
		{
			// If the 'connect' command is used from the console with no session then we'll have an invalid
			// session handle here.  In this case, search for the first searchable host session and use that.
			if (h == DrxLobbyInvalidSessionHandle)
			{
				for (int32 index = 0; index < MAX_MATCHMAKING_SESSIONS; ++index)
				{
					SSession* pSession = &m_sessions[index];
					if (((pSession->localFlags & (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST)) == (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST)) && (pSession->createFlags & DRXSESSION_CREATE_FLAG_SEARCHABLE))
					{
						h = index;
						break;
					}
				}
			}

			if (h != DrxLobbyInvalidSessionHandle && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
			{
				SSession* session = &m_sessions[h];
				uint8 numUsers = pPacket->ReadUINT8();

				if (session->numFilledSlots + numUsers <= session->data.m_numPublicSlots + session->data.m_numPrivateSlots)
				{
					char name[DRXLOBBY_USER_NAME_LENGTH];
					uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];

					pPacket->ReadString(name, DRXLOBBY_USER_NAME_LENGTH);
					pPacket->ReadData(userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

					DrxMatchMakingConnectionID id = AddRemoteConnection(h, c, CreateConnectionUID(h), saddr.sin_addr.s_addr, saddr.sin_port, numUsers, name, userData, false);

					if (id != DrxMatchMakingInvalidConnectionID)
					{
						// Added to session
						CDrxSharedLobbyPacket packet;

						if (packet.CreateWriteBuffer(MAX_LOBBY_PACKET_SIZE))
						{
							SSession::SRConnection* connection = &session->remoteConnection[id];

							packet.StartWrite(eLANPT_SessionRequestJoinResult, true);
							packet.WriteUINT8(returnTaskID);
							packet.WriteError(error);
							packet.WriteLobbySessionHandle(h);
							packet.WriteConnectionUID(connection->uid);
							packet.WriteUINT16((session->createFlags & DRXSESSION_CREATE_FLAG_GAME_MASK) >> DRXSESSION_CREATE_FLAG_GAME_FLAGS_SHIFT);
							packet.WriteUINT8(session->localConnection.numUsers);
							if (gEnv->IsDedicated())                          // Server should flag if its dedicated or not
							{
								packet.WriteBool(true);
							}
							else
							{
								packet.WriteBool(false);
							}
							packet.WriteConnectionUID(session->localConnection.uid);
							packet.WriteString(session->localConnection.name, DRXLOBBY_USER_NAME_LENGTH);
							packet.WriteData(session->localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

							Send(DrxMatchMakingInvalidTaskID, &packet, h, id);

							// Now connect success has been sent to new client it is safe to inform game of the connection
							SessionUserDataEvent(eCLSE_SessionUserJoin, h, id);

							// Send the new clients connection to the old clients
							packet.StartWrite(eLANPT_SessionAddRemoteConnections, true);
							packet.WriteLobbySessionHandle(h);
							packet.WriteUINT8(1);
							packet.WriteUINT32(htonl(connection->ip));
							packet.WriteUINT16(htons(connection->port));
							packet.WriteConnectionUID(connection->uid);
							packet.WriteUINT8(connection->numUsers);
							packet.WriteBool(connection->m_isDedicated);
							packet.WriteString(connection->name, DRXLOBBY_USER_NAME_LENGTH);
							packet.WriteData(connection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

							NetLog("[Lobby] Send new users to peers");
							SendToAll(DrxMatchMakingInvalidTaskID, &packet, h, id);

							// Send the remote connections to the new client
							uint32 connectionToAdd = 0;

							while (true)
							{
								uint8 numConnections = 0;
								uint8 numAdded = 0;
								uint32 testSize;
								uint32 i;

								packet.StartWrite(eLANPT_SessionAddRemoteConnections, true);
								packet.WriteLobbySessionHandle(h);

								// Find out how many of the remote connections can fit in the packet

								testSize = packet.GetWriteBufferPos() + DrxLobbyPacketUINT8Size;

								for (i = connectionToAdd; i < MAX_LOBBY_CONNECTIONS; i++)
								{
									SSession::SRConnection* connectionAdd = &session->remoteConnection[i];

									if (connectionAdd->used && (connectionAdd != connection) && (connectionAdd->flags & CMMRC_FLAG_PLATFORM_DATA_VALID))
									{
										uint32 add = DrxLobbyPacketUINT32Size + DrxLobbyPacketUINT16Size + DrxLobbyPacketConnectionUIDSize +
										             DrxLobbyPacketUINT8Size + DrxLobbyPacketBoolSize + DRXLOBBY_USER_NAME_LENGTH + DRXLOBBY_USER_DATA_SIZE_IN_BYTES;

										if (testSize + add < MAX_LOBBY_PACKET_SIZE)
										{
											testSize += add;
											numConnections++;
										}
										else
										{
											break;
										}
									}
								}

								if (numConnections > 0)
								{
									// Add and send the connections
									packet.WriteUINT8(numConnections);

									for (i = connectionToAdd, numAdded = 0; (i < MAX_LOBBY_CONNECTIONS) && (numAdded < numConnections); i++, connectionToAdd++)
									{
										SSession::SRConnection* connectionAdd = &session->remoteConnection[i];

										if (connectionAdd->used && (connectionAdd != connection) && (connectionAdd->flags & CMMRC_FLAG_PLATFORM_DATA_VALID))
										{
											packet.WriteUINT32(htonl(connectionAdd->ip));
											packet.WriteUINT16(htons(connectionAdd->port));
											packet.WriteConnectionUID(connectionAdd->uid);
											packet.WriteUINT8(connectionAdd->numUsers);
											packet.WriteBool(connectionAdd->m_isDedicated);
											packet.WriteString(connectionAdd->name, DRXLOBBY_USER_NAME_LENGTH);
											packet.WriteData(connectionAdd->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
											numAdded++;

											NetLog("[Lobby] Send connection " PRFORMAT_MMCINFO " users to new connection " PRFORMAT_MMCINFO,
											       PRARG_MMCINFO(DrxMatchMakingConnectionID(i), connectionAdd->connectionID, connectionAdd->uid), PRARG_MMCINFO(id, connection->connectionID, connection->uid));
										}
									}

									Send(DrxMatchMakingInvalidTaskID, &packet, h, id);
								}
								else
								{
									// No more connections to send
									break;
								}
							}

							packet.FreeWriteBuffer();
						}
						else
						{
							FreeRemoteConnection(h, id);
							error = eCLE_ConnectionFailed;
						}
					}
					else
					{
						error = eCLE_SessionFull;
					}
				}
				else
				{
					error = eCLE_SessionFull;
				}
			}
			else
			{
				error = eCLE_ConnectionFailed;
			}
		}
		else
		{
			error = eCLE_ConnectionFailed;
		}
	}
	else
	{
		error = eCLE_ConnectionFailed;
	}

	if (error != eCLE_Success)
	{
		// Can't add to session so send back error
		const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT8Size + DrxLobbyPacketErrorSize;
		CDrxSharedLobbyPacket packet;

		if (packet.CreateWriteBuffer(MaxBufferSize))
		{
			packet.StartWrite(eLANPT_SessionRequestJoinResult, true);
			packet.WriteUINT8(returnTaskID);
			packet.WriteError(error);

			Send(DrxMatchMakingInvalidTaskID, &packet, h, addr);

			packet.FreeWriteBuffer();
		}
	}

	NetLog("[Lobby] Processed Session request join packet error %d", error);
}

void CDrxLANMatchMaking::ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	EDrxLobbyError error = eCLE_Success;
	DrxLobbyConnectionID c;

	if (m_lobby->ConnectionFromAddress(&c, addr))
	{
		DrxMatchMakingTaskID mmTaskID;

		pPacket->StartRead();
		mmTaskID = pPacket->ReadUINT8();
		mmTaskID = FindTaskFromTaskTaskID(eT_SessionJoin, mmTaskID);

		if (mmTaskID != DrxMatchMakingInvalidTaskID)
		{
			STask* task = &m_task[mmTaskID];
			DRXSOCKADDR_IN saddr;

			if (m_lobby->ConvertAddr(addr, &saddr))
			{
				error = pPacket->ReadError();

				NetLog("[Lobby] Received SessionRequestJoinResult error %d", error);

				UpdateTaskError(mmTaskID, error);

				if (task->error == eCLE_Success)
				{
					SSession* pSession = &m_sessions[task->session];
					uint8 numUsers;
					bool isDedicated;
					SDrxMatchMakingConnectionUID hostConnectionUID;
					char name[DRXLOBBY_USER_NAME_LENGTH];
					uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];

					DrxLobbySessionHandle h = pPacket->ReadLobbySessionHandle();
					DRX_ASSERT((pSession->id.m_h == DrxLobbyInvalidSessionHandle) || (pSession->id.m_h == h));
					pSession->id.m_h = h;
					pSession->localConnection.uid = pPacket->ReadConnectionUID();
					uint32 sessionCreateGameFlags = pPacket->ReadUINT16();
					pSession->createFlags = (pSession->createFlags & DRXSESSION_CREATE_FLAG_SYSTEM_MASK) | (sessionCreateGameFlags << DRXSESSION_CREATE_FLAG_GAME_FLAGS_SHIFT);
					pSession->sid = pSession->localConnection.uid.m_sid;

					bool hostMigrationSupported = m_lobby->GetLobbyServiceFlag(m_serviceType, eCLSF_SupportHostMigration);
					if ((pSession->createFlags & DRXSESSION_CREATE_FLAG_MIGRATABLE) && hostMigrationSupported)
					{
						pSession->localFlags |= DRXSESSION_LOCAL_FLAG_CAN_SEND_HOST_HINTS;
					}

					NetLog("[Lobby] Created local connection " PRFORMAT_SH " " PRFORMAT_UID, PRARG_SH(task->session), PRARG_UID(pSession->localConnection.uid));
					if ((!(pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)) && (pSession->localConnection.uid.m_uid > m_connectionUIDCounter))
					{
						// Keep all remote clients in sync with the host (in case they're chosen to be host during host migration)
						m_connectionUIDCounter = pSession->localConnection.uid.m_uid;
					}

					numUsers = pPacket->ReadUINT8();
					isDedicated = pPacket->ReadBool();
					hostConnectionUID = pPacket->ReadConnectionUID();
					pPacket->ReadString(name, DRXLOBBY_USER_NAME_LENGTH);
					pPacket->ReadData(userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

					DrxMatchMakingConnectionID id = AddRemoteConnection(task->session, c, hostConnectionUID, saddr.sin_addr.s_addr, saddr.sin_port, numUsers, name, userData, isDedicated);

					if (id != DrxMatchMakingInvalidConnectionID)
					{
						NetLog("[Lobby] Created server connection " PRFORMAT_MMCINFO, PRARG_MMCINFO(id, c, hostConnectionUID));
						StopTaskRunning(mmTaskID);
						pSession->hostConnectionID = id;
						SessionUserDataEvent(eCLSE_SessionUserJoin, h, id);
					}
					else
					{
						UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
					}
				}
			}
			else
			{
				UpdateTaskError(mmTaskID, eCLE_ConnectionFailed);
			}

			if (task->error != eCLE_Success)
			{
				StartSessionDelete(mmTaskID);
			}

			NetLog("[Lobby] Processed session request join result error %d", task->error);
		}
	}
}

void CDrxLANMatchMaking::ProcessSessionAddRemoteConnections(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	uint32 bufferPos = 0;
	DrxLobbySessionHandle h;

	pPacket->StartRead();
	h = pPacket->ReadLobbySessionHandle();

	h = FindSessionFromServerID(h);

	if (h != DrxLobbyInvalidSessionHandle)
	{
		SSession* session = &m_sessions[h];
		if ((session->localFlags & DRXSESSION_LOCAL_FLAG_HOST) == 0)
		{
			uint8 numConnections = pPacket->ReadUINT8();

			for (uint32 i = 0; i < numConnections; i++)
			{
				char name[DRXLOBBY_USER_NAME_LENGTH];
				uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
				uint32 ip;
				uint16 port;
				uint8 numUsers;
				bool isDedicated;
				SDrxMatchMakingConnectionUID connectionUID;

				ip = pPacket->ReadUINT32();
				port = pPacket->ReadUINT16();
				connectionUID = pPacket->ReadConnectionUID();
				numUsers = pPacket->ReadUINT8();
				isDedicated = pPacket->ReadBool();
				pPacket->ReadString(name, DRXLOBBY_USER_NAME_LENGTH);
				pPacket->ReadData(userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

				DrxMatchMakingConnectionID id = AddRemoteConnection(h, DrxLobbyInvalidConnectionID, connectionUID, ntohl(ip), ntohs(port), numUsers, name, userData, isDedicated);

				if (id != DrxMatchMakingInvalidConnectionID)
				{
					NetLog("[Lobby] Add new connection " PRFORMAT_SHMMCINFO, PRARG_SHMMCINFO(h, id, session->remoteConnection[id].connectionID, connectionUID));
					SessionUserDataEvent(eCLSE_SessionUserJoin, h, id);
				}
			}
		}
		else
		{
			NetLog("[Lobby] CDrxLANMatchMaking::ProcessSessionAddRemoteConnections() packet received on " PRFORMAT_SH " but we're the host!  Ignoring", PRARG_SH(h));
		}
	}
}

size_t CDrxLANMatchMaking::CalculateServerDataSize(DrxLobbySessionHandle h)
{
	size_t size =
	  DrxLobbyPacketUnReliableHeaderSize +
	  DrxLobbyPacketUINT8Size +              // Requesters task id
	  DrxLobbyPacketLobbySessionHandleSize + // Session handle
	  DrxLobbyPacketUINT32Size +             // Num filled slots
	  DrxLobbyPacketUINT32Size +             // Num public slots
	  DrxLobbyPacketUINT32Size +             // Num private slots
	  MAX_SESSION_NAME_LENGTH +              // Session name
	  DrxLobbyPacketBoolSize;                // Ranked

	for (uint32 i = 0; i < m_sessions[h].data.m_numData; i++)
	{
		switch (m_sessions[h].data.m_data[i].m_type)
		{
		case eCLUDT_Int64:
		case eCLUDT_Float64:
		case eCLUDT_Int64NoEndianSwap:
			size += DrxLobbyPacketUINT64Size;
			break;

		case eCLUDT_Int32:
		case eCLUDT_Float32:
			size += DrxLobbyPacketUINT32Size;
			break;

		case eCLUDT_Int16:
			size += DrxLobbyPacketUINT16Size;
			break;

		case eCLUDT_Int8:
			size += DrxLobbyPacketUINT8Size;
			break;
		}
	}

	return size;
}

size_t CDrxLANMatchMaking::CalculateServerDataSize() const
{
	size_t size =
	  DrxLobbyPacketUnReliableHeaderSize +
	  DrxLobbyPacketUINT8Size +              // Requesters task id
	  DrxLobbyPacketLobbySessionHandleSize + // Session handle
	  DrxLobbyPacketUINT32Size +             // Num filled slots
	  DrxLobbyPacketUINT32Size +             // Num public slots
	  DrxLobbyPacketUINT32Size +             // Num private slots
	  MAX_SESSION_NAME_LENGTH +              // Session name
	  DrxLobbyPacketBoolSize;                // Ranked

	for (uint32 i = 0; i < m_registeredUserData.num; i++)
	{
		switch (m_registeredUserData.data[i].m_type)
		{
		case eCLUDT_Int64:
		case eCLUDT_Float64:
		case eCLUDT_Int64NoEndianSwap:
			size += DrxLobbyPacketUINT64Size;
			break;

		case eCLUDT_Int32:
		case eCLUDT_Float32:
			size += DrxLobbyPacketUINT32Size;
			break;

		case eCLUDT_Int16:
			size += DrxLobbyPacketUINT16Size;
			break;

		case eCLUDT_Int8:
			size += DrxLobbyPacketUINT8Size;
			break;
		}
	}

	return size;
}

void CDrxLANMatchMaking::SendServerData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	LOBBY_AUTO_LOCK;
	uint8 requestersTaskID;

	pPacket->StartRead();
	requestersTaskID = pPacket->ReadUINT8();

	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		SSession* pSession = &m_sessions[i];
		if (((pSession->localFlags & (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST)) == (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST)) && (pSession->createFlags & DRXSESSION_CREATE_FLAG_SEARCHABLE))
		{
			uint32 bufferSize = CalculateServerDataSize(i);
			assert(bufferSize == CalculateServerDataSize() && "Expected data size not identical between sender and receiver, server will not be discoverable");
			CDrxSharedLobbyPacket packet;

			if (packet.CreateWriteBuffer(bufferSize))
			{
				packet.StartWrite(eLANPT_MM_ServerData, false);
				packet.WriteUINT8(requestersTaskID);
				packet.WriteLobbySessionHandle(i);
				packet.WriteUINT32(pSession->numFilledSlots);
				packet.WriteUINT32(pSession->data.m_numPublicSlots);
				packet.WriteUINT32(pSession->data.m_numPrivateSlots);
				packet.WriteData(pSession->data.m_name, MAX_SESSION_NAME_LENGTH);
				packet.WriteBool(pSession->data.m_ranked);

				for (uint32 j = 0; j < pSession->data.m_numData; j++)
				{
					packet.WriteDrxLobbyUserData(&pSession->data.m_data[j]);
				}

				assert(packet.GetWriteBufferPos() == bufferSize && "Written data size doesn't match expected data size");

				Send(DrxMatchMakingInvalidTaskID, &packet, i, addr);
				packet.FreeWriteBuffer();
			}
		}
	}
}

void CDrxLANMatchMaking::ServerDataToGame(DrxMatchMakingTaskID mmTaskID, uint32 ip, uint16 port, TMemHdl params, uint32 length)
{
	LOBBY_AUTO_LOCK;

	SDrxLANSessionID* id = NULL;
	const uint32 bufferSize = CalculateServerDataSize();
	if (length == bufferSize) // Guard against incompatible discovery response (likely different engine version)
	{
		id = new SDrxLANSessionID;
	}
	else
	{
		CNetAddressResolver* pResolver = m_lobby->GetResolver();
		if (pResolver)
		{
			SIPv4Addr addr(uint32(htonl(ip)), port);
			TAddressString ipStr = pResolver->ToNumericString(addr);
			DrxLogAlways("[Lobby] Session at %s is incompatible, not returned", ipStr.c_str());
		}
	}
	if (id)
	{
		STask* pTask = &m_task[mmTaskID];
		SDrxSessionSearchResult result;
		SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA];
		CDrxSharedLobbyPacket packet;
		uint8 taskID8;

		packet.SetReadBuffer((uint8*)m_lobby->MemGetPtr(params), length);
		packet.ReadPacketHeader();
		packet.StartRead();
		taskID8 = packet.ReadUINT8();

		id->m_ip = ip;
		id->m_port = port;
		id->m_h = packet.ReadLobbySessionHandle();
		result.m_id = id;

		result.m_numFilledSlots = packet.ReadUINT32();
		result.m_data.m_numPublicSlots = packet.ReadUINT32();
		result.m_data.m_numPrivateSlots = packet.ReadUINT32();
		packet.ReadData(result.m_data.m_name, MAX_SESSION_NAME_LENGTH);
		result.m_data.m_ranked = packet.ReadBool();

		result.m_data.m_data = userData;
		result.m_data.m_numData = m_registeredUserData.num;

		result.m_ping = 0;
		result.m_numFriends = 0;
		result.m_flags = 0;

		for (uint32 i = 0; i < m_registeredUserData.num; i++)
		{
			result.m_data.m_data[i].m_id = m_registeredUserData.data[i].m_id;
			result.m_data.m_data[i].m_type = m_registeredUserData.data[i].m_type;
			packet.ReadDrxLobbyUserData(&result.m_data.m_data[i]);
		}

		assert(packet.GetReadBufferPos() == bufferSize && "Read data size doesn't match expected data size");

		bool filterOk = true;
		SDrxSessionSearchParam* pParam = static_cast<SDrxSessionSearchParam*>(m_lobby->MemGetPtr(pTask->params[SEARCH_PARAM_GAME_PARAM]));
		SDrxSessionSearchData* pData = static_cast<SDrxSessionSearchData*>(m_lobby->MemGetPtr(pTask->params[SEARCH_PARAM_GAME_PARAM_DATA]));
		if (pParam && pData)
		{
			filterOk = ParamFilter(pParam->m_numData, pData, &result);
		}

		if (pTask->cb && filterOk)
		{
			((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &result, pTask->cbArg);
		}
	}

	m_lobby->MemFree(params);
}

bool CDrxLANMatchMaking::ParamFilter(uint32 nParams, const SDrxSessionSearchData* pParams, const SDrxSessionSearchResult* pResult)
{
	for (uint32 i = 0; i < nParams; i++)
	{
		const SDrxSessionSearchData* pParam = &pParams[i];

		for (uint32 j = 0; j < pResult->m_data.m_numData; j++)
		{
			const SDrxLobbyUserData* pData = &pResult->m_data.m_data[j];

			if (pData->m_id == pParam->m_data.m_id)
			{
				bool bOk = true;

				if (pData->m_type != pParam->m_data.m_type)
				{
					return false;
				}

				switch (pParam->m_operator)
				{
				case eCSSO_Equal:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 == pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 == pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 == pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 == pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 == pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 == pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_NotEqual:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 != pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 != pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 != pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 != pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 != pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 != pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_LessThan:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 < pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 < pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 < pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 < pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 < pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 < pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_LessThanEqual:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 <= pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 <= pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 <= pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 <= pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 <= pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 <= pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_GreaterThan:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 > pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 > pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 > pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 > pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 > pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 > pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_GreaterThanEqual:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = (pData->m_int64 >= pParam->m_data.m_int64);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = (pData->m_int32 >= pParam->m_data.m_int32);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = (pData->m_int16 >= pParam->m_data.m_int16);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = (pData->m_int8 >= pParam->m_data.m_int8);
							}
							break;
						case eCLUDT_Float64:
							{
								bOk = (pData->m_f64 >= pParam->m_data.m_f64);
							}
							break;
						case eCLUDT_Float32:
							{
								bOk = (pData->m_f32 >= pParam->m_data.m_f32);
							}
							break;
						default:
							break;
						}
					}
					break;
				case eCSSO_BitwiseAndNotEqualZero:
					{
						switch (pParam->m_data.m_type)
						{
						case eCLUDT_Int64:
						case eCLUDT_Int64NoEndianSwap:
							{
								bOk = ((pData->m_int64 & pParam->m_data.m_int64) != 0);
							}
							break;
						case eCLUDT_Int32:
							{
								bOk = ((pData->m_int32 & pParam->m_data.m_int32) != 0);
							}
							break;
						case eCLUDT_Int16:
							{
								bOk = ((pData->m_int16 & pParam->m_data.m_int16) != 0);
							}
							break;
						case eCLUDT_Int8:
							{
								bOk = ((pData->m_int8 & pParam->m_data.m_int8) != 0);
							}
							break;
						case eCLUDT_Float64:
						case eCLUDT_Float32:
							{
								DRX_ASSERT_MESSAGE(0, "eCSSO_BitwiseAndNotEqualZero not supported on floating point numbers.");
								bOk = false;
							}
							break;
						default:
							break;
						}
					}
					break;
				default:
					break;
				}

				if (bOk == false)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void CDrxLANMatchMaking::ProcessServerData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	DRXSOCKADDR_IN sockAddr;

	LOBBY_AUTO_LOCK;

	if (m_lobby->ConvertAddr(addr, &sockAddr))
	{
		pPacket->StartRead();
		DrxMatchMakingTaskID taskID = pPacket->ReadUINT8();

		if ((taskID < MAX_MATCHMAKING_TASKS) && m_task[taskID].used && m_task[taskID].running && ((m_task[taskID].startedTask == eT_SessionQuery) || (m_task[taskID].startedTask == eT_SessionSearch)))
		{
			STask* pTask = &m_task[taskID];
			uint32 ip = sockAddr.sin_addr.s_addr;
			uint16 port = ntohs(sockAddr.sin_port);

			if (m_task[taskID].startedTask == eT_SessionSearch)
			{
				SSearch* pSearch = &m_search[pTask->search];

				if (pSearch->numServers < MAX_LAN_SEARCH_SERVERS)
				{
					for (uint32 i = 0; i < pSearch->numServers; i++)
					{
						if ((pSearch->servers[i].m_ip == ip) && (pSearch->servers[i].m_port == port))
						{
							return;
						}
					}

					pSearch->servers[pSearch->numServers].m_ip = ip;
					pSearch->servers[pSearch->numServers].m_port = port;
					pSearch->numServers++;

					NetLog("[Lobby] Found Session %u.%u.%u.%u:%u", ((uint8*)&ip)[0], ((uint8*)&ip)[1], ((uint8*)&ip)[2], ((uint8*)&ip)[3], port);
				}
			}

			TMemHdl params = m_lobby->MemAlloc(pPacket->GetReadBufferSize());
			memcpy(m_lobby->MemGetPtr(params), pPacket->GetReadBuffer(), pPacket->GetReadBufferSize());
			TO_GAME_FROM_LOBBY(&CDrxLANMatchMaking::ServerDataToGame, this, taskID, ip, port, params, pPacket->GetReadBufferSize());
		}
	}
}

void CDrxLANMatchMaking::EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->error == eCLE_Success)
	{
		SDrxUserInfoResult temp;
		int a;

		// Glue in local user(s)
		SDrxLANUserID* pID = new SDrxLANUserID;

		if (pID)
		{
			pID->id = GetLANUserID(m_sessions[pTask->session].localConnection.uid);
			temp.m_userID = pID;
			temp.m_conID = m_sessions[pTask->session].localConnection.uid;
			temp.m_isDedicated = gEnv->IsDedicated() && (m_sessions[pTask->session].localFlags & DRXSESSION_LOCAL_FLAG_HOST);
			drx_strcpy(temp.m_userName, m_sessions[pTask->session].localConnection.name);
			memcpy(temp.m_userData, m_sessions[pTask->session].localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
			((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &temp, pTask->cbArg);
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
		}

		for (a = 0; a < MAX_LOBBY_CONNECTIONS; a++)
		{
			if (m_sessions[pTask->session].remoteConnection[a].used)
			{
				pID = new SDrxLANUserID;

				if (pID)
				{
					pID->id = GetLANUserID(m_sessions[pTask->session].remoteConnection[a].uid);
					temp.m_userID = pID;
					temp.m_conID = m_sessions[pTask->session].remoteConnection[a].uid;
					temp.m_isDedicated = m_sessions[pTask->session].remoteConnection[a].m_isDedicated;
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

		((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_Success, NULL, pTask->cbArg);
	}
	else
	{
		((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
	}
}

DrxSessionID CDrxLANMatchMaking::SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h)
{
	return NULL;
}

void CDrxLANMatchMaking::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;

	switch (pSPacket->StartRead())
	{
	case eLANPT_MM_RequestServerData:
		SendServerData(addr, pSPacket);
		break;

	case eLANPT_MM_ServerData:
		ProcessServerData(addr, pSPacket);
		break;

	case eLANPT_SessionRequestJoin:
		ProcessSessionRequestJoin(addr, pSPacket);
		break;

	case eLANPT_SessionRequestJoinResult:
		ProcessSessionRequestJoinResult(addr, pSPacket);
		break;

	case eLANPT_SessionAddRemoteConnections:
		ProcessSessionAddRemoteConnections(addr, pSPacket);
		break;

	#if NETWORK_HOST_MIGRATION
	case eLANPT_HostMigrationStart:
		ProcessHostMigrationStart(addr, pSPacket);
		break;

	case eLANPT_HostMigrationServer:
		ProcessHostMigrationFromServer(addr, pSPacket);
		break;

	case eLANPT_HostMigrationClient:
		ProcessHostMigrationFromClient(addr, pSPacket);
		break;
	#endif

	case eLANPT_UserData:
		ProcessLocalUserData(addr, pSPacket);
		break;

	default:
		CDrxMatchMaking::OnPacket(addr, pSPacket);
		break;
	}
}

	#if NETWORK_HOST_MIGRATION
void CDrxLANMatchMaking::HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause)
{
	SSession* pSession = &m_sessions[h];

	pSession->hostMigrationInfo.Reset();

	if (pSession->newHostUID.m_uid == pSession->localConnection.uid.m_uid)
	{
		// New host is me so no need to be told the new host address on LAN
		pSession->hostMigrationInfo.m_newHostAddress = TNetAddress(TLocalNetAddress());
		pSession->hostMigrationInfo.m_newHostAddressValid = true;
	}
}

EDrxLobbyError CDrxLANMatchMaking::HostMigrationServer(SHostMigrationInfo* pInfo)
{
	EDrxLobbyError error = eCLE_Success;

	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(pInfo->m_session);
	if (h != DrxLobbyInvalidSessionHandle)
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		pSession->localFlags |= DRXSESSION_LOCAL_FLAG_HOST;
		pSession->hostConnectionID = DrxMatchMakingInvalidConnectionID;
		error = StartTask(eT_SessionMigrateHostServer, &mmTaskID, NULL, h, NULL, NULL);

		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[mmTaskID];
		#if HOST_MIGRATION_SOAK_TEST_BREAK_DETECTION
			DetectHostMigrationTaskBreak(h, "HostMigrationServer()");
		#endif
			pSession->hostMigrationInfo.m_taskID = pTask->lTaskID;

			NetLog("[Host Migration]: matchmaking migrating " PRFORMAT_SH " on the SERVER", PRARG_SH(h));
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[Host Migration]: CDrxLANMatchMaking::HostMigrationServer(): " PRFORMAT_SH ", error %d", PRARG_SH(h), error);
	}

	return error;
}

void CDrxLANMatchMaking::HostMigrationServerNT(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	if (pTask->canceled)
	{
		NetLog("[Host Migration]: HostMigrationServerNT() cancelled for " PRFORMAT_SH " - bailing", PRARG_SH(pTask->session));
		StopTaskRunning(mmTaskID);
		return;
	}

	NetLog("[Host Migration]: matchmaking SERVER migrated session successfully - informing CLIENTS");

	const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT32Size;
	CDrxSharedLobbyPacket packet;

	if (packet.CreateWriteBuffer(MaxBufferSize))
	{
		packet.StartWrite(eLANPT_HostMigrationServer, true);
		packet.WriteUINT32(mmTaskID);

		SendToAll(mmTaskID, &packet, pTask->session, pSession->serverConnectionID);

		pSession->hostMigrationInfo.m_sessionMigrated = true;
	}

	StartSubTask(eT_SessionMigrateHostFinish, mmTaskID);
	pTask->StartTimer();
}

EDrxLobbyError CDrxLANMatchMaking::HostMigrationClient(DrxLobbySessionHandle h, DrxMatchMakingTaskID hostTaskID)
{
	EDrxLobbyError error = eCLE_Success;

	if ((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[h];
		DrxMatchMakingTaskID mmTaskID;

		pSession->localFlags &= ~DRXSESSION_LOCAL_FLAG_HOST;
		error = StartTask(eT_SessionMigrateHostClient, &mmTaskID, NULL, h, NULL, NULL);

		if (error == eCLE_Success)
		{
			NetLog("[Host Migration]: matchmaking migrating to the new session on the CLIENT");

			STask* pTask = &m_task[mmTaskID];
			pTask->returnTaskID = hostTaskID;
		#if HOST_MIGRATION_SOAK_TEST_BREAK_DETECTION
			DetectHostMigrationTaskBreak(h, "HostMigrationClient()");
		#endif
			pSession->hostMigrationInfo.m_taskID = pTask->lTaskID;
			StartTaskRunning(mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[Host Migration]: CDrxLANMatchMaking::HostMigrationClient() error %d", error);
	}

	return error;

}

void CDrxLANMatchMaking::TickHostMigrationClientNT(DrxMatchMakingTaskID mmTaskID)
{
	NetLog("[Host Migration]: matchmaking CLIENT migrated to new session successfully");

	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	SSession::SRConnection* pConnection = &pSession->remoteConnection[pSession->hostConnectionID];

	if (m_lobby->ConnectionGetState(pConnection->connectionID) == eCLCS_Connected)
	{
		const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT32Size;
		CDrxSharedLobbyPacket packet;

		if (packet.CreateWriteBuffer(MaxBufferSize))
		{
			packet.StartWrite(eLANPT_HostMigrationClient, true);
			packet.WriteUINT32(pTask->returnTaskID);

			Send(mmTaskID, &packet, pTask->session, pSession->hostConnectionID);
		}

		NetLog("[Host Migration]: matchmaking CLIENT migrated to new " PRFORMAT_SH " successfully", PRARG_SH(pTask->session));
		pSession->hostMigrationInfo.m_sessionMigrated = true;
		pSession->hostMigrationInfo.m_matchMakingFinished = true;
		pSession->hostMigrationInfo.m_newHostAddressValid = true;
	}
	else
	{
		NetLog("[Host Migration]: matchmaking CLIENT migrated to new " PRFORMAT_SH " successfully, but host disconnected", PRARG_SH(pTask->session));
	}

	StopTaskRunning(mmTaskID);
}

void CDrxLANMatchMaking::ProcessHostMigrationFromServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	uint32 bufferPos = 0;
	DrxLobbySessionHandle h = DrxLobbyInvalidSessionHandle;

	pPacket->StartRead();
	DrxMatchMakingTaskID hostTaskID = pPacket->ReadUINT32();

	SDrxMatchMakingConnectionUID mmCxUID = pPacket->GetFromConnectionUID();
	DrxMatchMakingConnectionID mmCxID = DrxMatchMakingInvalidConnectionID;

	if (FindConnectionFromUID(mmCxUID, &h, &mmCxID))
	{
		SSession* pSession = &m_sessions[h];
		pSession->hostConnectionID = mmCxID;

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			NetLog("[Host Migration]: ignoring session details from " PRFORMAT_SHMMCINFO " as I am already the server " PRFORMAT_UID,
			       PRARG_SHMMCINFO(h, mmCxID, pSession->remoteConnection[mmCxID].connectionID, mmCxUID), PRARG_UID(pSession->localConnection.uid));
		}
		else
		{
			NetLog("[Host Migration]: matchmaking CLIENT received session details from " PRFORMAT_SHMMCINFO " - migrating to new LAN session",
			       PRARG_SHMMCINFO(h, mmCxID, pSession->remoteConnection[mmCxID].connectionID, mmCxUID));
			HostMigrationClient(h, hostTaskID);

			if (m_lobby)
			{
				TMemHdl mh = m_lobby->MemAlloc(sizeof(SDrxLobbyRoomOwnerChanged));
				if (mh != TMemInvalidHdl)
				{
					SDrxLobbyRoomOwnerChanged* pOwnerData = (SDrxLobbyRoomOwnerChanged*)m_lobby->MemGetPtr(mh);

					pOwnerData->m_session = CreateGameSessionHandle(h, pSession->localConnection.uid);
					m_lobby->DecodeAddress(addr, &pOwnerData->m_ip, &pOwnerData->m_port);
					pOwnerData->m_ip = htonl(pOwnerData->m_ip);

					TO_GAME_FROM_LOBBY(&CDrxLANMatchMaking::DispatchRoomOwnerChangedEvent, this, mh);
				}
			}

			pSession->hostMigrationInfo.m_newHostAddress = addr;
			return;
		}
	}
	else
	{
		NetLog("[Host Migration]: matchmaking CLIENT received session details from unknown connection " PRFORMAT_ADDR " - ignoring", PRARG_ADDR(addr));
	}

	// If HostMigrationClient() was not called, this client will be pruned (after timeout) on the new host
	DRX_ASSERT_MESSAGE(false, "ProcessHostMigrationFromServer failed to call HostMigrationClient");
}

void CDrxLANMatchMaking::ProcessHostMigrationFromClient(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	uint32 bufferPos = 0;

	pPacket->StartRead();
	DrxMatchMakingTaskID hostTaskID = pPacket->ReadUINT32();

	DrxLobbyConnectionID c;
	if (m_lobby->ConnectionFromAddress(&c, addr))
	{
		hostTaskID = FindTaskFromTaskTaskID(eT_SessionMigrateHostFinish, hostTaskID);
		if (hostTaskID != DrxMatchMakingInvalidTaskID)
		{
			STask* pTask = &m_task[hostTaskID];
			SSession* pSession = &m_sessions[pTask->session];

			for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
			{
				SSession::SRConnection* connection = &pSession->remoteConnection[i];

				if (connection->used && (connection->connectionID == c))
				{
					connection->m_migrated = true;
					float timetaken = (g_time - pSession->hostMigrationInfo.m_startTime).GetSeconds();
					NetLog("[Host Migration]: matchmaking SERVER received CLIENT confirmation " PRFORMAT_SHMMCINFO " @ %fs", PRARG_SHMMCINFO(pTask->session, DrxMatchMakingConnectionID(i), connection->connectionID, connection->uid), timetaken);
					return;
				}
			}
		}
	}

	SDrxMatchMakingConnectionUID mmCxUID = pPacket->GetFromConnectionUID();
	NetLog("[Host Migration]: matchmaking SERVER received CLIENT confirmation for unknown connection " PRFORMAT_ADDR " " PRFORMAT_UID, PRARG_ADDR(addr), PRARG_UID(mmCxUID));
}

void CDrxLANMatchMaking::TickHostMigrationFinishNT(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	bool finished = true;

	for (uint32 index = 0; finished && index < MAX_LOBBY_CONNECTIONS; ++index)
	{
		if (pSession->remoteConnection[index].used && !pSession->remoteConnection[index].m_migrated && !pSession->remoteConnection[index].uid.m_uid == DEDICATED_SERVER_CONNECTION_UID)
		{
			finished = false;
		}
	}

	float timeout = CLobbyCVars::Get().hostMigrationTimeout - (DrxMatchMakingHostMigratedConnectionConfirmationWindow / 1000.0f);
	float timetaken = (g_time - pSession->hostMigrationInfo.m_startTime).GetSeconds();

	if (finished || (pTask->canceled) || (timetaken > timeout))
	{
		NetLog("[Host Migration]: matchmaking host migration finished for " PRFORMAT_SH, PRARG_SH(pTask->session));

		for (uint32 index = 0; index < MAX_LOBBY_CONNECTIONS; ++index)
		{
			SSession::SRConnection* pConnection = &pSession->remoteConnection[index];

			if (pConnection->used && pConnection->uid.m_uid != DEDICATED_SERVER_CONNECTION_UID)
			{
				if (pConnection->m_migrated)
				{
					// This connection migrated so reset for next time
					NetLog("[Host Migration]: client " PRFORMAT_SHMMCINFO " migrated successfully @ %fs", PRARG_SHMMCINFO(pTask->session, DrxMatchMakingConnectionID(index), pConnection->connectionID, pConnection->uid), timetaken);
					pConnection->m_migrated = false;
				}
				else
				{
					// Prune this connection
					if (CLobbyCVars::Get().netPruneConnectionsAfterHostMigration != 0)
					{
						NetLog("[Host Migration]: client " PRFORMAT_SHMMCINFO " failed to migrate - pruning @ %fs", PRARG_SHMMCINFO(pTask->session, DrxMatchMakingConnectionID(index), pConnection->connectionID, pConnection->uid), timetaken);
						SessionDisconnectRemoteConnectionViaNub(pTask->session, index, eDS_Local, DrxMatchMakingInvalidConnectionID, eDC_FailedToMigrateToNewHost, "Failed to migrate player to new game");
					}
					else
					{
						NetLog("[Host Migration]: client " PRFORMAT_SHMMCINFO " failed to migrate but pruning disabled @ %fs", PRARG_SHMMCINFO(pTask->session, DrxMatchMakingConnectionID(index), pConnection->connectionID, pConnection->uid), timetaken);
					}
				}
			}
		}

		pSession->hostMigrationInfo.m_matchMakingFinished = true;
		StopTaskRunning(mmTaskID);
	}
}

bool CDrxLANMatchMaking::GetNewHostAddress(char* address, SHostMigrationInfo* pInfo)
{
	DrxLobbySessionHandle sessionIndex = GetSessionHandleFromGameSessionHandle(pInfo->m_session);
	SSession* pSession = &m_sessions[sessionIndex];
	bool success = pSession->hostMigrationInfo.m_newHostAddressValid;

	if (success)
	{
		if (pSession->serverConnectionID != DrxMatchMakingInvalidConnectionID)
		{
			success = (pSession->desiredHostUID != DrxMatchMakingInvalidConnectionUID);

			if (success)
			{
				DrxMatchMakingConnectionID connID = DrxMatchMakingInvalidConnectionID;

				if (pSession->localConnection.uid == pSession->desiredHostUID)
				{
					pSession->hostMigrationInfo.SetIsNewHost(true);
					pSession->newHostUID = pSession->desiredHostUID;
					NetLog("[Host Migration]: " PRFORMAT_SH " becoming the new host...", PRARG_SH(sessionIndex));
				}
				else if (FindConnectionFromSessionUID(sessionIndex, pSession->desiredHostUID, &connID))
				{
					pSession->hostMigrationInfo.SetIsNewHost(false);
					pSession->newHostUID = pSession->desiredHostUID;
					NetLog("[Host Migration]: " PRFORMAT_SH " new host is " PRFORMAT_UID, PRARG_SH(sessionIndex), PRARG_UID(pSession->newHostUID));
				}
				else
				{
					success = false;
					NetLog("[Host Migration]: " PRFORMAT_SH " new host is " PRFORMAT_UID " but it's not here", PRARG_SH(sessionIndex), PRARG_UID(pSession->newHostUID));
				}
			}

			return success;
		}
	}

	if (success)
	{
		// Decode the address into an ip string
		if (stl::get_if<TLocalNetAddress>(&pSession->hostMigrationInfo.m_newHostAddress))
		{
			pSession->hostMigrationInfo.SetIsNewHost(true);
			NetLog("[Host Migration]: " PRFORMAT_SH " becoming the new host...", PRARG_SH(sessionIndex));

			const SDrxLobbyParameters& lobbyParams = m_lobby->GetLobbyParameters();
			uint16 port = lobbyParams.m_listenPort;
			drx_sprintf(address, HOST_MIGRATION_MAX_SERVER_NAME_SIZE, "%s:%u", LOCAL_CONNECTION_STRING, port);
		}
		else
		{
			pSession->hostMigrationInfo.SetIsNewHost(false);
			NetLog("[Host Migration]: " PRFORMAT_SH " new host is " PRFORMAT_UID, PRARG_SH(sessionIndex), PRARG_UID(pSession->newHostUID));
			SIPv4Addr* pAddr = stl::get_if<SIPv4Addr>(&pSession->hostMigrationInfo.m_newHostAddress);
			if (pAddr)
			{
				pSession->id.m_ip = ntohl(pAddr->addr);
				pSession->id.m_port = pAddr->port;
			}
			m_lobby->DecodeAddress(pSession->hostMigrationInfo.m_newHostAddress, address, false);
		}
	}

	return success;
}

void CDrxLANMatchMaking::HostMigrationStartNT(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	DrxLobbySessionHandle h = pTask->session;
	SSession* pSession = &m_sessions[h];

	// Ensure we're the host and we've not started the session
	if ((pSession->localFlags & (DRXSESSION_LOCAL_FLAG_HOST | DRXSESSION_LOCAL_FLAG_STARTED)) == DRXSESSION_LOCAL_FLAG_HOST)
	{
		// Determine if host hinting has a better host
		SHostHintInformation newHost;
		DrxMatchMakingConnectionID newHostID;
		if (FindConnectionFromSessionUID(pTask->session, pSession->newHostUID, &newHostID))
		{
			SSession::SRConnection* pConnection = &pSession->remoteConnection[newHostID];
			newHost = pConnection->hostHintInfo;
		}

		bool isBetterHost = false;
		if (SortNewHostPriorities_ComparatorHelper(newHost, pSession->hostHintInfo) < 0)
		{
			isBetterHost = true;
		}

		if (isBetterHost)
		{
			if (HostMigrationInitiate(h, eDC_Unknown))
			{
				// I'm currently the host but there's a better host out there...
				const uint32 maxBufferSize = DrxLobbyPacketHeaderSize;
				uint8 buffer[maxBufferSize];
				CDrxLobbyPacket packet;

				NetLog("[Host Migration]: local server is not best host for " PRFORMAT_SH " - instructing clients to choose new host", PRARG_SH(pTask->session));
				packet.SetWriteBuffer(buffer, maxBufferSize);
				packet.StartWrite(eLANPT_HostMigrationStart, true);

				SendToAll(mmTaskID, &packet, h, DrxMatchMakingInvalidConnectionID);
				pSession->localFlags &= ~DRXSESSION_LOCAL_FLAG_HOST;
			}
			else
			{
				NetLog("[Host Migration]: local server is not best host for " PRFORMAT_SH ", but session is not migratable", PRARG_SH(pTask->session));
				UpdateTaskError(mmTaskID, eCLE_SessionNotMigratable);
				StopTaskRunning(mmTaskID);
			}
		}
		else
		{
			NetLog("[Host Migration]: local server is best host for " PRFORMAT_SH, PRARG_SH(pTask->session));
		}
	}
	else
	{
		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_STARTED)
		{
			NetLog("[Host Migration]: SessionEnsureBestHost() called after the session had started");
		}
	}
}

void CDrxLANMatchMaking::TickHostMigrationStartNT(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	StopTaskRunning(mmTaskID);
}

void CDrxLANMatchMaking::ProcessHostMigrationStart(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	DrxLobbySessionHandle h = DrxLobbyInvalidSessionHandle;
	DrxMatchMakingConnectionID mmCxID = DrxMatchMakingInvalidConnectionID;

	SDrxMatchMakingConnectionUID mmCxUID = pPacket->GetFromConnectionUID();

	if (FindConnectionFromUID(mmCxUID, &h, &mmCxID))
	{
		SSession* pSession = &m_sessions[h];

		NetLog("[Host Migration]: received host migration push event for " PRFORMAT_SHMMCINFO, PRARG_SHMMCINFO(h, mmCxID, pSession->remoteConnection[mmCxID].connectionID, mmCxUID));

		if (!(pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST))
		{
			if (pSession->hostMigrationInfo.m_state == eHMS_Idle)
			{
				NetLog("[Host Migration]: remote server is not best host for " PRFORMAT_SH " - instructed to choose new host", PRARG_SH(h));
				HostMigrationInitiate(h, eDC_Unknown);
			}
			else
			{
				NetLog("[Host Migration]: ignoring pushed migration event - already migrating " PRFORMAT_SH, PRARG_SH(h));
			}
		}
	}
	else
	{
		NetLog("[Host Migration]: received push migration event from unknown host, " PRFORMAT_ADDR " " PRFORMAT_UID, PRARG_ADDR(addr), PRARG_UID(mmCxUID));
	}
}
	#endif

EDrxLobbyError CDrxLANMatchMaking::SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError rc = eCLE_Success;

	#if NETWORK_HOST_MIGRATION
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h = GetSessionHandleFromGameSessionHandle(gh);

	if (h != DrxLobbyInvalidSessionHandle)
	{
		DrxMatchMakingTaskID mmTaskID;
		SSession* pSession = &m_sessions[h];
		rc = StartTask(eT_SessionMigrateHostStart, &mmTaskID, pTaskID, h, (void*)pCB, pCBArg);

		if (rc == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxLANMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	#endif

	return rc;
}

void CDrxLANMatchMaking::SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];

	if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED)
	{
		if (id == DrxMatchMakingInvalidConnectionID)
		{
			SDrxLANUserID* pID = new SDrxLANUserID;

			if (pID)
			{
				SSession::SLConnection* pConnection = &pSession->localConnection;
				if (pConnection->numUsers > 0)
				{
					SDrxUserInfoResult userInfo;

					pID->id = GetLANUserID(pConnection->uid);
					userInfo.m_userID = pID;
					userInfo.m_conID = pConnection->uid;
					userInfo.m_isDedicated = gEnv->IsDedicated() && (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST);
					drx_strcpy(userInfo.m_userName, pConnection->name);
					memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

					CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
				}
			}
		}
		else
		{
			SDrxLANUserID* pID = new SDrxLANUserID;

			if (pID)
			{
				SSession::SRConnection* pConnection = &pSession->remoteConnection[id];
				if (pConnection->numUsers > 0)
				{
					SDrxUserInfoResult userInfo;

					pID->id = GetLANUserID(pConnection->uid);
					userInfo.m_userID = pID;
					userInfo.m_conID = pConnection->uid;
					userInfo.m_isDedicated = pConnection->m_isDedicated;
					drx_strcpy(userInfo.m_userName, pConnection->name);
					memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

					CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
				}
				else
				{
					InitialDispatchUserPackets(h, id);
					pConnection->gameInformedConnectionEstablished = true;
				}
			}
		}
	}
}

void CDrxLANMatchMaking::InitialUserDataEvent(DrxLobbySessionHandle h)
{
	SSession* pSession = &m_sessions[h];

	pSession->localFlags |= DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED;
	SessionUserDataEvent(eCLSE_SessionUserJoin, h, DrxMatchMakingInvalidConnectionID);

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

		if (pConnection->used)
		{
			SessionUserDataEvent(eCLSE_SessionUserJoin, h, i);
		}
	}
}

void CDrxLANMatchMaking::DispatchRoomOwnerChangedEvent(TMemHdl mh)
{
	UDrxLobbyEventData eventData;
	eventData.pRoomOwnerChanged = (SDrxLobbyRoomOwnerChanged*)m_lobby->MemGetPtr(mh);

	m_lobby->DispatchEvent(eCLSE_RoomOwnerChanged, eventData);

	m_lobby->MemFree(mh);
}

TNetAddress CDrxLANMatchMaking::GetHostAddressFromSessionHandle(DrxSessionHandle gh)
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
		return TNetAddress(TLocalNetAddress(m_lobby->GetInternalSocketPort(eCLS_LAN)));
	}

	SIPv4Addr addr4;

	addr4.addr = ntohl(pSession->id.m_ip);
	addr4.port = pSession->id.m_port;

	return TNetAddress(addr4);
}

void CDrxLANMatchMaking::Kick(const DrxUserID* pUserID, EDisconnectionCause cause)
{
	if (pUserID != NULL)
	{
		const SDrxLANUserID* pLANUserID = static_cast<const SDrxLANUserID*>(pUserID->get());
		KickCmd(DrxLobbyInvalidConnectionID, pLANUserID->id, NULL, cause);
	}
}

uint64 CDrxLANMatchMaking::GetConnectionUserID(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const
{
	SSession::SRConnection* pPlatformConnection = reinterpret_cast<SSession::SRConnection*>(pConnection);

	if (pPlatformConnection->used)
	{
		return GetLANUserID(pPlatformConnection->uid);
	}

	return INVALID_USER_ID;
}

#endif // USE_LAN
