// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#include "DrxLobby.h"
#include "DrxSharedLobbyPacket.h"
#include "LAN/DrxLANLobby.h"
#if USE_LIVE
	#include "Live/DrxLiveLobby.h"
#endif
#if USE_PSN
	#if DRX_PLATFORM_ORBIS
		#include "PSNOrbis/DrxPSN2Lobby.h"
	#endif
#endif
#if USE_STEAM
	#include "Steam/DrxSteamLobby.h"
#endif
#if USE_DURANGOLIVE
	#include "DurangoLive/DrxDurangoLiveLobby.h"
#endif

<DinrusX/CoreX/Game/IGameFramework.h>
#include "INetworkPrivate.h"
#include "Protocol/FrameTypes.h"
<DinrusX/CoreX/Memory/BucketAllocatorImpl.h>

<DinrusX/CoreX/Math/Random.h>
<DinrusX/CoreX/Platform/platform_impl.inl>

const int LOBBY_KEEP_ALIVE_INTERVAL = (DrxLobbySendInterval + 100);
const int LOBBY_FORCE_DISCONNECT_TIMER = 99999999;

static const uint8 CONNECTION_COUNTER_MAX = 255;

#if ENCRYPT_LOBBY_PACKETS
const char* g_lobbyEncryptionKey = "gSPyI9\"£$h83H8Uasd73nn()u12gh[[&";
#endif

#define INVALID_CONNECTION_COOKIE 0

#if !defined(_LIB)
CTimeValue g_time;
SObjectCounters* g_pObjcnt = NULL;
#endif

#ifdef USE_GLOBAL_BUCKET_ALLOCATOR
CDrxLobby::CMemAllocator::LobbyBuckets CDrxLobby::CMemAllocator::m_bucketAllocator(NULL, false);
#else
CDrxLobby::CMemAllocator::LobbyBuckets CDrxLobby::CMemAllocator::m_bucketAllocator;
#endif
IDrxLobby* CDrxLobby::m_pLobby = NULL;

#if defined(_RELEASE) && !defined(DEDICATED_SERVER)
	#define NO_NETWORK_SECURITY_LOGS
#endif

#if defined(NO_NETWORK_SECURITY_LOGS)
	#define SECURE_NET_LOG(...)
#else
	#define SECURE_NET_LOG NetLog
#endif

DRXREGISTER_SINGLETON_CLASS(CDrxLobby)

CDrxLobby::CDrxLobby()
{
	gEnv->pLobby = this;
}

CDrxLobby::~CDrxLobby()
{
	int a;

#if NETWORK_HOST_MIGRATION
	RemoveHostMigrationEventListener(this);
#endif

	for (a = 0; a < eCLS_NumServices; a++)
	{
		InternalSocketDie(EDrxLobbyService(a));
	}

#if ENCRYPT_LOBBY_PACKETS
	if (gEnv->pNetwork)
	{
		gEnv->pNetwork->EndCipher(m_cipher);
	}
#endif

	m_pLobby = NULL;
}

bool CDrxLobby::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	if (m_pLobby)
	{
		DrxFatalError("Trying to create a lobby when we already have one, this does not end well");
	}

	m_pLobby = this;
	g_time = gEnv->pTimer->GetAsyncTime();

	for (uint32 i = 0; i < eCLS_NumServices; i++)
	{
		m_services[i] = NULL;
		m_task[i].cb = NULL;
		m_task[i].cbArg = NULL;

		m_socketServices[i].m_socket = NULL;
		m_socketServices[i].m_socketListenPort = 0;
		m_socketServices[i].m_socketConnectPort = 0;

		memset(&m_serviceParams[i], 0, sizeof(SDrxLobbyParameters));

		m_servicePacketEnd[i] = 0xffff;
	}

	for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
	{
		m_serviceTask[i].used = false;
	}

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		m_connection[i].used = false;
	}

	m_userPacketEnd = 0xffff;

	m_lvlName[0] = 0;
	m_rulName[0] = 0;

	m_callbacks.reserve(10);      // pre-reserve space for callback event data
	m_service = eCLS_LAN;
	m_natType = eNT_Unknown;

#if USE_GFWL
	m_gfwlExtras = new CDrxLobbyGFWLExtras;
#else
	m_gfwlExtras = NULL;
#endif

#if NETWORK_HOST_MIGRATION
	//m_hostMigrationListeners.reserve(10);
	AddHostMigrationEventListener(this, "DrxLobby", ELPT_Engine);
#endif

#if ENCRYPT_LOBBY_PACKETS
	if (gEnv->pNetwork)
	{
		m_cipher = gEnv->pNetwork->BeginCipher((const uint8*)g_lobbyEncryptionKey, 32);
	}
#endif

	return true;
}

EDrxLobbyService CDrxLobby::SetLobbyService(EDrxLobbyService service)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyService ret = m_service;

	assert(service < eCLS_NumServices);
#if DRX_PLATFORM_DURANGO || DRX_PLATFORM_ORBIS
	m_service = eCLS_Online;
#else
	m_service = service;
#endif

	return ret;
}

#if NETWORK_HOST_MIGRATION
IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnInitiate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	NetLog("[Host Migration]: CDrxLobby::OnInitiate() started");

	SHostMigrationInfo_Private& privateInfo = static_cast<SHostMigrationInfo_Private&>(hostMigrationInfo);
	privateInfo.m_lobbyHMStatus = eLHMS_InitiateMigrate;

	NetLog("[Host Migration]: CDrxLobby::OnInitiate() finished");
	return IHostMigrationEventListener::Listener_Done;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnDisconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	return IHostMigrationEventListener::Listener_Done;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnDemoteToClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	return IHostMigrationEventListener::Listener_Done;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnPromoteToServer(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	bool done = false;
	CDrxMatchMaking* pMatchMaking = (CDrxMatchMaking*)GetMatchMaking();

	if (pMatchMaking)
	{
		SHostMigrationInfo_Private& privateInfo = static_cast<SHostMigrationInfo_Private&>(hostMigrationInfo);
		switch (privateInfo.m_lobbyHMStatus)
		{
		case eLHMS_InitiateMigrate:
			NetLog("[Host Migration]: CDrxLobby::OnPromoteToServer() started");
			pMatchMaking->HostMigrationServer(&hostMigrationInfo);
			++privateInfo.m_lobbyHMStatus;
			break;
		case eLHMS_Migrating:
		default:
			done = pMatchMaking->IsSessionMigrated(&hostMigrationInfo);
			if (done)
			{
				privateInfo.m_lobbyHMStatus = eLHMS_InitiateMigrate;
			}
			break;
		}

		if (hostMigrationInfo.m_logProgress || (done == true))
		{
			NetLog("[Host Migration]: CDrxLobby::OnPromoteToServer() %s for " PRFORMAT_SH, ((done == true) ? "finished" : "waiting"), PRARG_SH(pMatchMaking->GetSessionHandleFromGameSessionHandle(hostMigrationInfo.m_session)));
		}
	}

	if (done)
	{
		return IHostMigrationEventListener::Listener_Done;
	}

	return IHostMigrationEventListener::Listener_Wait;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnReconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	bool done = false;
	CDrxMatchMaking* pMatchMaking = (CDrxMatchMaking*)GetMatchMaking();
	if (pMatchMaking)
	{
		SHostMigrationInfo_Private& privateInfo = static_cast<SHostMigrationInfo_Private&>(hostMigrationInfo);
		switch (privateInfo.m_lobbyHMStatus)
		{
		case eLHMS_InitiateMigrate:
			NetLog("[Host Migration]: CDrxLobby::OnReconnectClient() started");
			++privateInfo.m_lobbyHMStatus;
			break;
		case eLHMS_Migrating:
			done = pMatchMaking->IsSessionMigrated(&hostMigrationInfo);
		default:
			break;
		}

		if (hostMigrationInfo.m_logProgress || (done == true))
		{
			NetLog("[Host Migration]: CDrxLobby::OnReconnectClient() %s for " PRFORMAT_SH, ((done == true) ? "finished" : "waiting"), PRARG_SH(pMatchMaking->GetSessionHandleFromGameSessionHandle(hostMigrationInfo.m_session)));
		}

		if (done)
		{
			pMatchMaking->OnReconnectClient(hostMigrationInfo);
		}
	}

	if (done)
	{
		return IHostMigrationEventListener::Listener_Done;
	}

	return IHostMigrationEventListener::Listener_Wait;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnFinalise(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	return IHostMigrationEventListener::Listener_Done;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnTerminate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	return IHostMigrationEventListener::Listener_Done;
}

IHostMigrationEventListener::EHostMigrationReturn CDrxLobby::OnReset(SHostMigrationInfo& hostMigrationInfo, HMStateType& state)
{
	return IHostMigrationEventListener::Listener_Done;
}
#endif

void CDrxLobby::InviteAccepted(EDrxLobbyService service, uint32 user, DrxSessionID sessionID, EDrxLobbyError error)
{
	UDrxLobbyEventData eventData;
	SDrxLobbyInviteAcceptedData inviteData;

	eventData.pInviteAcceptedData = &inviteData;
	inviteData.m_service = service;
	inviteData.m_user = user;
	inviteData.m_id = sessionID;
	inviteData.m_error = error;
	DispatchEvent(eCLSE_InviteAccepted, eventData);
}

void CDrxLobby::InviteAccepted(EDrxLobbyService service, uint32 user, DrxUserID userID, EDrxLobbyError error, EDrxLobbyInviteType inviteType)
{
	UDrxLobbyEventData eventData;
	SDrxLobbyUserInviteAcceptedData inviteData;

	eventData.pUserInviteAcceptedData = &inviteData;
	inviteData.m_service = service;
	inviteData.m_user = user;
	inviteData.m_inviterId = userID;
	inviteData.m_error = error;
	inviteData.m_type = inviteType;
	DispatchEvent(eCLSE_UserInviteAccepted, eventData);
}

uint32 CDrxLobby::GetDisconnectTimeOut()
{
	const CNetCVars* pNetCVars = GetNetCVars();

	if (pNetCVars)
	{
		if (gEnv->pSystem->IsDevMode())
		{
			if (pNetCVars->InactivityTimeoutDevmode > 0.0f)
			{
				return (uint32)(1000.0f * pNetCVars->InactivityTimeoutDevmode);
			}
		}
		else
		{
			if (pNetCVars->InactivityTimeout > 0.0f)
			{
				return (uint32)(1000.0f * pNetCVars->InactivityTimeout);
			}
		}
	}

	return DrxLobbyTimeOut;
}

void CDrxLobby::LogPacketsInBuffer(const uint8* pBuffer, uint32 size)
{
#if !defined(_RELEASE) || defined(RELEASE_LOGGING)
	CDrxSharedLobbyPacket packet;
	SDrxLobbyPacketHeader* pPacketHeader = packet.GetLobbyPacketHeader();
	SDrxLobbyPacketDataHeader* pDataHeader = packet.GetLobbyPacketDataHeader();

	packet.SetReadBuffer(pBuffer, size);
	pPacketHeader->reliable = true; // No point reading whole header for this log
	packet.SetReadBufferPos(DrxLobbyPacketReliablePacketHeaderSize);

	while (packet.GetReadBufferPos() < size)
	{
		packet.ReadDataHeader();
		uint32 encodedPacketType = pDataHeader->lobbyPacketType;
		DecodePacketDataHeader(&packet);
		NetLog("    packet %d (%d) size %d", pDataHeader->lobbyPacketType, encodedPacketType, pDataHeader->dataSize);
		packet.SetReadBufferPos(packet.GetReadBufferPos() + pDataHeader->dataSize);
	}
#endif // #if !defined(_RELEASE) || defined(RELEASE_LOGGING)
}

void CDrxLobby::Tick(bool flush)
{
	if (gEnv->IsEditor() || (gEnv->pSystem && gEnv->pSystem->IsQuitting()))
	{
		return;
	}

	ProcessCachedPacketBuffer();

	uint32 lastFrameTime = 0;
	uint32 disconnectTimeOut = GetDisconnectTimeOut();
	uint32 DrxLobbyKeepAliveInterval = LOBBY_KEEP_ALIVE_INTERVAL;

	g_time = gEnv->pTimer->GetAsyncTime();

	if (!flush)
	{
		lastFrameTime = (uint32)(g_time.GetMilliSecondsAsInt64() - m_lastTickTime.GetMilliSecondsAsInt64());
		if (lastFrameTime > DrxLobbySendInterval)
		{
			NetLog("CDrxLobby::TimerCallback() long frame detected, lastFrameTime=%u", lastFrameTime);
			// Guard against very long frames that can cause sends to time out even before they have started.
			lastFrameTime = DrxLobbySendInterval;
		}
	}

#if USE_LOBBY_REMOTE_CONNECTIONS
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SConnection* pConnection = &m_connection[i];

		if (pConnection->used)
		{
			bool doneSend = false;

			pConnection->timeSinceSend += lastFrameTime;
			pConnection->timeSinceRecv += lastFrameTime;

			switch (pConnection->state)
			{
			case eCLCS_NotConnected:
				if (pConnection->refCount == 0)
				{
					SECURE_NET_LOG("[lobby] Release connection " PRFORMAT_LCINFO, PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));

					FreeConnection(i);
				}

				break;

			case eCLCS_Pending:
				{
					if (pConnection->timeSinceSend >= DrxLobbySendInterval)
					{
						const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize
	#if RESET_CONNECTED_CONNECTION
						                             + DrxLobbyPacketUINT64Size;
	#endif
						;
						uint8 buffer[MaxBufferSize];
						CDrxSharedLobbyPacket packet;

						packet.SetWriteBuffer(buffer, MaxBufferSize);
						doneSend = true;
						packet.StartWrite(eLobbyPT_ConnectionRequest, false);

	#if RESET_CONNECTED_CONNECTION
						SECURE_NET_LOG("[Lobby] Sending connection request with cookie %llx connection " PRFORMAT_LCINFO, pConnection->sendCookie, PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
						packet.WriteUINT64(pConnection->sendCookie);
	#endif

						if (Send(&packet, pConnection->addr, i, NULL) != eSE_Ok)
						{
							SECURE_NET_LOG("[Lobby] Failed to send setting pending connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
							pConnection->state = eCLCS_NotConnected;
						}
					}
				}

				break;

			case eCLCS_Connected:
				if (((pConnection->timeSinceRecv > DrxLobbyKeepAliveInterval) && (pConnection->timeSinceSend > DrxLobbyKeepAliveInterval)) ||
				    ((pConnection->timeSinceSend >= DrxLobbySendInterval) && (pConnection->ping.times[NUM_LOBBY_PINGS - 1] == DRXLOBBY_INVALID_PING)) ||
				    (pConnection->timeSinceSend >= DrxLobbyInGameSendInterval))
				{
					const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT64Size;
					uint8 buffer[MaxBufferSize];
					CDrxSharedLobbyPacket packet;

					packet.SetWriteBuffer(buffer, MaxBufferSize);
					doneSend = true;
					packet.StartWrite(eLobbyPT_Ping, false);
					packet.WriteUINT64(gEnv->pTimer->GetAsyncTime().GetMilliSecondsAsInt64());

					if (Send(&packet, pConnection->addr, i, NULL) != eSE_Ok)
					{
						SECURE_NET_LOG("[Lobby] Failed to send setting connected connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
						pConnection->state = eCLCS_NotConnected;
					}
				}

				if (pConnection->refCount == 0)
				{
					// There are no references to this connection.
					if (pConnection->dataQueue.Empty())
					{
						pConnection->disconnectTimer += lastFrameTime;

						if (pConnection->disconnectTimer > disconnectTimeOut)
						{
							// It hasn't been in use for a while
							SECURE_NET_LOG("[Lobby] No references to connection setting connected connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
							pConnection->state = eCLCS_NotConnected;
						}
					}
					else
					{
						// It's still in use so reset counter.
						pConnection->disconnectTimer = 0;
					}
				}
				else
				{
					pConnection->disconnectTimer = 0;
				}

				break;
			}

			if (pConnection->timeSinceRecv > disconnectTimeOut)
			{
				// It hasn't been in use for a while
				if (pConnection->state != eCLCS_NotConnected)
				{
					SECURE_NET_LOG("[Lobby] Connection to " PRFORMAT_LCINFO " has timed out (timeSinceRecv %u disconnectTimeOut %u) setting to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr), pConnection->timeSinceRecv, disconnectTimeOut);
					pConnection->state = eCLCS_NotConnected;
				}
			}

			if (pConnection->dataQueue.Empty() && (pConnection->reliableBuildPacket.GetWriteBufferPos() > 0))
			{
				AddReliablePacketToSendQueue(i, &pConnection->reliableBuildPacket);
			}

			if (!pConnection->dataQueue.Empty())
			{
				if ((pConnection->timeSinceSend >= DrxLobbySendInterval) || flush)
				{
					SConnection::SData& data = pConnection->dataQueue.Front();

	#if !defined(_RELEASE) || defined(RELEASE_LOGGING)
					CDrxSharedLobbyPacket packetInfo;
					SDrxLobbyPacketHeader* pPacketHeader = packetInfo.GetLobbyPacketHeader();

					packetInfo.SetReadBuffer((const uint8*)MemGetPtr(data.data), data.dataSize);

					packetInfo.ReadPacketHeader();

					SECURE_NET_LOG("[lobby] Send reliable connection " PRFORMAT_LCINFO " from " PRFORMAT_UID " counter %d size %d",
					               PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr), PRARG_UID(pPacketHeader->fromUID), data.counter, data.dataSize);

					LogPacketsInBuffer((uint8*)MemGetPtr(data.data), data.dataSize);
	#endif  // #if !defined(_RELEASE) || defined(RELEASE_LOGGING)

					doneSend = true;

					// Data hasn't been acknowledged so try sending again
					SSocketService* pSocketService = GetCorrectSocketServiceForAddr(pConnection->addr);
					if (pSocketService->m_socket)
					{
						if (pSocketService->m_socket->Send(EncryptPacket((uint8*)MemGetPtr(data.data), data.dataSize), data.dataSize, pConnection->addr) != eSE_Ok)
						{
							SECURE_NET_LOG("[Lobby] Failed to send setting connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
							pConnection->state = eCLCS_NotConnected;
						}
					}
				}

				if ((pConnection->timeSinceRecv > disconnectTimeOut) || (pConnection->state == eCLCS_NotConnected))
				{
					// Timeout on send
					OnError(pConnection->addr, eSE_UnreachableAddress);
				}
			}
			else
			{
				if (pConnection->state == eCLCS_Freeing)
				{
					SECURE_NET_LOG("[Lobby] Setting freeing connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));
					pConnection->state = eCLCS_NotConnected;
				}
			}

			if (doneSend)
			{
				pConnection->timeSinceSend = 0;
			}
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	if (m_mutex.TryLock())
	{
		m_fromGameQueue.Flush(false);
		m_mutex.Unlock();
	}

	if (flush)
	{
		// We're flushing, so no need to tick services or reset the timer
		return;
	}

	// Tick all lobby services.
	// Things like invite notifications for a service still need to be processed even if currently using a different service
	for (uint32 i = 0; i < eCLS_NumServices; i++)
	{
		if (m_services[i])
		{
			m_services[i]->Tick(g_time);
		}
	}

	m_lastTickTime = g_time;
}

void CDrxLobby::InitialiseServiceCB(EDrxLobbyError error, CDrxLobby* lobby, EDrxLobbyService service)
{
	if (error != eCLE_Success)
	{
		lobby->m_services[service] = NULL;
	}

	if (lobby->m_task[service].cb)
	{
		lobby->m_task[service].cb(service, error, lobby->m_task[service].cbArg);
	}
}

void CDrxLobby::TerminateServiceCB(EDrxLobbyError error, CDrxLobby* lobby, EDrxLobbyService service)
{
	// Remove our reference to the service. It will be deleted when all references are gone.
	lobby->m_services[service] = NULL;

	lobby->CheckFreeGlobalResources();

	if (lobby->m_task[service].cb)
	{
		lobby->m_task[service].cb(service, error, lobby->m_task[service].cbArg);
	}
}

#define MAX_IP_EXPECTED  (16)
#define MAX_ADDRESS_SIZE (MAX_IP_EXPECTED + 6 + 1)

void FixPortBinding(TNetAddress& defaultBind, int port)
{
#if DRX_PLATFORM_WINDOWS                      // On windows we should check the sv_bind variable and attempt to play nice and bind to the address requested
	CNetAddressResolver* pResolver = CDrxLobby::GetResolver();

	if (pResolver)
	{
		char address[MAX_ADDRESS_SIZE];

		drx_sprintf(address, "0.0.0.0:%d", port);

		ICVar* pCVar = gEnv->pConsole->GetCVar("sv_bind");
		if (pCVar && pCVar->GetString())
		{
			if (strlen(pCVar->GetString()) <= MAX_IP_EXPECTED)
			{
				drx_sprintf(address, "%s:%d", pCVar->GetString(), port);
			}
			else
			{
				NetLog("Address to bind '%s' does not appear to be an IP address", pCVar->GetString());
			}
		}

		CNameRequestPtr pReq = pResolver->RequestNameLookup(address);
		pReq->Wait();
		if (pReq->GetResult() != eNRR_Succeeded)
		{
			NetLog("Name resolution for '%s' failed - binding to 0.0.0.0", address);
		}
		else
		{
			defaultBind = pReq->GetAddrs()[0];
		}

	}
#endif // DRX_PLATFORM_WINDOWS
}

void CDrxLobby::InternalSocketCreate(EDrxLobbyService service)
{
	SSocketService* pSocketService = GetCorrectSocketService(service);

	DRX_ASSERT_MESSAGE(service < eCLS_NumServices, "Illegal service specified");
	DRX_ASSERT_MESSAGE(m_services[service] != NULL, "Tried to create a socket for a non existant service.");

	pSocketService->m_socketConnectPort = 0;
	pSocketService->m_socketListenPort = 0;

	if (m_services[service] != NULL)
	{
		m_services[service]->GetSocketPorts(pSocketService->m_socketConnectPort, pSocketService->m_socketListenPort);
	}

	uint16 port = pSocketService->m_socketListenPort;
	TNetAddress defaultBind = TNetAddress(SIPv4Addr(0, port));
	FixPortBinding(defaultBind, port);

	ISocketIOUpr* pSocketIOUpr = GetExternalSocketIOUpr();
	uint32 flags = eSF_Online;

	if (service == eCLS_LAN)
	{
		pSocketIOUpr = GetInternalSocketIOUpr();
		flags = (eSF_BroadcastSend | eSF_BroadcastReceive);
	}

	if (pSocketIOUpr && !pSocketService->m_socket)
	{
		InternalSocketFree(service);

		NetLog("[Lobby] Creating socket on port %u (service [%s], socket manager [%s])", port, (service == eCLS_LAN) ? "LAN" : "Online", pSocketIOUpr->GetName());
		pSocketService->m_socket = pSocketIOUpr->CreateDatagramSocket(defaultBind, flags);

		if (!pSocketService->m_socket)
		{
			NetLog("[Lobby] Failed to create socket on port %u", port);
			if ((gEnv->IsDedicated() == false) || ((gEnv->IsClient() == true) && (gEnv->bServer == false)))
			{
				// If we are not a dedicated server (Which should always use the port specified in the cfg file) and we fail to create the socket
				// try to create on some different ports.

				for (uint32 i = 1; i < MAX_SOCKET_PORTS_TRY; i++)
				{
					port++;
					defaultBind = TNetAddress(SIPv4Addr(0, port));
					FixPortBinding(defaultBind, port);
					NetLog("[Lobby] Creating socket on port %u", port);
					pSocketService->m_socket = pSocketIOUpr->CreateDatagramSocket(defaultBind, flags);

					if (pSocketService->m_socket)
					{
						break;
					}
					else
					{
						NetLog("[Lobby] Failed to create socket on port %u", port);
					}
				}
			}
		}

		if (pSocketService->m_socket)
		{
			NetLog("[Lobby] Created socket on port %u", port);
			pSocketService->m_socket->RegisterListener(this);
			pSocketService->m_socketListenPort = port;
			m_serviceParams[service].m_listenPort = port;
			if (m_services[service] != NULL)
			{
				m_services[service]->OnInternalSocketChanged(pSocketService->m_socket);
			}
		}
		else
		{
			NetLog("[Lobby] Socket could not be created, check firewall or try a different port. Connecting to network games may fail if not fixed.");
		}
	}
}

void CDrxLobby::InternalSocketDie(EDrxLobbyService service)
{
	DRX_ASSERT_MESSAGE(service < eCLS_NumServices, "Illegal service specified");
	SSocketService* pSocketService = GetCorrectSocketService(service);
	ISocketIOUpr* pSocketIOUpr = GetExternalSocketIOUpr();
	if (service == eCLS_LAN)
	{
		pSocketIOUpr = GetInternalSocketIOUpr();
	}

	if (pSocketIOUpr && pSocketService->m_socket)
	{
		pSocketService->m_socket->UnregisterListener(this);
		pSocketIOUpr->FreeDatagramSocket(pSocketService->m_socket);
		pSocketService->m_socket = NULL;
	}

	if (m_services[service])
	{
		m_services[service]->OnInternalSocketChanged(pSocketService->m_socket);
	}
}

void CDrxLobby::InternalSocketFree(EDrxLobbyService service)
{
	SSocketService* pSocketService = GetCorrectSocketService(service);
	if (pSocketService->m_socket)
	{
		InternalSocketDie(service);
	}
}

EDrxLobbyError CDrxLobby::CheckAllocGlobalResources()
{
	g_pObjcnt = &((INetworkPrivate*)gEnv->pNetwork)->GetObjectCounters();
	return eCLE_Success;
}

void CDrxLobby::CheckFreeGlobalResources()
{
	uint32 i;

	for (i = 0; i < eCLS_NumServices; i++)
	{
		if (m_services[i])
		{
			break;
		}
	}

	if (i == eCLS_NumServices)
	{
		int a;

		for (i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			m_serviceTask[i].used = false;
		}

		for (i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
		{
			FreeConnection(i);
		}

		for (a = 0; a < eCLS_NumServices; a++)
		{
			InternalSocketDie(EDrxLobbyService(a));
		}
	}
}

DrxLobbyTaskID CDrxLobby::CreateTask()
{
	for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
	{
		if (!m_serviceTask[i].used)
		{
			m_serviceTask[i].used = true;
#if ENABLE_DRXLOBBY_DEBUG_TESTS
			m_serviceTask[i].startTaskTime = 0LL;
			m_serviceTask[i].tickTaskTime = 0LL;
			m_serviceTask[i].generateErrorDone = false;
			m_serviceTask[i].startTaskTimerStarted = false;
			m_serviceTask[i].startTaskDone = false;
			m_serviceTask[i].tickTaskTimerStarted = false;
			m_serviceTask[i].tickTaskDone = false;
#endif
			return i;
		}
	}

	return DrxLobbyInvalidTaskID;
}

void CDrxLobby::ReleaseTask(DrxLobbyTaskID id)
{
	m_serviceTask[id].used = false;
}

#if ENABLE_DRXLOBBY_DEBUG_TESTS
bool CDrxLobby::DebugGenerateError(DrxLobbyTaskID id, EDrxLobbyError& error)
{
	if (CLobbyCVars::Get().cldEnable)
	{
		SServiceTask* pTask = &m_serviceTask[id];

		if (!pTask->generateErrorDone)
		{
			pTask->generateErrorDone = true;

			if (drx_random(0, 99) < CLobbyCVars::Get().cldErrorPercentage)
			{
				NetLog("[Lobby] Generating random eCLE_TimeOut on task %u", id);
				error = eCLE_TimeOut;
				return true;
			}
		}
	}

	return false;
}

bool CDrxLobby::DebugOKToStartTaskRunning(DrxLobbyTaskID id)
{
	int minDelay = CLobbyCVars::Get().cldMinDelayTime;
	int maxDelay = CLobbyCVars::Get().cldMaxDelayTime;

	if (CLobbyCVars::Get().cldEnable && (minDelay >= 0) && (maxDelay > minDelay))
	{
		SServiceTask* pTask = &m_serviceTask[id];

		if (!pTask->startTaskTimerStarted)
		{
			CTimeValue addTime;
			int64 addMilliSeconds;

			addMilliSeconds = drx_random(minDelay, maxDelay);

			addTime.SetMilliSeconds(addMilliSeconds);

			pTask->startTaskTimerStarted = true;
			pTask->startTaskDone = false;
			pTask->startTaskTime = g_time + addTime;

			NetLog("[Lobby] Delay start task %u for %" PRIi64 " milliseconds", id, addMilliSeconds);

			return false;
		}

		NetLog("[Lobby] Delayed task %u started", id);
	}

	return true;
}

bool CDrxLobby::DebugTickCallStartTaskRunning(DrxLobbyTaskID id)
{
	if (CLobbyCVars::Get().cldEnable)
	{
		SServiceTask* pTask = &m_serviceTask[id];

		if (pTask->startTaskTimerStarted && !pTask->startTaskDone)
		{
			if (g_time > pTask->startTaskTime)
			{
				pTask->startTaskDone = true;
				return true;
			}
		}
	}

	return false;
}

bool CDrxLobby::DebugOKToTickTask(DrxLobbyTaskID id, bool running)
{
	int minDelay = CLobbyCVars::Get().cldMinDelayTime;
	int maxDelay = CLobbyCVars::Get().cldMaxDelayTime;

	if (CLobbyCVars::Get().cldEnable && (minDelay >= 0) && (maxDelay > minDelay))
	{
		SServiceTask* pTask = &m_serviceTask[id];

		if (running && !pTask->startTaskDone)
		{
			if (!pTask->tickTaskTimerStarted)
			{
				CTimeValue addTime;
				int64 addMilliSeconds;

				addMilliSeconds = drx_random(minDelay, maxDelay);

				addTime.SetMilliSeconds(addMilliSeconds);

				pTask->tickTaskTimerStarted = true;
				pTask->tickTaskTime = g_time + addTime;

				NetLog("[Lobby] Delay start ticking task %u for %" PRIi64 " milliseconds", id, addMilliSeconds);

				return false;
			}
			else
			{
				if (g_time > pTask->tickTaskTime)
				{
					if (!pTask->tickTaskDone)
					{
						NetLog("[Lobby] Delayed task %u ticking", id);

						pTask->tickTaskDone = true;
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}
#endif

EDrxLobbyError CDrxLobby::Initialise(EDrxLobbyService service, EDrxLobbyServiceFeatures features, DrxLobbyConfigurationCallback cfgCb, DrxLobbyCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	if (features & eCLSO_Base)
	{
		error = CheckAllocGlobalResources();
	}

	assert(service >= 0 && service < eCLS_NumServices);

	if (error == eCLE_Success)
	{
		switch (service)
		{
		case eCLS_LAN:
			//#if !defined(_RELEASE) && !defined(DEDICATED_SERVER) && !defined(PURE_CLIENT)
#if USE_LAN
			if (m_services[eCLS_LAN] == NULL)
			{
				m_services[eCLS_LAN] = new CDrxLANLobbyService(this, service);

				if (!m_services[eCLS_LAN])
				{
					error = eCLE_OutOfMemory;
				}
			}
#else
			error = eCLE_ServiceNotSupported;
#endif // USE_LAN

			break;

		case eCLS_Online:
			if (m_services[eCLS_Online] == NULL)
			{
#if USE_STEAM
				if (CLobbyCVars::Get().useSteamAsOnlineLobby)
				{
					m_services[eCLS_Online] = new CDrxSteamLobbyService(this, service);

					if (!m_services[eCLS_Online])
					{
						error = eCLE_OutOfMemory;
					}
				}
				else
				{
					error = eCLE_ServiceNotSupported;
				}
#else
	#if USE_LIVE
				m_services[eCLS_Online] = new CDrxLiveLobbyService(this, service);

				if (!m_services[eCLS_Online])
				{
					error = eCLE_OutOfMemory;
				}
	#elif USE_PSN
				m_services[eCLS_Online] = new CDrxPSNLobbyService(this, service);

				if (!m_services[eCLS_Online])
				{
					error = eCLE_OutOfMemory;
				}
	#elif USE_DURANGOLIVE
				m_services[eCLS_Online] = new CDrxDurangoLiveLobbyService(this, service);

				if (!m_services[eCLS_Online])
				{
					error = eCLE_OutOfMemory;
				}
	#else
				m_services[eCLS_Online] = NULL;
				error = eCLE_ServiceNotSupported;
	#endif
#endif
				break;
			}
		}

		if (error == eCLE_Success)
		{
			m_task[service].cb = cb;
			m_task[service].cbArg = cbArg;
			this->m_configCB = cfgCb;

			error = m_services[service]->Initialise(features, (features & eCLSO_Base) ? InitialiseServiceCB : NULL);

			if (error == eCLE_Success)
			{
				if (features & eCLSO_Base)
				{
					FROM_GAME_TO_LOBBY(&CDrxLobbyService::CreateSocketNT, m_services[service].get());
				}
			}
			else
			{
				// Task didn't start so remove our reference to the service
				m_services[service] = NULL;
			}
		}
	}

	if ((error == eCLE_Success) && (features & eCLSO_Base))
	{
		// Final step, initialise lobby parameters
		m_services[service]->GetSocketPorts(m_serviceParams[service].m_connectPort, m_serviceParams[service].m_listenPort);
	}

	DrxLog("[Lobby] Initialise service %d error %d\n", service, error);

	return error;
}

EDrxLobbyError CDrxLobby::Terminate(EDrxLobbyService service, EDrxLobbyServiceFeatures features, DrxLobbyCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;
	EDrxLobbyError error;

	if (m_services[service])
	{
		m_task[service].cb = cb;
		m_task[service].cbArg = cbArg;

		error = m_services[service]->Terminate(features, (features & eCLSO_Base) ? TerminateServiceCB : NULL);

		if (features & eCLSO_Base)
		{
			m_services[service] = NULL;
		}
	}
	else
	{
		error = eCLE_NotInitialised;
	}

	{
		if (features & eCLSO_Base)
		{
			CheckFreeGlobalResources();
		}
	}
	DrxLog("[Lobby] Terminate service %d error %d\n", service, error);

	return error;
}

EDrxLobbyError CDrxLobby::ProcessEvents()
{
	if (m_safetyToGameMutex.TryLock())
	{
		m_toGameQueue.Flush(false);
		m_safetyToGameMutex.Unlock();
		return eCLE_Success;
	}

	return eCLE_SuccessContinue;
}

DrxLobbyConnectionID CDrxLobby::CreateConnection(const TNetAddress& address)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		if (!m_connection[i].used)
		{
			SConnection* pConnection = &m_connection[i];

	#if RESET_CONNECTED_CONNECTION
			pConnection->sendCookie = gEnv->pTimer->GetAsyncTime().GetValue();
			pConnection->recvCookie = INVALID_CONNECTION_COOKIE;
	#endif

			pConnection->timeSinceSend = 0;
			pConnection->timeSinceRecv = 0;
			pConnection->disconnectTimer = 0;
			pConnection->addr = address;
			pConnection->state = eCLCS_Pending;
			pConnection->refCount = 0;
			pConnection->counterIn = 0;
			pConnection->counterOut = 0;
			pConnection->used = true;
			pConnection->reliableBuildPacket.SetWriteBuffer(pConnection->reliableBuildPacketBuffer, MAX_LOBBY_PACKET_SIZE);

			pConnection->ping.aveTime = DRXLOBBY_INVALID_PING;
			pConnection->ping.currentTime = 0;

			for (uint32 j = 0; j < NUM_LOBBY_PINGS; j++)
			{
				pConnection->ping.times[j] = DRXLOBBY_INVALID_PING;
			}

			EDrxLobbyService service = GetCorrectSocketServiceTypeForAddr(address);
			if (!GetInternalSocket(service))
			{
				NetLog("[Lobby] Socket was not previously ready, trying again");
				InternalSocketCreate(service);
			}

			SECURE_NET_LOG("[lobby] Create reliable connection " PRFORMAT_LCINFO, PRARG_LCINFO(DrxLobbyConnectionID(i), pConnection->addr));

			return i;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return DrxLobbyInvalidConnectionID;
}

DrxLobbyConnectionID CDrxLobby::FindConnection(const TNetAddress& address)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		if (m_connection[i].used && (m_connection[i].addr == address))
		{
			return i;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return DrxLobbyInvalidConnectionID;
}

void CDrxLobby::ConnectionAddRef(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* pConnection = &m_connection[c];

		if (pConnection->used)
		{
			SECURE_NET_LOG("RefCount of connection " PRFORMAT_LCINFO " is incrementing, %u->%u. Thread #%" PRI_THREADID, PRARG_LCINFO(c, pConnection->addr), pConnection->refCount, pConnection->refCount + 1u, DrxGetCurrentThreadId());

			pConnection->refCount++;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

bool CDrxLobby::KeepPacketAfterDisconnect(uint32 packetType)
{
	return ((packetType == eLobbyPT_SessionDeleteRemoteConnection) ||
	        (packetType == eLobbyPT_SessionRequestJoinResult));
}

void CDrxLobby::ConnectionRemoveRef(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* pConnection = &m_connection[c];

		if (pConnection->used)
		{
			if (pConnection->refCount > 0)
			{
				SECURE_NET_LOG("RefCount of connection " PRFORMAT_LCINFO " is decrementing, %u->%d. Thread #%" PRI_THREADID, PRARG_LCINFO(c, pConnection->addr), pConnection->refCount, pConnection->refCount - 1u, DrxGetCurrentThreadId());

				pConnection->refCount--;

				if (pConnection->refCount == 0)
				{
					if (pConnection->reliableBuildPacket.GetWriteBufferPos() > 0)
					{
						AddReliablePacketToSendQueue(c, &pConnection->reliableBuildPacket);
					}

					// We only need to keep eLobbyPT_SessionDeleteRemoteConnection in the send queue so the other end knows we are leaving.
					// All other packets should be removed as they are not important now.
					NetLog("[Lobby] %u packets in send queue", pConnection->dataQueue.Size());
					pConnection->counterOut -= (uint8) pConnection->dataQueue.Size();

					if (!pConnection->dataQueue.Empty())
					{
						uint32 numCheck = pConnection->dataQueue.Size();

						for (uint32 i = 0; i < numCheck; i++)
						{
							SConnection::SData& data = pConnection->dataQueue.Front();
							uint8* pData = (uint8*)MemGetPtr(data.data);
							CDrxSharedLobbyPacket packet;
							uint32 dataPos = 0;
							SDrxLobbyPacketDataHeader* pDataHeader = packet.GetLobbyPacketDataHeader();

							pConnection->dataQueue.Pop();

							while (GetNextPacketFromBuffer(pData, data.dataSize, dataPos, &packet))
							{
								uint32 encodedPacketType = pDataHeader->lobbyPacketType;

								DecodePacketDataHeader(&packet);

								if (KeepPacketAfterDisconnect(pDataHeader->lobbyPacketType))
								{
									NetLog("[Lobby] Keep packet %u (%u) Size %u", pDataHeader->lobbyPacketType, encodedPacketType, pDataHeader->dataSize);
									EncodePacketDataHeader(&packet);
									AddPacketToReliableBuildPacket(c, &packet);
								}
								else
								{
									NetLog("[Lobby] Remove packet %u Size %u", pDataHeader->lobbyPacketType, pDataHeader->dataSize);
								}
							}

							MemFree(data.data);
						}
					}

					if (pConnection->reliableBuildPacket.GetWriteBufferPos() > 0)
					{
						AddReliablePacketToSendQueue(c, &pConnection->reliableBuildPacket);

						// Put connection into freeing state
						// Any sends in queue will still try and send and no new sends will be accepted
						pConnection->state = eCLCS_Freeing;
					}
					else
					{
						SECURE_NET_LOG("[lobby] Release connection " PRFORMAT_LCINFO, PRARG_LCINFO(c, pConnection->addr));

						FreeConnection(c);
					}
				}
			}
			else
			{
	#if !defined(_RELEASE)
				DrxFatalError("RefCount of connection " PRFORMAT_LCINFO " is trying to drop below 0! Thread #%" PRI_THREADID ".", PRARG_LCINFO(c, pConnection->addr), DrxGetCurrentThreadId());
	#else
				SECURE_NET_LOG("RefCount of connection " PRFORMAT_LCINFO " is trying to drop below 0! Thread #%" PRI_THREADID ".", PRARG_LCINFO(c, pConnection->addr), DrxGetCurrentThreadId());
	#endif
			}
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

bool CDrxLobby::ConnectionHasReference(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* pConnection = &m_connection[c];

		if (pConnection->used)
		{
			return pConnection->refCount > 0;
		}
	}
#endif

	return false;
}

void CDrxLobby::ConnectionSetState(DrxLobbyConnectionID c, EDrxLobbyConnectionState state)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* pConnection = &m_connection[c];

		if (pConnection->used)
		{
			pConnection->state = state;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

EDrxLobbyConnectionState CDrxLobby::ConnectionGetState(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* pConnection = &m_connection[c];

		if (pConnection->used)
		{
			return pConnection->state;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return eCLCS_NotConnected;
}

void CDrxLobby::FreeConnection(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* connection = &m_connection[c];

		if (connection->used)
		{
			connection->used = false;

			while (!connection->dataQueue.Empty())
			{
				SConnection::SData& data = connection->dataQueue.Front();

				MemFree(data.data);
				connection->dataQueue.Pop();
			}
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

bool CDrxLobby::ConnectionFromAddress(DrxLobbyConnectionID* connection, const TNetAddress& address)
{
	*connection = FindConnection(address);

	return *connection != DrxLobbyInvalidConnectionID;
}

bool CDrxLobby::AddressFromConnection(TNetAddress& address, DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	assert(c == DrxLobbyInvalidConnectionID || c < MAX_LOBBY_CONNECTIONS);

	if (c < MAX_LOBBY_CONNECTIONS)
	{
		SConnection* connection = &m_connection[c];

		if (connection->used)
		{
			address = connection->addr;
			return true;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return false;
}

bool CDrxLobby::SetConnectionAddress(DrxLobbyConnectionID c, TNetAddress& address)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		SConnection* connection = &m_connection[c];

		if (connection->used)
		{
			connection->addr = address;
			return true;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return false;
}

void CDrxLobby::FlushMessageQueue()
{
	Tick(true);
}

bool CDrxLobby::AddReliablePacketToSendQueue(DrxLobbyConnectionID connectionID, CDrxLobbyPacket* pPacket)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	SConnection* pConnection = &m_connection[connectionID];

	if (!pConnection->dataQueue.Full())
	{
		SConnection::SData sdata;

		sdata.data = MemAlloc(pPacket->GetWriteBufferPos());

		if (sdata.data != TMemInvalidHdl)
		{
			uint8* pData = (uint8*)MemGetPtr(sdata.data);

			memcpy(pData, pPacket->GetWriteBuffer(), pPacket->GetWriteBufferPos());

			sdata.dataSize = pPacket->GetWriteBufferPos();
			sdata.counter = pConnection->counterOut;
			pConnection->dataQueue.Push(sdata);

			pPacket->SetWriteBufferPos(0);

			return true;
		}
	}
	else
	{
		SECURE_NET_LOG("[Lobby] Trying to send to connection " PRFORMAT_LCINFO " but the send queue is full", PRARG_LCINFO(connectionID, pConnection->addr));
		const int numPacketsInQueue = pConnection->dataQueue.Size();
		for (int i = 0; i < numPacketsInQueue; ++i)
		{
			SConnection::SData& data = pConnection->dataQueue[i];
			NetLog("counter %d size %d", data.counter, data.dataSize);
			LogPacketsInBuffer((uint8*)MemGetPtr(data.data), data.dataSize);
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return false;
}

bool CDrxLobby::AddPacketToReliableBuildPacket(DrxLobbyConnectionID connectionID, CDrxLobbyPacket* pPacket)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	SConnection* pConnection = &m_connection[connectionID];
	CDrxSharedLobbyPacket* pSBuildPacket = (CDrxSharedLobbyPacket*)&pConnection->reliableBuildPacket;
	SDrxLobbyPacketHeader* pBuildPacketHeader = pSBuildPacket->GetLobbyPacketHeader();
	SDrxLobbyPacketDataHeader* pBuildDataHeader = pSBuildPacket->GetLobbyPacketDataHeader();
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;
	SDrxLobbyPacketHeader* pPacketHeader = pSPacket->GetLobbyPacketHeader();
	SDrxLobbyPacketDataHeader* pDataHeader = pSPacket->GetLobbyPacketDataHeader();
	uint8* pDataBuffer = pSPacket->GetWriteBuffer() + DrxLobbyPacketReliableHeaderSize;
	uint32 dataSize = pSPacket->GetWriteBufferPos() - DrxLobbyPacketReliableHeaderSize;

	if (pSPacket->GetWriteBufferPos() > MAX_LOBBY_PACKET_SIZE)
	{
		if (pSBuildPacket->GetWriteBufferPos() > 0)
		{
			if (!AddReliablePacketToSendQueue(connectionID, pSBuildPacket))
			{
				return false;
			}
		}

		pConnection->counterOut++;

		*pBuildPacketHeader = *pPacketHeader;
		*pBuildDataHeader = *pDataHeader;

		pPacketHeader->counterOut = pConnection->counterOut;
		pPacketHeader->counterIn = pConnection->counterIn;
		pDataHeader->dataSize = dataSize;

		uint32 bufferPos = pSPacket->GetWriteBufferPos();
		pSPacket->SetWriteBufferPos(0);
		pSPacket->WritePacketHeader();
		pSPacket->WriteDataHeader();
		pSPacket->SetWriteBufferPos(bufferPos);

		if (AddReliablePacketToSendQueue(connectionID, pSPacket))
		{
			return true;
		}

		return false;
	}

	if (pSBuildPacket->GetWriteBufferPos() > 0)
	{
		// Send and start new if
		if (((pSBuildPacket->GetWriteBufferPos() + DrxLobbyPacketReliableDataHeaderSize + dataSize) > pSBuildPacket->GetReadBufferSize()) || // Data will not fit in current packet
		    (pBuildPacketHeader->fromUID.m_sid != pPacketHeader->fromUID.m_sid))                                                             // sid has changed due to host migration
		{
			if (!AddReliablePacketToSendQueue(connectionID, pSBuildPacket))
			{
				return false;
			}
		}
	}

	if (pSBuildPacket->GetWriteBufferPos() == 0)
	{
		// Build packet is empty so start with this packet
		pConnection->counterOut++;

		*pBuildPacketHeader = *pPacketHeader;
		*pBuildDataHeader = *pDataHeader;

		pBuildPacketHeader->counterOut = pConnection->counterOut;
		pBuildPacketHeader->counterIn = pConnection->counterIn;
		pBuildDataHeader->dataSize = dataSize;

		pSBuildPacket->WritePacketHeader();
		pSBuildPacket->WriteDataHeader();
		pSBuildPacket->WriteData(pDataBuffer, dataSize);
	}
	else
	{
		// Add data to end of current packet.
		*pBuildDataHeader = *pDataHeader;
		pBuildDataHeader->dataSize = dataSize;
		pSBuildPacket->WriteDataHeader();
		pSBuildPacket->WriteData(pDataBuffer, dataSize);
	}

	return true;
#else // USE_LOBBY_REMOTE_CONNECTIONS
	return false;
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

void CDrxLobby::CalculatePacketTypeEncoding()
{
	CMTRand_int32 r(DRXLOBBY_PACKET_TYPE_ENCODE_SEED);
	uint32 i;

	for (i = 0; i < 256; i++)
	{
		m_encodePacketType[i] = i;
	}

	for (i = 0; i < 1024; i++)
	{
		uint32 lhs = r.GenerateUint32() & 0xff;
		uint32 rhs = r.GenerateUint32() & 0xff;

		std::swap(m_encodePacketType[lhs], m_encodePacketType[rhs]);
	}

	for (i = 0; i < 256; i++)
	{
		m_decodePacketType[m_encodePacketType[i]] = i;
	}

	for (i = m_servicePacketEnd[eCLS_LAN]; i <= DRXLANLOBBY_PACKET_MAX; i++)
	{
		m_decodePacketType[m_encodePacketType[i]] = eLobbyPT_InvalidPackeType;
	}

	for (i = m_servicePacketEnd[eCLS_Online]; i <= DRXONLINELOBBY_PACKET_MAX; i++)
	{
		m_decodePacketType[m_encodePacketType[i]] = eLobbyPT_InvalidPackeType;
	}

	for (i = m_userPacketEnd; i <= DRXLOBBY_USER_PACKET_MAX; i++)
	{
		m_decodePacketType[m_encodePacketType[i]] = eLobbyPT_InvalidPackeType;
	}
}

void CDrxLobby::EncodePacketDataHeader(CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;
	SDrxLobbyPacketDataHeader* pDataHeader = pSPacket->GetLobbyPacketDataHeader();

	pDataHeader->lobbyPacketType = m_encodePacketType[pDataHeader->lobbyPacketType];
}

void CDrxLobby::DecodePacketDataHeader(CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;
	SDrxLobbyPacketDataHeader* pDataHeader = pSPacket->GetLobbyPacketDataHeader();

	pDataHeader->lobbyPacketType = m_decodePacketType[pDataHeader->lobbyPacketType];
}

ESocketError CDrxLobby::Send(CDrxLobbyPacket* pPacket, const TNetAddress& to, DrxLobbyConnectionID connectionID, DrxLobbySendID* pSendID)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;
	ESocketError ret = eSE_Ok;

	EncodePacketDataHeader(pSPacket);

	if (pSPacket->GetReliable())
	{
		if (connectionID == DrxLobbyInvalidConnectionID)
		{
			connectionID = FindConnection(to);

			if (connectionID == DrxLobbyInvalidConnectionID)
			{
				connectionID = CreateConnection(to);
			}
		}

		if (connectionID != DrxLobbyInvalidConnectionID)
		{
			SConnection* pConnection = &m_connection[connectionID];

			if ((pConnection->state == eCLCS_Connected) || (pConnection->state == eCLCS_Pending))
			{
				if (AddPacketToReliableBuildPacket(connectionID, pSPacket))
				{
					if (pConnection->dataQueue.Empty())
					{
						AddReliablePacketToSendQueue(connectionID, &pConnection->reliableBuildPacket);

						SConnection::SData& data = pConnection->dataQueue.Front();

						SECURE_NET_LOG("[lobby] Send reliable connection " PRFORMAT_LCINFO " counter %d size %d", PRARG_LCINFO(connectionID, pConnection->addr), data.counter, data.dataSize);

						LogPacketsInBuffer((uint8*)MemGetPtr(data.data), data.dataSize);

						// Data hasn't been acknowledged so try sending again
						SSocketService* pSocketService = GetCorrectSocketServiceForAddr(pConnection->addr);
						if (pSocketService->m_socket)
						{
							if (pSocketService->m_socket->Send(EncryptPacket((uint8*)MemGetPtr(data.data), data.dataSize), data.dataSize, pConnection->addr) != eSE_Ok)
							{
								SECURE_NET_LOG("[Lobby] Failed to send setting connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(connectionID, pConnection->addr));
								pConnection->state = eCLCS_NotConnected;
							}
						}
					}

					if (pSendID)
					{
						*pSendID = DrxLobbyCreateSendID(connectionID, pConnection->counterOut);
					}
				}
				else
				{
					ret = eSE_MiscFatalError;
				}
			}
		}
		else
		{
			ret = eSE_MiscFatalError;
		}
	}
	else
	{
		SSocketService* pSocketService = GetCorrectSocketServiceForAddr(to);

		if (pSocketService->m_socket)
		{
			uint8* pBuffer = pSPacket->GetWriteBuffer();
			uint32 bufferSize = pSPacket->GetWriteBufferPos();

			pSPacket->SetWriteBufferPos(0);
			pSPacket->WritePacketHeader();
			pSPacket->WriteDataHeader();
			pSPacket->SetWriteBufferPos(bufferSize);

			if (pSendID)
			{
				*pSendID = DrxLobbyInvalidSendID;
			}

			ret = pSocketService->m_socket->Send(EncryptPacket(pBuffer, bufferSize), bufferSize, to);
		}
		else
		{
			ret = eSE_MiscFatalError;
		}
	}

	DecodePacketDataHeader(pSPacket);

	return ret;
#else // USE_LOBBY_REMOTE_CONNECTIONS
	return eSE_MiscFatalError;
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

ESocketError CDrxLobby::SendVoice(CDrxLobbyPacket* pPacket, const TNetAddress& to)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	SSocketService* pSocketService = GetCorrectSocketService(m_service);

	if (pSocketService->m_socket)
	{
		CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;
		uint8* pBuffer = pSPacket->GetWriteBuffer();
		uint32 bufferSize = pSPacket->GetWriteBufferPos();

		EncodePacketDataHeader(pSPacket);

		pSPacket->SetWriteBufferPos(0);
		pSPacket->WritePacketHeader();
		pSPacket->WriteDataHeader();
		pSPacket->SetWriteBufferPos(bufferSize);

		DecodePacketDataHeader(pSPacket);

		return pSocketService->m_socket->SendVoice(EncryptPacket(pBuffer, bufferSize), bufferSize, to);
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return eSE_MiscFatalError;
}

void CDrxLobby::UpdateConnectionPing(DrxLobbyConnectionID connectionID, DrxPing ping)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	SConnection* pConnection = &m_connection[connectionID];

	pConnection->ping.times[pConnection->ping.currentTime] = ping;
	pConnection->ping.currentTime = (pConnection->ping.currentTime + 1) % NUM_LOBBY_PINGS;

	uint32 numValidPings = 0;
	DrxPingAccumulator totalTime = 0;

	for (uint32 i = 0; i < NUM_LOBBY_PINGS; ++i)
	{
		if (pConnection->ping.times[i] != DRXLOBBY_INVALID_PING)
		{
			totalTime += pConnection->ping.times[i];
			++numValidPings;
		}
	}

	if (numValidPings)
	{
		pConnection->ping.aveTime = (DrxPing)(totalTime / numValidPings);
	}
	else
	{
		pConnection->ping.aveTime = DRXLOBBY_INVALID_PING;
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

DrxPing CDrxLobby::GetConnectionPing(DrxLobbyConnectionID connectionID)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if ((connectionID != DrxLobbyInvalidConnectionID) && (connectionID < MAX_LOBBY_CONNECTIONS))
	{
		return m_connection[connectionID].ping.aveTime;
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return DRXLOBBY_INVALID_PING;
}

bool CDrxLobby::GetNextPacketFromBuffer(const uint8* pData, uint32 dataSize, uint32& dataPos, CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;

	if (dataPos == 0)
	{
		pSPacket->SetReadBuffer(pData, dataSize);
		pSPacket->SetReadBufferPos(0);
		pSPacket->ReadPacketHeader();
		dataPos = DrxLobbyPacketReliablePacketHeaderSize;
	}

	if (dataPos < dataSize)
	{
		SDrxLobbyPacketDataHeader* pDataHeader = pSPacket->GetLobbyPacketDataHeader();

		pSPacket->SetReadBuffer(pData + dataPos - DrxLobbyPacketReliablePacketHeaderSize, dataSize - (dataPos - DrxLobbyPacketReliablePacketHeaderSize));
		pSPacket->SetReadBufferPos(DrxLobbyPacketReliablePacketHeaderSize);
		pSPacket->ReadDataHeader();
		pSPacket->SetReadBuffer(pData + dataPos - DrxLobbyPacketReliablePacketHeaderSize, pDataHeader->dataSize + DrxLobbyPacketReliableHeaderSize);
		pSPacket->SetReadBufferPos(DrxLobbyPacketReliableHeaderSize + pDataHeader->dataSize);
		dataPos += DrxLobbyPacketReliableDataHeaderSize + pDataHeader->dataSize;
		return true;
	}

	return false;
}

void CDrxLobby::ProcessPacket(const TNetAddress& addr, DrxLobbyConnectionID connectionID, CDrxLobbyPacket* pPacket)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	SConnection* pConnection = (connectionID != DrxLobbyInvalidConnectionID) ? &m_connection[connectionID] : NULL;

	switch (pPacket->StartRead())
	{
	case eLobbyPT_Ack:
		if (pConnection)
		{
			uint8 counterIn = pPacket->ReadUINT8();

			if (!pConnection->dataQueue.Empty())
			{
				SConnection::SData& qdata = pConnection->dataQueue.Front();

				if (counterIn == qdata.counter)
				{
					const uint8 qDataCounter = qdata.counter;
					// Other end has received the packet we are trying to send
					OnSendComplete(addr);
					SECURE_NET_LOG("[lobby] Got ack on connection " PRFORMAT_LCINFO " counterIn=%u, qDataCounter=%u", PRARG_LCINFO(connectionID, pConnection->addr), counterIn, qDataCounter);
				}
			}
		}

		break;

	case eLobbyPT_ConnectionRequest:
		{
	#if RESET_CONNECTED_CONNECTION
			uint64 cookie = pPacket->ReadUINT64();
	#endif

			if (connectionID == DrxLobbyInvalidConnectionID)
			{
				connectionID = CreateConnection(addr);
			}
			else
			{
				pConnection = &m_connection[connectionID];

				if (pConnection->state == eCLCS_Freeing)
				{
					SECURE_NET_LOG("[Lobby] Connection Request on existing Freeing connection " PRFORMAT_LCINFO, PRARG_LCINFO(connectionID, pConnection->addr));
					uint8 refCount = pConnection->refCount;
					FreeConnection(connectionID);
					connectionID = CreateConnection(addr);
					NetLog("[Lobby] setting refCount back to %u for resetted connection", refCount);
					pConnection = &m_connection[connectionID];
					pConnection->refCount = refCount;
				}
	#if RESET_CONNECTED_CONNECTION
				else
				{
					if ((pConnection->recvCookie != cookie) && (pConnection->recvCookie != INVALID_CONNECTION_COOKIE))
					{
						SECURE_NET_LOG("[Lobby] Connection Request on existing connected connection, current cookie %llx new cookie %llx Freeing connection " PRFORMAT_LCINFO, pConnection->recvCookie, cookie, PRARG_LCINFO(connectionID, pConnection->addr));
						CDrxMatchMaking* pMatchMaking = (CDrxMatchMaking*)GetMatchMaking();
						if (pMatchMaking)
						{
							pMatchMaking->ResetConnection(connectionID);
						}

						uint8 refCount = pConnection->refCount;

						FreeConnection(connectionID);
						connectionID = CreateConnection(addr);

						if (connectionID != DrxLobbyInvalidConnectionID)
						{
							NetLog("[Lobby] setting refCount back to %u for resetted connection", refCount);

							pConnection = &m_connection[connectionID];
							pConnection->refCount = refCount;
						}
					}
				}
	#endif
			}

			if (connectionID != DrxLobbyInvalidConnectionID)
			{
				pConnection = &m_connection[connectionID];

				const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize
	#if RESET_CONNECTED_CONNECTION
				                             + DrxLobbyPacketUINT64Size;
	#endif // RESET_CONNECTED_CONNECTION
				;
				uint8 buffer[MaxBufferSize];
				CDrxSharedLobbyPacket resultPacket;

	#if RESET_CONNECTED_CONNECTION
				pConnection->recvCookie = cookie;
	#endif

				pConnection->state = eCLCS_Connected;

				resultPacket.SetWriteBuffer(buffer, MaxBufferSize);
				resultPacket.StartWrite(eLobbyPT_ConnectionRequestResult, false);
	#if RESET_CONNECTED_CONNECTION
				SECURE_NET_LOG("[Lobby] Sending cookie %llx with connection " PRFORMAT_LCINFO, pConnection->sendCookie, PRARG_LCINFO(connectionID, pConnection->addr));
				resultPacket.WriteUINT64(pConnection->sendCookie);
	#endif

				if (Send(&resultPacket, pConnection->addr, connectionID, NULL) != eSE_Ok)
				{
					SECURE_NET_LOG("[Lobby] Failed to send setting connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(connectionID, pConnection->addr));
					pConnection->state = eCLCS_NotConnected;
				}
			}

			break;
		}

	case eLobbyPT_ConnectionRequestResult:
		if (pConnection)
		{
			pConnection->state = eCLCS_Connected;
	#if RESET_CONNECTED_CONNECTION
			pConnection->recvCookie = pPacket->ReadUINT64();
			SECURE_NET_LOG("[Lobby] Setting recvCookie %llx for connection " PRFORMAT_LCINFO, pConnection->recvCookie, PRARG_LCINFO(connectionID, pConnection->addr));
	#endif
		}

		break;

	case eLobbyPT_Ping:
		if (pConnection)
		{
			CTimeValue currentTime = gEnv->pTimer->GetAsyncTime();
			CTimeValue recvTime = pPacket->GetRecvTime();
			uint64 timeDelayed = (currentTime - recvTime).GetMilliSecondsAsInt64();
			const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT64Size;
			uint8 buffer[MaxBufferSize];
			CDrxSharedLobbyPacket pongPacket;

			pongPacket.SetWriteBuffer(buffer, MaxBufferSize);
			uint64 time = pPacket->ReadUINT64() + timeDelayed;

			pongPacket.StartWrite(eLobbyPT_Pong, false);
			pongPacket.WriteUINT64(time);

			if (Send(&pongPacket, pConnection->addr, connectionID, NULL) != eSE_Ok)
			{
				SECURE_NET_LOG("[Lobby] Failed to send setting connection " PRFORMAT_LCINFO " to eCLCS_NotConnected", PRARG_LCINFO(connectionID, pConnection->addr));
				pConnection->state = eCLCS_NotConnected;
			}
		}

		break;

	case eLobbyPT_Pong:
		if (pConnection)
		{
			CTimeValue recvTime = pPacket->GetRecvTime();
			uint64 sendTime = pPacket->ReadUINT64();
			uint64 pingTime = recvTime.GetMilliSecondsAsInt64() - sendTime;
			if (pingTime > DRXLOBBY_INVALID_PING)
			{
				pingTime = DRXLOBBY_INVALID_PING;
			}

			UpdateConnectionPing(connectionID, (DrxPing)pingTime);
		}

		break;

	case eLobbyPT_InvalidPackeType:
		if (pConnection)
		{
			pConnection->state = eCLCS_NotConnected;
		}

		break;

	default:
		if (m_services[m_service])
		{
			// Give packet to the current service
			m_services[m_service]->OnPacket(addr, pPacket);
		}

		break;
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

void CDrxLobby::OnPacket(const TNetAddress& addr, const uint8* pData, uint32 length)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	DrxLobbyConnectionID connectionID = FindConnection(addr);
	SConnection* pConnection = (connectionID != DrxLobbyInvalidConnectionID) ? &m_connection[connectionID] : NULL;

	if (pData[0] == GetLobbyPacketID())
	{
		m_cachedPacketBuffer.WritePacket(addr, pData, length);
	}

	if (pConnection)
	{
		pConnection->timeSinceRecv = 0;
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

void CDrxLobby::ProcessCachedPacketBuffer(void)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	while (m_cachedPacketBuffer.PacketsAvailable())
	{
		CTimeValue recvTime;
		TNetAddress addr;
		const uint8* pData;
		uint32 length;
		m_cachedPacketBuffer.ReadPacket(recvTime, addr, pData, length);

		DrxLobbyConnectionID connectionID = FindConnection(addr);
		SConnection* pConnection = (connectionID != DrxLobbyInvalidConnectionID) ? &m_connection[connectionID] : NULL;

		const uint8* pDecodedData;
		CDrxSharedLobbyPacket packet;
		SDrxLobbyPacketHeader* pPacketHeader = packet.GetLobbyPacketHeader();
		SDrxLobbyPacketDataHeader* pDataHeader = packet.GetLobbyPacketDataHeader();
		bool processPacket = true;

		pDecodedData = DecryptPacket(pData, length);

		packet.SetReadBuffer(pDecodedData, length);
		packet.SetReadBufferPos(0);
		packet.ReadPacketHeader();
		pPacketHeader->recvTime = recvTime;

		if (packet.GetReliable())
		{
			if (pConnection && ((pConnection->state == eCLCS_Pending) || (pConnection->state == eCLCS_Connected)))
			{
				SECURE_NET_LOG("[lobby] OnPacket reliable connection " PRFORMAT_LCINFO " size %u", PRARG_LCINFO(connectionID, pConnection->addr), length);

				if (((uint8)(pConnection->counterIn - pPacketHeader->counterOut)) == CONNECTION_COUNTER_MAX)
				{
					// This is a new packet
					NetLog("[lobby] New packet counter %d", pPacketHeader->counterOut);

					pConnection->counterIn = pPacketHeader->counterOut;

					if (!pConnection->dataQueue.Empty())
					{
						SConnection::SData& qdata = pConnection->dataQueue.Front();

						if (pPacketHeader->counterIn == qdata.counter)
						{
							const uint8 qDataCounter = qdata.counter;
							// Other end has received the packet we are trying to send
							OnSendComplete(addr);
							SECURE_NET_LOG("[lobby] Got ack(first) on connection " PRFORMAT_LCINFO " counterIn=%u, qDataCounter=%u", PRARG_LCINFO(connectionID, pConnection->addr), pPacketHeader->counterIn, qDataCounter);
						}
					}
				}
				else
				{
					processPacket = false;
					NetLog("[lobby] Repeat packet counter %d", pPacketHeader->counterOut);
				}

				const uint32 MaxBufferSize = DrxLobbyPacketUnReliableHeaderSize + DrxLobbyPacketUINT8Size;
				uint8 buffer[MaxBufferSize];
				CDrxLobbyPacket ackPacket;

				ackPacket.SetWriteBuffer(buffer, MaxBufferSize);
				ackPacket.StartWrite(eLobbyPT_Ack, false);
				ackPacket.WriteUINT8(pPacketHeader->counterOut);

				Send(&ackPacket, addr, connectionID, NULL);

				NetLog("[lobby] Send ack (counter=%u)", pPacketHeader->counterOut);
			}
			else
			{
				NetLog("[lobby] OnPacket reliable packet for non connected connection " PRFORMAT_LCINFO " size %u ignoring", PRARG_LCINFO(connectionID, addr), length);

				processPacket = false;

				// We don't have a valid connection for this reliable packet.
				// This will most likely happen if we have disconnected but the other end doesn't know and is trying to disconnect from us.
				// If this is one of the packet types that is kept and still sent after a disconnect send back an ack to stop the other end sending.
				uint32 dataPos = 0;
				bool sendAck = false;

				while (GetNextPacketFromBuffer(pDecodedData, length, dataPos, &packet))
				{
					uint32 encodedPacketType = pDataHeader->lobbyPacketType;
					DecodePacketDataHeader(&packet);

					if (KeepPacketAfterDisconnect(pDataHeader->lobbyPacketType))
					{
						sendAck = true;
						NetLog("    Ignoring packet %u (%u) size %u Still needs ack", pDataHeader->lobbyPacketType, encodedPacketType, pDataHeader->dataSize);
					}
					else
					{
						NetLog("    Ignoring packet %u (%u) size %u Doesn't need ack", pDataHeader->lobbyPacketType, encodedPacketType, pDataHeader->dataSize);
					}
				}

				if (sendAck)
				{
					const uint32 MaxBufferSize = DrxLobbyPacketUnReliableHeaderSize + DrxLobbyPacketUINT8Size;
					uint8 buffer[MaxBufferSize];
					CDrxLobbyPacket ackPacket;

					ackPacket.SetWriteBuffer(buffer, MaxBufferSize);
					ackPacket.StartWrite(eLobbyPT_Ack, false);
					ackPacket.WriteUINT8(pPacketHeader->counterOut);

					Send(&ackPacket, addr, connectionID, NULL);

					NetLog("[lobby] Send ack for non connected connection (counter=%u)", pPacketHeader->counterOut);
				}
			}
		}

		if (processPacket)
		{
			if (packet.GetReliable())
			{
				uint32 dataPos = 0;

				while (GetNextPacketFromBuffer(pDecodedData, length, dataPos, &packet))
				{
					uint32 encodedPacketType = pDataHeader->lobbyPacketType;
					DecodePacketDataHeader(&packet);
					NetLog("    Process packet %d (%d) size %d", pDataHeader->lobbyPacketType, encodedPacketType, pDataHeader->dataSize);
					ProcessPacket(addr, connectionID, &packet);
				}
			}
			else
			{
				packet.ReadDataHeader();
				DecodePacketDataHeader(&packet);
				ProcessPacket(addr, connectionID, &packet);
			}
		}
	}

	m_cachedPacketBuffer.Reset();
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

void CDrxLobby::OnError(const TNetAddress& addr, ESocketError error)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	DrxLobbyConnectionID c;
	DrxLobbySendID sendID = DrxLobbyInvalidSendID;

	if (ConnectionFromAddress(&c, addr))
	{
		SConnection* connection = &m_connection[c];

		while (!connection->dataQueue.Empty())
		{
			SConnection::SData& data = connection->dataQueue.Front();

			sendID = DrxLobbyCreateSendID(c, data.counter);

			MemFree(data.data);

			if (m_services[m_service])
			{
				m_services[m_service]->OnError(addr, error, sendID);
			}

			SECURE_NET_LOG("[lobby] Socket error %d on connection " PRFORMAT_LCINFO " free send data counter %u", error, PRARG_LCINFO(c, connection->addr), data.counter);

			connection->dataQueue.Pop();
		}

		connection->state = eCLCS_NotConnected;
	}
	else
	{
		if (m_services[m_service])
		{
			m_services[m_service]->OnError(addr, error, sendID);
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

void CDrxLobby::OnSendComplete(const TNetAddress& addr)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	DrxLobbyConnectionID c;
	DrxLobbySendID sendID = DrxLobbyInvalidSendID;

	if (ConnectionFromAddress(&c, addr))
	{
		SConnection* connection = &m_connection[c];

		if (!connection->dataQueue.Empty())
		{
			SConnection::SData& data = connection->dataQueue.Front();

			sendID = DrxLobbyCreateSendID(c, data.counter);

			SECURE_NET_LOG("[lobby] Send complete on connection " PRFORMAT_LCINFO " free send data counter %d", PRARG_LCINFO(c, connection->addr), data.counter);
			LogPacketsInBuffer((uint8*)MemGetPtr(data.data), data.dataSize);

			MemFree(data.data);

			connection->dataQueue.Pop();
		}
	}

	if (m_services[m_service])
	{
		m_services[m_service]->OnSendComplete(addr, sendID);
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

#if NETWORK_HOST_MIGRATION
uint8 CDrxLobby::GetActiveConnections(void)
{
	uint8 count = 0;

	for (uint32 index = 0; index < MAX_LOBBY_CONNECTIONS; ++index)
	{
		if (m_connection[index].used && (m_connection[index].state == eCLCS_Connected))
		{
			++count;
		}
	}

	return count;
}
#endif

void CDrxLobby::RegisterEventInterest(EDrxLobbySystemEvent eventOfInterest, DrxLobbyEventCallback cb, void* pUserData)
{
	SEventCBData newEvent;

	newEvent.cb = cb;
	newEvent.pUserData = pUserData;
	newEvent.event = eventOfInterest;

	m_callbacks.push_back(newEvent);
}

void CDrxLobby::UnregisterEventInterest(EDrxLobbySystemEvent eventOfInterest, DrxLobbyEventCallback cb, void* pUserData)
{
	SEventCBData newEvent;

	newEvent.cb = cb;
	newEvent.pUserData = pUserData;
	newEvent.event = eventOfInterest;

	stl::find_and_erase(m_callbacks, newEvent);
}

void CDrxLobby::DispatchEvent(EDrxLobbySystemEvent evnt, UDrxLobbyEventData data)
{
	EventCBList::iterator callbackItem;

	for (callbackItem = m_callbacks.begin(); callbackItem != m_callbacks.end(); ++callbackItem)
	{
		if (callbackItem->event == evnt)
		{
			callbackItem->cb(data, callbackItem->pUserData);
		}
	}
}

void CDrxLobby::GetConfigurationInformation(SConfigurationParams* infos, uint32 infoCnt)
{
	if (m_configCB)
	{
		m_configCB(GetLobbyServiceType(), infos, infoCnt);
	}
}

IDrxTCPServicePtr CDrxLobbyService::GetTCPService(const char* pService)
{
#if USE_DRX_TCPSERVICE
	if (m_pTCPServiceFactory)
	{
		return m_pTCPServiceFactory->GetTCPService(pService);
	}
#endif // USE_DRX_TCPSERVICE
	return NULL;
}

IDrxTCPServicePtr CDrxLobbyService::GetTCPService(const char* pServer, uint16 port, const char* pUrlPrefix)
{
#if USE_DRX_TCPSERVICE
	if (m_pTCPServiceFactory)
	{
		return m_pTCPServiceFactory->GetTCPService(pServer, port, pUrlPrefix);
	}
#endif // USE_DRX_TCPSERVICE
	return NULL;
}

bool CDrxLobbyService::GetFlag(EDrxLobbyServiceFlag flag)
{
	return (m_lobbyServiceFlags & flag) == flag;
}

// Описание:
//	 Is the given flag true for the lobby service of the given type?
// Arguments:
//	 service - lobby service to be queried
//	 flag - flag to be queried
// Return:
//	 True if the flag is true.
bool CDrxLobby::GetLobbyServiceFlag(EDrxLobbyService service, EDrxLobbyServiceFlag flag)
{
	bool result = false;

	if (m_services[service])
	{
		result = m_services[service]->GetFlag(flag);
	}

	return result;
}

CDrxLobbyService::CDrxLobbyService(CDrxLobby* pLobby, EDrxLobbyService service)
	: m_pLobby(pLobby)
	, m_service(service)
#if USE_DRX_TCPSERVICE
	, m_pTCPServiceFactory(NULL)
#endif // USE_DRX_TCPSERVICE
{
	m_lobbyServiceFlags = eCLSF_AllowMultipleSessions | eCLSF_SupportHostMigration;
	ZeroMemory(&m_tasks, sizeof(m_tasks));
}

CDrxLobbyService::~CDrxLobbyService()
{

}

EDrxLobbyError CDrxLobbyService::Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	if (features & eCLSO_Base)
	{
		LOBBY_AUTO_LOCK;

		for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			m_tasks[i].used = false;
		}
	}

	return eCLE_Success;
}

EDrxLobbyError CDrxLobbyService::Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB)
{
	if (features & eCLSO_Base)
	{
		LOBBY_AUTO_LOCK;

		FROM_GAME_TO_LOBBY(&CDrxLobbyService::FreeSocketNT, this);
	}

	return eCLE_Success;
}

void CDrxLobbyService::CreateSocketNT(void)
{
	m_pLobby->InternalSocketCreate(m_service);
}

void CDrxLobbyService::FreeSocketNT(void)
{
	m_pLobby->InternalSocketFree(m_service);
}

EDrxLobbyError CDrxLobbyService::StartTask(uint32 eTask, uint32 user, bool startRunning, DrxLobbyServiceTaskID* pLSTaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg)
{
	DrxLobbyTaskID lobbyTaskID = m_pLobby->CreateTask();

	if (lobbyTaskID != DrxLobbyInvalidTaskID)
	{
		for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			STask* pTask = GetTask(i);

			if (!pTask->used)
			{
				pTask->user = user;
				pTask->lTaskID = lobbyTaskID;
				pTask->error = eCLE_Success;
				pTask->startedTask = eTask;
				pTask->pCB = pCb;
				pTask->pCBArg = pCbArg;
				pTask->used = true;
				pTask->running = startRunning;

				if (pLSTaskID)
				{
					*pLSTaskID = i;
				}

				if (pLTaskID)
				{
					*pLTaskID = lobbyTaskID;
				}

				for (uint32 j = 0; j < MAX_LOBBY_TASK_DATAS; j++)
				{
					pTask->dataMem[j] = TMemInvalidHdl;
					pTask->dataNum[j] = 0;
				}

				return eCLE_Success;
			}
		}

		m_pLobby->ReleaseTask(lobbyTaskID);
	}

	return eCLE_TooManyTasks;
}

void CDrxLobbyService::FreeTask(DrxLobbyServiceTaskID lsTaskID)
{
	STask* pTask = GetTask(lsTaskID);

	m_pLobby->ReleaseTask(pTask->lTaskID);

	for (uint32 i = 0; i < MAX_LOBBY_TASK_DATAS; i++)
	{
		if (pTask->dataMem[i] != TMemInvalidHdl)
		{
			m_pLobby->MemFree(pTask->dataMem[i]);
			pTask->dataMem[i] = TMemInvalidHdl;
		}
	}

	pTask->used = false;
}

void CDrxLobbyService::CancelTask(DrxLobbyTaskID lTaskID)
{
	LOBBY_AUTO_LOCK;

	DrxLogAlways("[Lobby]Try cancel task %u", lTaskID);

	if (lTaskID != DrxLobbyInvalidTaskID)
	{
		for (uint32 i = 0; i < MAX_LOBBY_TASKS; i++)
		{
			STask* pTask = GetTask(i);

			DRX_ASSERT_MESSAGE(pTask, "CDrxLobby: Task base pointers not setup");

			if (pTask->used && (pTask->lTaskID == lTaskID))
			{
				DrxLogAlways("[Lobby] Task %u canceled", lTaskID);
				pTask->pCB = NULL;
				break;
			}
		}
	}
}

void CDrxLobbyService::UpdateTaskError(DrxLobbyServiceTaskID lsTaskID, EDrxLobbyError error)
{
	STask* pTask = GetTask(lsTaskID);

	if (pTask->error == eCLE_Success)
	{
		pTask->error = error;
	}
}

void CDrxLobbyService::StartTaskRunning(DrxLobbyServiceTaskID lsTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_GetUserPrivileges:
			StopTaskRunning(lsTaskID);
			break;
		}
	}
}

void CDrxLobbyService::StopTaskRunning(DrxLobbyServiceTaskID lsTaskID)
{
	STask* pTask = GetTask(lsTaskID);

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxLobbyService::EndTask, this, lsTaskID);
	}
}

void CDrxLobbyService::EndTask(DrxLobbyServiceTaskID lsTaskID)
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
				if (!m_pLobby->GetInternalSocket(m_service))
				{
					UpdateTaskError(lsTaskID, eCLE_InternalError);
				}

				((DrxLobbyPrivilegeCallback)pTask->pCB)(pTask->lTaskID, pTask->error, 0, pTask->pCBArg);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] Lobby Service EndTask %u Result %d", pTask->startedTask, pTask->error);
		}

		FreeTask(lsTaskID);
	}
}

EDrxLobbyError CDrxLobbyService::GetUserPrivileges(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyPrivilegeCallback pCB, void* pCBArg)
{
	EDrxLobbyError error;
	DrxLobbyServiceTaskID tid;

	LOBBY_AUTO_LOCK;

	error = StartTask(eT_GetUserPrivileges, user, false, &tid, pTaskID, (void*)pCB, pCBArg);

	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxLobbyService::StartTaskRunning, this, tid);
	}

	return error;
}

void CDrxLobbyService::GetSocketPorts(uint16& connectPort, uint16& listenPort)
{
	listenPort = CLobbyCVars::Get().lobbyDefaultPort;
	connectPort = CLobbyCVars::Get().lobbyDefaultPort;
}

void CDrxLobbyService::MakeAddrPCCompatible(TNetAddress& addr)
{
}

void CDrxLobby::MakeAddrPCCompatible(TNetAddress& addr)
{
	m_services[m_service]->MakeAddrPCCompatible(addr);
}

void CDrxLobby::MakeConnectionPCCompatible(DrxLobbyConnectionID connectionID)
{
	if (connectionID != DrxLobbyInvalidConnectionID)
	{
		m_services[m_service]->MakeAddrPCCompatible(m_connection[connectionID].addr);
	}
}

CDrxLobby::SSocketService* CDrxLobby::GetCorrectSocketServiceForAddr(const TNetAddress& addr)
{
	return &m_socketServices[GetCorrectSocketServiceTypeForAddr(addr)];
}

EDrxLobbyService CDrxLobby::GetCorrectSocketServiceTypeForAddr(const TNetAddress& addr)
{
	const SIPv4Addr* pIPv4Addr = stl::get_if<SIPv4Addr>(&addr);

	if (pIPv4Addr && (pIPv4Addr->lobbyService != eCLS_NumServices))
	{
		return EDrxLobbyService(pIPv4Addr->lobbyService);
	}

	return m_service;
}

CDrxLobby::SSocketService* CDrxLobby::GetCorrectSocketService(EDrxLobbyService serviceType)
{
	//Note that there will always be a 2 sockets no matter which services are initialised
	return &m_socketServices[serviceType];
}

uint16 CDrxLobby::GetInternalSocketPort(EDrxLobbyService service)
{
	SSocketService* pSocketService = GetCorrectSocketService(service);

	return pSocketService->m_socketListenPort;
}

IDatagramSocketPtr CDrxLobby::GetInternalSocket(EDrxLobbyService service)
{
	SSocketService* pSocketService = GetCorrectSocketService(service);

	return pSocketService->m_socket;
}

const SDrxLobbyParameters& CDrxLobby::GetLobbyParameters() const
{
	return m_serviceParams[m_service];
}

uint32 CDrxLobby::TimeSincePacketInMS(DrxLobbyConnectionID c) const
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		if (m_connection[c].used)
		{
			return m_connection[c].timeSinceRecv;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS

	return 0;
}

void CDrxLobby::ForceTimeoutConnection(DrxLobbyConnectionID c)
{
#if USE_LOBBY_REMOTE_CONNECTIONS
	if (c != DrxLobbyInvalidConnectionID)
	{
		if (m_connection[c].used)
		{
			m_connection[c].timeSinceRecv = LOBBY_FORCE_DISCONNECT_TIMER;
		}
	}
#endif // USE_LOBBY_REMOTE_CONNECTIONS
}

const TNetAddress* CDrxLobby::GetNetAddress(DrxLobbyConnectionID c) const
{
	const TNetAddress* pAddress = NULL;

	if ((c != DrxLobbyInvalidConnectionID) && (c < MAX_LOBBY_CONNECTIONS))
	{
		if (m_connection[c].used)
		{
			pAddress = &(m_connection[c].addr);
		}
	}

	return pAddress;
}

EDrxLobbyError CDrxLobbyService::CreateTaskParamMem(DrxLobbyServiceTaskID lsTaskID, uint32 param, const void* pParamData, size_t paramDataSize)
{
	STask* pTask = &m_tasks[lsTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxLobbyService: Task base pointers not setup");

	if (paramDataSize > 0)
	{
		pTask->dataMem[param] = m_pLobby->MemAlloc(paramDataSize);
		void* p = m_pLobby->MemGetPtr(pTask->dataMem[param]);

		if (p)
		{
			if (pParamData)
			{
				memcpy(p, pParamData, paramDataSize);
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

	return eCLE_Success;
}

const uint8* CDrxLobby::EncryptPacket(const uint8* buffer, uint32 length)
{
#if ENCRYPT_LOBBY_PACKETS
	if (gEnv->pNetwork)
	{
		m_tempEncryptionBuffer[0] = buffer[0];

		gEnv->pNetwork->EncryptBuffer(m_cipher, m_tempEncryptionBuffer + 1, buffer + 1, length - 1);

		return m_tempEncryptionBuffer;
	}
	else
#endif
	{
		return buffer;
	}
}

const uint8* CDrxLobby::DecryptPacket(const uint8* buffer, uint32 length)
{
#if ENCRYPT_LOBBY_PACKETS
	if (gEnv->pNetwork)
	{
		m_tempDecryptionBuffer[0] = buffer[0];

		gEnv->pNetwork->DecryptBuffer(m_cipher, m_tempDecryptionBuffer + 1, buffer + 1, length - 1);

		return m_tempDecryptionBuffer;
	}
	else
#endif
	{
		return buffer;
	}
}

bool CDrxLobby::DecodeAddress(const TNetAddress& address, uint32* pIP, uint16* pPort)
{
	bool decoded = true;

	const SIPv4Addr* pIPv4Addr = stl::get_if<SIPv4Addr>(&address);
	if (pIPv4Addr != NULL)
	{
		*pIP = pIPv4Addr->addr;
		*pPort = pIPv4Addr->port;
	}
	else
	{
		const TLocalNetAddress* pLocalAddr = stl::get_if<TLocalNetAddress>(&address);
		if (pLocalAddr != NULL)
		{
			*pIP = LOOPBACK_ADDRESS;
			*pPort = *pLocalAddr;
		}
		else
		{
			*pIP = *pPort = 0;
			decoded = false;
		}
	}

	return decoded;
}

void CDrxLobby::DecodeAddress(uint32 ip, uint16 port, char* ipstring, bool ignorePort)
{
	if (ignorePort)
	{
		sprintf(ipstring, "%u.%u.%u.%u", ((ip >> 24) & 0xff), ((ip >> 16) & 0xff), ((ip >> 8) & 0xff), (ip & 0xff));
	}
	else
	{
		sprintf(ipstring, "%u.%u.%u.%u:%u", ((ip >> 24) & 0xff), ((ip >> 16) & 0xff), ((ip >> 8) & 0xff), (ip & 0xff), port);
	}
}

void CDrxLobby::DecodeAddress(const TNetAddress& address, char* ipstring, bool ignorePort)
{
	uint32 ip;
	uint16 port;

	if (DecodeAddress(address, &ip, &port))
	{
		DecodeAddress(ip, port, ipstring, ignorePort);
	}
	else
	{
		sprintf(ipstring, "<UNKNOWN>");
	}
}

bool CDrxLobby::ConvertAddr(const TNetAddress& addrIn, DRXSOCKADDR_IN* pSockAddr)
{
	if (gEnv->pNetwork)
	{
		return ((INetworkPrivate*)gEnv->pNetwork)->ConvertAddr(addrIn, pSockAddr);
	}

	return false;
}

bool CDrxLobby::ConvertAddr(const TNetAddress& addrIn, DRXSOCKADDR* pSockAddr, int* addrLen)
{
	if (gEnv->pNetwork)
	{
		return ((INetworkPrivate*)gEnv->pNetwork)->ConvertAddr(addrIn, pSockAddr, addrLen);
	}

	return false;
}

ISocketIOUpr* CDrxLobby::GetExternalSocketIOUpr()
{
	if (gEnv->pNetwork)
	{
		return &((INetworkPrivate*)gEnv->pNetwork)->GetExternalSocketIOUpr();
	}

	return NULL;
}

ISocketIOUpr* CDrxLobby::GetInternalSocketIOUpr()
{
	if (gEnv->pNetwork)
	{
		return &((INetworkPrivate*)gEnv->pNetwork)->GetInternalSocketIOUpr();
	}

	return NULL;
}

CNetAddressResolver* CDrxLobby::GetResolver()
{
	if (gEnv->pNetwork)
	{
		return ((INetworkPrivate*)gEnv->pNetwork)->GetResolver();
	}

	return NULL;
}

const CNetCVars* CDrxLobby::GetNetCVars()
{
	if (gEnv->pNetwork)
	{
		return &((INetworkPrivate*)gEnv->pNetwork)->GetCVars();
	}

	return NULL;
}

uint8 CDrxLobby::GetLobbyPacketID()
{
	if (gEnv->pNetwork)
	{
		return ((INetworkPrivate*)gEnv->pNetwork)->FrameIDToHeader(eH_DrxLobby);
	}

	return 0;
}

#if NETWORK_HOST_MIGRATION
THostMigrationEventListenerContainer* CDrxLobby::GetHostMigrationListeners()
{
	return &m_hostMigrationListeners;
}
#endif

void CDrxLobby::AddHostMigrationEventListener(IHostMigrationEventListener* pListener, const char* pWho, uint32 priority)
{
#if NETWORK_HOST_MIGRATION
	for (THostMigrationEventListenerContainer::iterator it = m_hostMigrationListeners.begin(); it != m_hostMigrationListeners.end(); ++it)
	{
		if (it->second.m_pListener == pListener)
		{
			// The listener is already registered
			return;
		}
	}

	SHostMigrationEventListenerInfo info(pListener, pWho);
	m_hostMigrationListeners.insert(std::pair<EListenerPriorityType, SHostMigrationEventListenerInfo>((EListenerPriorityType)priority, info));
#endif
}

void CDrxLobby::RemoveHostMigrationEventListener(IHostMigrationEventListener* pListener)
{
#if NETWORK_HOST_MIGRATION
	for (THostMigrationEventListenerContainer::iterator it = m_hostMigrationListeners.begin(); it != m_hostMigrationListeners.end(); ++it)
	{
		if (it->second.m_pListener == pListener)
		{
			m_hostMigrationListeners.erase(it);
			return;
		}
	}
#endif
}

CDrxLobby::CMemAllocator::CMemAllocator()
{
#ifdef USE_GLOBAL_BUCKET_ALLOCATOR
	m_bucketAllocator.EnableExpandCleanups(false);
#endif
	m_pGeneralHeap = DrxGetIMemoryUpr()->CreateGeneralExpandingMemoryHeap(LOBBY_GENERAL_HEAP_SIZE, LOBBY_GENERAL_HEAP_SIZE, "DrxLobby General");
	m_totalBucketAllocated = 0;
	m_totalGeneralHeapAllocated = 0;
	m_MaxBucketAllocated = 0;
	m_MaxGeneralHeapAllocated = 0;
}

CDrxLobby::CMemAllocator::~CMemAllocator()
{
	PrintStats();

	if (m_pGeneralHeap)
	{
		m_pGeneralHeap->Release();
	}
}

TMemHdl CDrxLobby::CMemAllocator::MemAlloc(size_t sz)
{
	void* p = NULL;

	if (sz <= m_bucketAllocator.MaxSize)
	{
		p = m_bucketAllocator.allocate(sz);

		if (p == NULL)
		{
			DrxFatalError("Lobby Allocation For %" PRISIZE_T " Failed - Suggest increasing the bucket allocator size!!!!", sz);
		}

		m_totalBucketAllocated += m_bucketAllocator.getSize(p);

		if (m_totalBucketAllocated > m_MaxBucketAllocated)
		{
			m_MaxBucketAllocated = m_totalBucketAllocated;
		}
	}
	else
	{
		p = m_pGeneralHeap->Malloc(sz, "Lobby");

		if (p == NULL)
		{
			DrxFatalError("Lobby Allocation For %" PRISIZE_T " Failed - Suggest increasing the general heap size!!!!", sz);
		}

		m_totalGeneralHeapAllocated += m_pGeneralHeap->UsableSize(p);

		if (m_totalGeneralHeapAllocated > m_MaxGeneralHeapAllocated)
		{
			m_MaxGeneralHeapAllocated = m_totalGeneralHeapAllocated;
		}
	}

	return p;
}

void CDrxLobby::CMemAllocator::MemFree(TMemHdl h)
{
	void* p = MemGetPtr(h);

	if (m_bucketAllocator.IsInAddressRange(p))
	{
		m_totalBucketAllocated -= m_bucketAllocator.getSize(p);
		m_bucketAllocator.deallocate(p);
	}
	else
	{
		m_totalGeneralHeapAllocated -= m_pGeneralHeap->UsableSize(p);
		m_pGeneralHeap->Free(p);
	}
}

void* CDrxLobby::CMemAllocator::MemGetPtr(TMemHdl h)
{
	return h;
}

TMemHdl CDrxLobby::CMemAllocator::MemGetHdl(void* ptr)
{
	return ptr;
}

void CDrxLobby::CMemAllocator::PrintStats()
{
	NetLog("DrxLobby Memory Stats");
	NetLog("Current Bucket Allocated %" PRISIZE_T " Maximum Bucket Allocated %" PRISIZE_T, m_totalBucketAllocated, m_MaxBucketAllocated);
	NetLog("Current General Heap Allocated %" PRISIZE_T " Maximum General Heap Allocated %" PRISIZE_T, m_totalGeneralHeapAllocated, m_MaxGeneralHeapAllocated);
}
