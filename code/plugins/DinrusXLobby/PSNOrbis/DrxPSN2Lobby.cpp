// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN
		<DinrusX/net.h>
		<DinrusX/libnetctl.h>

void CDrxPSNLobbyService::SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg)
{
	CDrxPSNLobbyService* _this = static_cast<CDrxPSNLobbyService*>(pArg);

	if (ecb == eCE_ErrorEvent)
	{
		for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			STask* pTask = _this->GetTask(i);

			if (pTask->used && pTask->running)
			{
				_this->UpdateTaskError(i, MapSupportErrorToLobbyError(data.m_errorEvent.m_error));
				_this->StopTaskRunning(i);
			}
		}
	}
}

CDrxPSNLobbyService::CDrxPSNLobbyService(CDrxLobby* pLobby, EDrxLobbyService service) : CDrxLobbyService(pLobby, service)
{
		#if USE_PSN_VOICE
	m_pVoice = NULL;
		#endif//USE_PSN_VOICE
	m_pLobbySupport = NULL;
		#if USE_DRX_MATCHMAKING
	m_pMatchmaking = NULL;
		#endif // USE_DRX_MATCHMAKING
	m_NPInited = false;
		#if USE_DRX_TCPSERVICE
	m_pTCPServiceFactory = NULL;
		#endif // USE_DRX_TCPSERVICE
		#if USE_DRX_STATS
	m_pStats = NULL;
		#endif // USE_DRX_STATS
	m_pLobbyUI = NULL;
		#if USE_DRX_FRIENDS
	m_pFriends = NULL;
		#endif // USE_DRX_FRIENDS
	m_pReward = NULL;
}

