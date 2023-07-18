// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxLANLobby.h"
#include "DrxDedicatedServerArbitrator.h"
#include "DrxDedicatedServer.h"

#if USE_LAN

CDrxLANLobbyService::CDrxLANLobbyService(CDrxLobby* pLobby, EDrxLobbyService service) : CDrxLobbyService(pLobby, service)
{
	m_lobbyServiceFlags &= ~eCLSF_AllowMultipleSessions;

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	m_pDedicatedServerArbitrator = NULL;
	#endif
	#if USE_DRX_DEDICATED_SERVER
	m_pDedicatedServer = NULL;
	#endif
}

EDrxLobbyError CDrxLANLobbyService::Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
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

	if ((ret == eCLE_Success) && (m_pMatchmaking == NULL) && (features & eCLSO_Matchmaking))
	{
		m_pMatchmaking = new CDrxLANMatchMaking(m_pLobby, this, m_service);

		if (m_pMatchmaking)
		{
			ret = m_pMatchmaking->Initialise();
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	if ((ret == eCLE_Success) && (m_pDedicatedServerArbitrator == NULL) && (features & eCLSO_Base))
	{
		if (gEnv->IsDedicated() && gEnv->bDedicatedArbitrator)
		{
			m_pDedicatedServerArbitrator = new CDrxDedicatedServerArbitrator(m_pLobby, this);

			if (m_pDedicatedServerArbitrator)
			{
				ret = m_pDedicatedServerArbitrator->Initialise();
			}
			else
			{
				return eCLE_OutOfMemory;
			}
		}
	}
	#endif

	#if USE_DRX_DEDICATED_SERVER
	if ((ret == eCLE_Success) && (m_pDedicatedServer == NULL) && (features & eCLSO_Base))
	{
		if (gEnv->IsDedicated() && !gEnv->bDedicatedArbitrator)
		{
			m_pDedicatedServer = new CDrxDedicatedServer(m_pLobby, this, m_service);

			if (m_pDedicatedServer)
			{
				ret = m_pDedicatedServer->Initialise();
			}
			else
			{
				return eCLE_OutOfMemory;
			}
		}
	}
	#endif

	if ((ret == eCLE_Success) && (features & eCLSO_Base))
	{
		m_pLobby->SetServicePacketEnd(m_service, eLANPT_EndType);
	}

	if (pCB)
	{
		pCB(ret, m_pLobby, m_service);
	}

	return ret;
}

EDrxLobbyError CDrxLANLobbyService::Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	EDrxLobbyError ret = eCLE_NotInitialised;

	if (m_pTCPServiceFactory || m_pMatchmaking
	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	    || m_pDedicatedServerArbitrator
	#endif
	#if USE_DRX_DEDICATED_SERVER
	    || m_pDedicatedServer
	#endif
	    )
	{
		ret = CDrxLobbyService::Terminate(features, pCB);

		if (m_pTCPServiceFactory && (features & eCLSO_TCPService))
		{
			EDrxLobbyError error = m_pTCPServiceFactory->Terminate(false);
			m_pTCPServiceFactory = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}

		if (m_pMatchmaking && (features & eCLSO_Matchmaking))
		{
			EDrxLobbyError error = m_pMatchmaking->Terminate();
			m_pMatchmaking = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
		if (m_pDedicatedServerArbitrator && (features & eCLSO_Base))
		{
			EDrxLobbyError error = m_pDedicatedServerArbitrator->Terminate();
			m_pDedicatedServerArbitrator = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
	#endif

	#if USE_DRX_DEDICATED_SERVER
		if (m_pDedicatedServer && (features & eCLSO_Base))
		{
			EDrxLobbyError error = m_pDedicatedServer->Terminate();
			m_pDedicatedServer = NULL;

			if (ret == eCLE_Success)
			{
				ret = error;
			}
		}
	#endif

		if (pCB)
		{
			pCB(ret, m_pLobby, m_service);
		}
	}

	return ret;
}

void CDrxLANLobbyService::Tick(CTimeValue tv)
{
	if (m_pLobby->MutexTryLock())
	{
		if (m_pMatchmaking)
		{
			m_pMatchmaking->Tick(tv);
		}

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
		if (m_pDedicatedServerArbitrator)
		{
			m_pDedicatedServerArbitrator->Tick(tv);
		}
	#endif

	#if USE_DRX_DEDICATED_SERVER
		if (m_pDedicatedServer)
		{
			m_pDedicatedServer->Tick(tv);
		}
	#endif

		m_pLobby->MutexUnlock();
	}

	if (m_pTCPServiceFactory)
	{
		m_pTCPServiceFactory->Tick(tv);
	}
}

EDrxLobbyError CDrxLANLobbyService::GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime)
{
	memset(pSystemTime, 0, sizeof(SDrxSystemTime));

	time_t ltime;
	time(&ltime);
	tm* pToday = localtime(&ltime);

	pSystemTime->m_Year = pToday->tm_year + 1900;
	pSystemTime->m_Month = static_cast<uint8>(pToday->tm_mon + 1);
	pSystemTime->m_Day = static_cast<uint8>(pToday->tm_mday);
	pSystemTime->m_Hour = static_cast<uint8>(pToday->tm_hour);
	pSystemTime->m_Minute = static_cast<uint8>(pToday->tm_min);
	pSystemTime->m_Second = static_cast<uint8>(pToday->tm_sec);

	NetLog("**WARNING** DrxLANLobbyService::GetSystemTime() is not a trusted time source");
	return eCLE_Success;
}

void CDrxLANLobbyService::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnPacket(addr, pPacket);
	}

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	if (m_pDedicatedServerArbitrator)
	{
		m_pDedicatedServerArbitrator->OnPacket(addr, pPacket);
	}
	#endif

	#if USE_DRX_DEDICATED_SERVER
	if (m_pDedicatedServer)
	{
		m_pDedicatedServer->OnPacket(addr, pPacket);
	}
	#endif
}

void CDrxLANLobbyService::OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID)
{
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnError(addr, error, sendID);
	}

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	if (m_pDedicatedServerArbitrator)
	{
		m_pDedicatedServerArbitrator->OnError(addr, error, sendID);
	}
	#endif

	#if USE_DRX_DEDICATED_SERVER
	if (m_pDedicatedServer)
	{
		m_pDedicatedServer->OnError(addr, error, sendID);
	}
	#endif
}

void CDrxLANLobbyService::OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID)
{
	if (m_pMatchmaking)
	{
		m_pMatchmaking->OnSendComplete(addr, sendID);
	}
}

void CDrxLANLobbyService::GetSocketPorts(uint16& connectPort, uint16& listenPort)
{
	CDrxLobbyService::GetSocketPorts(connectPort, listenPort);

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	if (m_pDedicatedServerArbitrator)
	{
		listenPort = CLobbyCVars::Get().dedicatedServerArbitratorPort;
	}
	#endif
}

#endif // USE_LAN
