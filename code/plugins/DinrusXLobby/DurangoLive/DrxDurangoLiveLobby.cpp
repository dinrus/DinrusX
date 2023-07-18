// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxDurangoLiveLobby.h"
<DinrusX/CoreX/TypeInfo_impl.h>

#if USE_DURANGOLIVE

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Storage::Streams;
using namespace Microsoft::WRL;
using namespace ABI::Windows::Xbox::Networking;
using namespace ABI::Windows::Xbox::System;

	#define DURANGO_DEFAULT_PORT           49150

	#define TDN_GET_USER_PRIVILEGES_RESULT 0

EDrxLobbyError DrxDurangoLiveLobbyGetErrorFromDurangoLive(HRESULT error)
{
	NetLog("[Lobby] eCLE_InternalError: Durango Live Error %d %x", error, error);
	return eCLE_InternalError;
}

CDrxDurangoLiveLobbyService::CDrxDurangoLiveLobbyService(CDrxLobby* pLobby, EDrxLobbyService service)
	: CDrxLobbyService(pLobby, service)
	, m_titleID(0)
{
}

CDrxDurangoLiveLobbyService::~CDrxDurangoLiveLobbyService()
{
}

EDrxLobbyError CDrxDurangoLiveLobbyService::Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	EDrxLobbyError ret = CDrxLobbyService::Initialise(features, pCB);

	if ((ret == eCLE_Success) && (m_pTCPServiceFactory == NULL) && (features & eCLSO_TCPService))
	{
		m_pTCPServiceFactory = new CDrxTCPServiceFactory();

		if (m_pTCPServiceFactory)
		{
			ret = m_pTCPServiceFactory->Initialise("Scripts/Network/TCPServices.xml", CDrxTCPServiceFactory::k_defaultMaxSockets);
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

	if (features & eCLSO_Base)
	{
		SConfigurationParams neededInfo[2] = {
			{ CLCC_LIVE_TITLE_ID,          { NULL }
			},{ CLCC_LIVE_SERVICE_CONFIG_ID, { NULL }
			}
		};

		m_pLobby->GetConfigurationInformation(neededInfo, DRX_ARRAY_COUNT(neededInfo));

		m_titleID = neededInfo[0].m_32;
		m_serviceConfigId.Set(reinterpret_cast<LPCWSTR>(neededInfo[1].m_pData));
	}

	#if USE_DRX_MATCHMAKING
	if ((ret == eCLE_Success) && (m_pMatchmaking == NULL) && (features & eCLSO_Matchmaking))
	{
		m_pMatchmaking = new CDrxDurangoLiveMatchMaking(m_pLobby, this, m_service);

		if (m_pMatchmaking)
		{
			ret = m_pMatchmaking->Initialise();
		}
		else
		{
			ret = eCLE_OutOfMemory;
		}
	}
	#endif // USE_DRX_MATCHMAKING

	#if USE_DRX_ONLINE_STORAGE
	if ((ret == eCLE_Success) && (m_pOnlineStorage == NULL) && (features & eCLSO_OnlineStorage))
	{
		m_pOnlineStorage = new CDrxDurangoLiveOnlineStorage(m_pLobby, this);
		if (m_pOnlineStorage)
		{
			ret = m_pOnlineStorage->Initialise();
		}
		else
		{
			ret = eCLE_OutOfMemory;
		}
	}
	#endif // USE_DRX_ONLINE_STORAGE

	if ((ret == eCLE_Success) && (m_pLobbyUI == NULL) && (features & eCLSO_LobbyUI))
	{
		m_pLobbyUI = new CDrxDurangoLiveLobbyUI(m_pLobby, this);

		if (m_pLobbyUI)
		{
			ret = m_pLobbyUI->Initialise();
		}
		else
		{
			ret = eCLE_OutOfMemory;
		}
	}

	if (ret == eCLE_Success)
	{
		if (features & eCLSO_Base)
		{
			m_pLobby->SetServicePacketEnd(m_service, eDurangoLivePT_EndType);
		}
	}
	else
	{
		Terminate(features, NULL);
	}

	if (pCB)
	{
		pCB(ret, m_pLobby, m_service);
	}

	NetLog("[Lobby] Durango Live Initialise error %d", ret);

	return ret;
}

EDrxLobbyError CDrxDurangoLiveLobbyService::Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	EDrxLobbyError ret = CDrxLobbyService::Terminate(features, pCB);

	if (m_pTCPServiceFactory && (features & eCLSO_TCPService))
	{
		EDrxLobbyError error = m_pTCPServiceFactory->Terminate(false);
		m_pTCPServiceFactory = NULL;

		if (ret == eCLE_Success)
		{
			ret = error;
		}
	}

	#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking && (features & eCLSO_Matchmaking))
	{
		EDrxLobbyError error = m_pMatchmaking->Terminate();
		m_pMatchmaking = NULL;

		if (ret == eCLE_Success)
		{
			ret = error;
		}
	}
	#endif // USE_DRX_MATCHMAKING

	#if USE_DRX_ONLINE_STORAGE
	if (m_pOnlineStorage && (features & eCLSO_OnlineStorage))
	{
		EDrxLobbyError error = m_pOnlineStorage->Terminate();
		m_pOnlineStorage = NULL;

		if (ret == eCLE_Success)
		{
			ret = error;
		}
	}
	#endif // USE_DRX_ONLINE_STORAGE

	if (m_pLobbyUI && (features & eCLSO_LobbyUI))
	{
		EDrxLobbyError error = m_pLobbyUI->Terminate();
		m_pLobbyUI = NULL;

		if (ret == eCLE_Success)
		{
			ret = error;
		}
	}

	if (pCB)
	{
		pCB(ret, m_pLobby, m_service);
	}

	return ret;
}

void CDrxDurangoLiveLobbyService::Tick(CTimeValue tv)
{
	if (m_pLobby->MutexTryLock())
	{
	#if USE_DRX_MATCHMAKING
		if (m_pMatchmaking)
		{
			m_pMatchmaking->Tick(tv);
		}
	#endif // USE_DRX_MATCHMAKING

	#if USE_DRX_ONLINE_STORAGE
		if (m_pOnlineStorage)
		{
			m_pOnlineStorage->Tick(tv);
		}
	#endif // USE_DRX_ONLINE_STORAGE

		if (m_pLobbyUI)
		{
			m_pLobbyUI->Tick(tv);
		}

		m_pLobby->MutexUnlock();
	}

	if (m_pTCPServiceFactory)
	{
		m_pTCPServiceFactory->Tick(tv);
	}
}

void CDrxDurangoLiveLobbyService::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnPacket(addr, pPacket);
	}
	#endif // USE_DRX_MATCHMAKING
}

void CDrxDurangoLiveLobbyService::OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID)
{
	#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnError(addr, error, sendID);
	}
	#endif // USE_DRX_MATCHMAKING
}

void CDrxDurangoLiveLobbyService::OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID)
{
	#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnSendComplete(addr, sendID);
	}
	#endif // USE_DRX_MATCHMAKING
}

DrxUserID CDrxDurangoLiveLobbyService::GetUserID(uint32 user)
{
	return DrxUserInvalidID;
}

void CDrxDurangoLiveLobbyService::StartTaskRunning(DrxLobbyServiceTaskID lsTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_GetUserPrivileges:
			StartGetUserPrivileges(lsTaskID);
			break;
		}
	}
}

void CDrxDurangoLiveLobbyService::StopTaskRunning(DrxLobbyServiceTaskID lsTaskID)
{
	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxDurangoLiveLobbyService::EndTask, this, lsTaskID);
	}
}

void CDrxDurangoLiveLobbyService::EndTask(DrxLobbyServiceTaskID lsTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		if (pTask->pCB)
		{
			switch (pTask->startedTask)
			{
			case eT_GetUserPrivileges:
				((DrxLobbyPrivilegeCallback)pTask->pCB)(pTask->lTaskID, pTask->error, pTask->dataNum[TDN_GET_USER_PRIVILEGES_RESULT], pTask->pCBArg);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] Lobby Service EndTask %d Result %d", pTask->startedTask, pTask->error);
		}

		FreeTask(lsTaskID);
	}
}

EDrxLobbyError CDrxDurangoLiveLobbyService::GetUserPrivileges(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyPrivilegeCallback pCB, void* pCBArg)
{
	EDrxLobbyError error;
	DrxLobbyServiceTaskID tid;

	LOBBY_AUTO_LOCK;

	error = StartTask(eT_GetUserPrivileges, user, false, &tid, pTaskID, (void*)pCB, pCBArg);

	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxDurangoLiveLobbyService::StartTaskRunning, this, tid);
	}

	return error;
}

void CDrxDurangoLiveLobbyService::StartGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID)
{
	STask* pTask = GetTask(lsTaskID);
	uint32 result = 0;

	if (m_pLobby->GetInternalSocket(m_service))
	{
	}
	else
	{
		UpdateTaskError(lsTaskID, eCLE_InternalError);
	}

	pTask->dataNum[TDN_GET_USER_PRIVILEGES_RESULT] = result;

	StopTaskRunning(lsTaskID);
}

EDrxLobbyError CDrxDurangoLiveLobbyService::GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime)
{
	memset(pSystemTime, 0, sizeof(SDrxSystemTime));

	EDrxLobbyError error = eCLE_Success;
	SYSTEMTIME systemTime;
	::GetSystemTime(&systemTime);

	pSystemTime->m_Year = systemTime.wYear;
	pSystemTime->m_Month = static_cast<uint8>(systemTime.wMonth);
	pSystemTime->m_Day = static_cast<uint8>(systemTime.wDay);
	pSystemTime->m_Hour = static_cast<uint8>(systemTime.wHour);
	pSystemTime->m_Minute = static_cast<uint8>(systemTime.wMinute);
	pSystemTime->m_Second = static_cast<uint8>(systemTime.wSecond);

	return error;
}

void CDrxDurangoLiveLobbyService::GetSocketPorts(uint16& connectPort, uint16& listenPort)
{
	connectPort = DURANGO_DEFAULT_PORT;
	listenPort = DURANGO_DEFAULT_PORT;
}

#endif // USE_DURANGOLIVE