EDrxLobbyError CDrxPSNLobbyService::Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	EDrxLobbyError error = CDrxLobbyService::Initialise(features, pCB);

	if (!m_NPInited)
	{
		int ret;

		ret = sceNetCtlInit();
		if ((ret < PSN_OK) && (ret != SCE_NET_CTL_ERROR_NOT_INITIALIZED))
		{
			NetLog("sceNetCtlInit() failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}

		SConfigurationParams titleInfo[3] = {
			{ CLCC_PSN_TITLE_ID,     { NULL }
			},{ CLCC_PSN_TITLE_SECRET, { NULL }
			},{ CLCC_PSN_AGE_LIMIT,    { NULL }
			}
		};
		GetConfigurationInformation(titleInfo, 3);

		#if !USE_NPTITLE_DAT
		ret = sceNpSetNpTitleId((const SceNpTitleId*)titleInfo[0].m_pData, (const SceNpTitleSecret*)titleInfo[1].m_pData);
		if (ret < PSN_OK)
		{
			NetLog("sceNpSetNpTitleId() failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
		#endif // !USE_NPTITLE_DAT

		SDrxLobbyAgeRestrictionSetup* pAgeSetup = (SDrxLobbyAgeRestrictionSetup*)titleInfo[2].m_pData;
		SceNpAgeRestriction* pAges = NULL;

		SceNpContentRestriction contentRestriction;
		memset(&contentRestriction, 0, sizeof(contentRestriction));
		contentRestriction.size = sizeof(contentRestriction);
		contentRestriction.defaultAgeRestriction = SCE_NP_NO_AGE_RESTRICTION;

		if (pAgeSetup && pAgeSetup->m_numCountries && pAgeSetup->m_pCountries)
		{
			pAges = new SceNpAgeRestriction[pAgeSetup->m_numCountries];
			if (pAges)
			{
				memset(pAges, 0, pAgeSetup->m_numCountries * sizeof(SceNpAgeRestriction));

				for (int i = 0; i < pAgeSetup->m_numCountries; ++i)
				{
					strncpy(pAges[i].countryCode.data, pAgeSetup->m_pCountries[i].id, 2);
					pAges[i].age = pAgeSetup->m_pCountries[i].age;
				}

				contentRestriction.ageRestrictionCount = pAgeSetup->m_numCountries;
				contentRestriction.ageRestriction = pAges;
			}
		}

		ret = sceNpSetContentRestriction(&contentRestriction);
		if (ret < PSN_OK)
		{
			NetLog("sceNpSetContentRestriction() failed. ret = 0x%x\n", ret);
			SAFE_DELETE_ARRAY(pAges);
			return eCLE_InternalError;
		}

		SAFE_DELETE_ARRAY(pAges);

		m_NPInited = true;
	}

		#if USE_DRX_TCPSERVICE
	if ((m_pTCPServiceFactory == NULL) && (features & eCLSO_TCPService))
	{
		m_pTCPServiceFactory = new CDrxTCPServiceFactory();

		if (m_pTCPServiceFactory)
		{
			error = m_pTCPServiceFactory->Initialise("Scripts/Network/TCPServices.xml", CDrxTCPServiceFactory::k_defaultMaxSockets);
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}
		#endif // USE_DRX_TCPSERVICE

	if ((error == eCLE_Success) && (m_pLobbySupport == NULL) && (features & eCLSO_Base))
	{
		m_pLobbySupport = new CDrxPSNSupport(m_pLobby, this, features);
		if (m_pLobbySupport)
		{
			error = m_pLobbySupport->Initialise();
			if (!m_pLobbySupport->RegisterEventHandler(eCE_ErrorEvent, CDrxPSNLobbyService::SupportCallback, this))
			{
				return eCLE_InternalError;
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

		#if USE_PSN_VOICE
	if ((error == eCLE_Success) && (m_pVoice == NULL) && (features & eCLSO_Voice))
	{
		m_pVoice = new CDrxPSNVoice(m_pLobby, this);

		if (m_pVoice)
		{
			error = m_pVoice->Initialise();
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}
		#endif

		#if USE_DRX_MATCHMAKING
	if ((error == eCLE_Success) && (m_pMatchmaking == NULL) && (features & eCLSO_Matchmaking))
	{
		m_pMatchmaking = new CDrxPSNMatchMaking(m_pLobby, this, m_pLobbySupport, m_service);

		if (m_pMatchmaking)
		{
			error = m_pMatchmaking->Initialise();
			if (!m_pLobbySupport->RegisterEventHandler(eCE_ErrorEvent | eCE_RequestEvent | eCE_RoomEvent | eCE_SignalEvent | eCE_WebApiEvent, CDrxPSNMatchMaking::SupportCallback, m_pMatchmaking))
			{
				return eCLE_InternalError;
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}
		#endif // USE_DRX_MATCHMAKING

		#if USE_DRX_STATS
	if ((error == eCLE_Success) && (m_pStats == NULL) && (features & eCLSO_Stats))
	{
		m_pStats = new CDrxPSNStats(m_pLobby, this, m_pLobbySupport);

		if (m_pStats)
		{
			error = m_pStats->Initialise();
			if (!m_pLobbySupport->RegisterEventHandler(eCE_ErrorEvent, CDrxPSNStats::SupportCallback, m_pStats))
			{
				return eCLE_InternalError;
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}
		#endif // USE_DRX_STATS

	if ((error == eCLE_Success) && (m_pLobbyUI == NULL) && (features & eCLSO_LobbyUI))
	{
		m_pLobbyUI = new CDrxPSNLobbyUI(m_pLobby, this, m_pLobbySupport);

		if (m_pLobbyUI)
		{
			error = m_pLobbyUI->Initialise();
			if (!m_pLobbySupport->RegisterEventHandler(eCE_ErrorEvent | eCE_WebApiEvent, CDrxPSNLobbyUI::SupportCallback, m_pLobbyUI))
			{
				return eCLE_InternalError;
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

		#if USE_DRX_FRIENDS
	if ((error == eCLE_Success) && (m_pFriends == NULL) && (features & eCLSO_Friends))
	{
		m_pFriends = new CDrxPSNFriends(m_pLobby, this, m_pLobbySupport);

		if (m_pFriends)
		{
			error = m_pFriends->Initialise();
			if (!m_pLobbySupport->RegisterEventHandler(eCE_ErrorEvent | eCE_WebApiEvent, CDrxPSNFriends::SupportCallback, m_pFriends))
			{
				return eCLE_InternalError;
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}
		#endif // USE_DRX_FRIENDS

	if ((error == eCLE_Success) && (m_pReward == NULL) && (features & eCLSO_Reward))
	{
		m_pReward = new CDrxPSNReward(m_pLobby);
		if (m_pReward)
		{
			error = m_pReward->Initialise();
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

	if (error == eCLE_Success)
	{
		if (features & eCLSO_Base)
		{
			m_pLobby->SetServicePacketEnd(m_service, ePSNPT_EndType);
		}
	}
	else
	{
		Terminate(features, NULL);
	}

	if (pCB)
	{
		pCB(error, m_pLobby, m_service);
	}

	return error;
}

EDrxLobbyError CDrxPSNLobbyService::Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	if (
		#if USE_DRX_MATCHMAKING
	  m_pMatchmaking ||
		#endif // USE_DRX_MATCHMAKING
		#if USE_DRX_TCPSERVICE
	  m_pTCPServiceFactory ||
		#endif // USE_DRX_TCPSERVICE
	  m_pLobbySupport ||
		#if USE_DRX_STATS
	  m_pStats ||
		#endif // USE_DRX_STATS
	  m_pLobbyUI ||
		#if USE_DRX_FRIENDS
	  m_pFriends ||
		#endif // USE_DRX_FRIENDS
	  m_pReward ||
		#if USE_PSN_VOICE
	  m_pVoice ||
		#endif
	  0
	  )
	{
		EDrxLobbyError ret = CDrxLobbyService::Terminate(features, pCB);

		#if USE_DRX_FRIENDS
		if (m_pFriends && (features & eCLSO_Friends))
		{
			EDrxLobbyError error = m_pFriends->Terminate();
			m_pFriends = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
		#endif // USE_DRX_FRIENDS

		if (m_pLobbyUI && (features & eCLSO_LobbyUI))
		{
			EDrxLobbyError error = m_pLobbyUI->Terminate();
			m_pLobbyUI = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}

		#if USE_DRX_STATS
		if (m_pStats && (features & eCLSO_Stats))
		{
			EDrxLobbyError error = m_pStats->Terminate();
			m_pStats = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
		#endif // USE_DRX_STATS

		#if USE_DRX_TCPSERVICE
		if (m_pTCPServiceFactory && (features & eCLSO_TCPService))
		{
			EDrxLobbyError error = m_pTCPServiceFactory->Terminate(false);
			m_pTCPServiceFactory = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
		#endif // USE_DRX_TCPSERVICE

		#if USE_PSN_VOICE
		if (m_pVoice && (features & eCLSO_Voice))
		{
			EDrxLobbyError error = m_pVoice->Terminate();
			m_pVoice = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
		#endif

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

		if (m_pReward && (features & eCLSO_Reward))
		{
			EDrxLobbyError error = m_pReward->Terminate();
			m_pReward = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}

		if (m_pLobbySupport && (features & eCLSO_Base))
		{
			EDrxLobbyError error = m_pLobbySupport->Terminate();
			m_pLobbySupport = NULL;

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
	else
	{
		return eCLE_NotInitialised;
	}
}

void CDrxPSNLobbyService::Tick(CTimeValue tv)
{
	if (m_pLobby->MutexTryLock())
	{
		if (m_pLobbySupport)
		{
			m_pLobbySupport->Tick();
		}

		for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			STask* pTask = GetTask(i);

			if (pTask->used && pTask->running)
			{
				if (pTask->error == eCLE_Success)
				{
					switch (pTask->startedTask)
					{
					case eT_GetUserPrivileges:
						TickGetUserPrivileges(i);
						break;
					}
				}
			}
		}

		#if USE_DRX_MATCHMAKING
		if (m_pMatchmaking)
		{
			m_pMatchmaking->Tick(tv);
		}
		#endif // USE_DRX_MATCHMAKING

		#if USE_PSN_VOICE
		if (m_pVoice)
		{
			m_pVoice->Tick(tv);
		}
		#endif

		#if USE_DRX_STATS
		if (m_pStats)
		{
			m_pStats->Tick(tv);
		}
		#endif // USE_DRX_STATS

		if (m_pReward)
		{
			m_pReward->Tick(tv);
		}

		if (m_pLobbyUI)
		{
			m_pLobbyUI->Tick(tv);
		}

		m_pLobby->MutexUnlock();
	}

		#if USE_DRX_TCPSERVICE
	if (m_pTCPServiceFactory)
	{
		m_pTCPServiceFactory->Tick(tv);
	}
		#endif // USE_DRX_TCPSERVICE
}

void CDrxPSNLobbyService::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
		#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnPacket(addr, pPacket);
	}
		#endif // USE_DRX_MATCHMAKING

		#if USE_PSN_VOICE
	if (m_pVoice)
	{
		m_pVoice->OnPacket(addr, pPacket);
	}
		#endif // USE_PSN_VOICE
}

void CDrxPSNLobbyService::OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID)
{
		#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnError(addr, error, sendID);
	}
		#endif // USE_DRX_MATCHMAKING
}

void CDrxPSNLobbyService::OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID)
{
		#if USE_DRX_MATCHMAKING
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnSendComplete(addr, sendID);
	}
		#endif // USE_DRX_MATCHMAKING
}

void CDrxPSNLobbyService::GetConfigurationInformation(SConfigurationParams* infos, uint32 cnt)
{
	m_pLobby->GetConfigurationInformation(infos, cnt);
}

DrxUserID CDrxPSNLobbyService::GetUserID(uint32 user)
{
	if (m_pLobbySupport && (m_pLobbySupport->GetOnlineState() >= ePSNOS_Online))
	{
		//-- the npId should be valid
		SDrxPSNUserID* pID = new SDrxPSNUserID;
		if (pID)
		{
			pID->npId = *(m_pLobbySupport->GetLocalNpId());
			return pID;
		}
	}

	return DrxUserInvalidID;
}

void CDrxPSNLobbyService::StartTaskRunning(DrxLobbyServiceTaskID lsTaskID)
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

void CDrxPSNLobbyService::StopTaskRunning(DrxLobbyServiceTaskID lsTaskID)
{
	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxPSNLobbyService::EndTask, this, lsTaskID);
	}
}

void CDrxPSNLobbyService::EndTask(DrxLobbyServiceTaskID lsTaskID)
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
				{
					uint32 result = 0;
					if (!m_pLobbySupport || (m_pLobbySupport->GetOnlineState() < ePSNOS_Online) || (pTask->error == eCLE_InsufficientPrivileges) || (pTask->error == eCLE_AgeRestricted))
					{
						result |= CLPF_BlockMultiplayerSessons;
					}

					((DrxLobbyPrivilegeCallback)pTask->pCB)(pTask->lTaskID, pTask->error, result, pTask->pCBArg);
				}
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

EDrxLobbyError CDrxPSNLobbyService::GetUserPrivileges(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyPrivilegeCallback pCB, void* pCBArg)
{
	EDrxLobbyError error;
	DrxLobbyServiceTaskID tid;

	LOBBY_AUTO_LOCK;

	error = StartTask(eT_GetUserPrivileges, user, false, &tid, pTaskID, (void*)pCB, pCBArg);

	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxPSNLobbyService::StartTaskRunning, this, tid);
	}

	return error;
}

EDrxLobbyError CDrxPSNLobbyService::GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime)
{
	SceRtcTick tick;
	EDrxLobbyError error = eCLE_Success;

	memset(pSystemTime, 0, sizeof(SDrxSystemTime));

	// There's no Network Clock as far as I can see. We'll have to use the system clock for now, which isn't as accurate.
	int rc = sceRtcGetCurrentTick(&tick);
	switch (rc)
	{
	case 0:
		// success
		break;
	case SCE_NP_ERROR_NOT_INITIALIZED:
		error = eCLE_NotInitialised;
		break;
	case SCE_NP_ERROR_INVALID_ARGUMENT:
		error = eCLE_InvalidParam;
		break;
	default:
		error = eCLE_InternalError;
		break;
	}

	if (error == eCLE_Success)
	{
		SceRtcDateTime time;
		if (sceRtcSetTick(&time, &tick) == SCE_OK)
		{
			pSystemTime->m_Year = time.year;
			pSystemTime->m_Month = static_cast<uint8>(time.month);
			pSystemTime->m_Day = static_cast<uint8>(time.day);
			pSystemTime->m_Hour = static_cast<uint8>(time.hour);
			pSystemTime->m_Minute = static_cast<uint8>(time.minute);
			pSystemTime->m_Second = static_cast<uint8>(time.second);
		}
		else
		{
			error = eCLE_InternalError;
		}
	}

	return error;
}

void CDrxPSNLobbyService::StartGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID)
{
	if (m_pLobbySupport && m_pLobby->GetInternalSocket(m_service))
	{
		// change this to ePSNOS_Matchmaking_Available if you want GetUserPrivileges to include PS+ test, otherwise it will test user age/PSN account privileges only
		m_pLobbySupport->ResumeTransitioning(ePSNOS_Online);
	}
	else
	{
		UpdateTaskError(lsTaskID, eCLE_InternalError);
		StopTaskRunning(lsTaskID);
	}
}

void CDrxPSNLobbyService::TickGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID)
{
	// change this to ePSNOS_Matchmaking_Available if you want GetUserPrivileges to include PS+ test, otherwise it will test user age/PSN account privileges only
	m_pLobbySupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pLobbySupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		StopTaskRunning(lsTaskID);
	}
}

void CDrxPSNLobbyService::GetSocketPorts(uint16& connectPort, uint16& listenPort)
{
	connectPort = SCE_NP_PORT;
	listenPort = SCE_NP_PORT;
}

		#if defined(WARFACE_CONSOLE_VERSION)
			#if defined(GUID_STYLE_DB)

DrxUserID CDrxPSNLobbyService::GetUserIDFromGUID(PlatformGUID64 xuid)
{
	SDrxPSNUserID* pID = new SDrxPSNUserID;

	if (pID)
	{
		pID->guid = xuid;
		return pID;
	}

	return DrxUserInvalidID;
}

PlatformGUID64 CDrxPSNLobbyService::GetGUIDFromUserID(DrxUserID id)
{
	return ((SDrxPSNUserID*)id.get())->guid;
}

			#endif
		#endif

void CDrxPSNLobbyService::MakeAddrPCCompatible(TNetAddress& addr)
{
	//SIPv4Addr* pIPv4Addr = addr.GetPtr<SIPv4Addr>();

	//if (pIPv4Addr)
	//{
	//	pIPv4Addr->lobbyService = eCLS_LAN;
	//}
}

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS
