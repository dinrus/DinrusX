// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxDedicatedServer.h"
#include "DrxSharedLobbyPacket.h"

#if USE_DRX_DEDICATED_SERVER

	#define FREE_SELF_TIME                        30000
	#define DEDICATED_SERVER_FIRST_FREE_SEND_TIME 5000

CDrxDedicatedServer::CDrxDedicatedServer(CDrxLobby* pLobby, CDrxLobbyService* pService, EDrxLobbyService serviceType) : CDrxMatchMaking(pLobby, pService, serviceType)
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

	m_allocatedSession = DrxLobbyInvalidSessionHandle;
	m_pLobby = pLobby;
}

void CDrxDedicatedServer::StartArbitratorNameLookup()
{
	CNetAddressResolver* pResolver = CDrxLobby::GetResolver();
	CLobbyCVars& cvars = CLobbyCVars::Get();

	if (pResolver)
	{
		string address;

		if (cvars.pDedicatedServerArbitratorIP && cvars.pDedicatedServerArbitratorIP->GetString())
		{
			if (strlen(cvars.pDedicatedServerArbitratorIP->GetString()))
			{
				address.Format("%s:%d", cvars.pDedicatedServerArbitratorIP->GetString(), cvars.dedicatedServerArbitratorPort);
				NetLog("Starting name resolution for dedicated server arbitrator '%s'", address.c_str());

				m_pNameReq = pResolver->RequestNameLookup(address);
			}
		}
	}
}

EDrxLobbyError CDrxDedicatedServer::Initialise()
{
	EDrxLobbyError error = CDrxMatchMaking::Initialise();

	if (error == eCLE_Success)
	{
		StartArbitratorNameLookup();
	}

	return error;
}

EDrxLobbyError CDrxDedicatedServer::Terminate()
{
	EDrxLobbyError error = CDrxMatchMaking::Terminate();

	if (m_allocatedSession != DrxLobbyInvalidSessionHandle)
	{
		FreeSessionHandle(m_allocatedSession);
		m_allocatedSession = DrxLobbyInvalidSessionHandle;
	}

	return error;
}

uint64 CDrxDedicatedServer::GetSIDFromSessionHandle(DrxLobbySessionHandle h)
{
	DRX_ASSERT_MESSAGE((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED), "CDrxLANMatchMaking::GetSIDFromSessionHandle: invalid session handle");

	return m_sessions[h].sid;
}

TNetAddress CDrxDedicatedServer::GetHostAddressFromSessionHandle(DrxSessionHandle gh)
{
	LOBBY_AUTO_LOCK;

	return TNetAddress(TLocalNetAddress(m_lobby->GetInternalSocketPort(eCLS_LAN)));
}

DrxMatchMakingConnectionID CDrxDedicatedServer::AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, uint32 numUsers)
{
	DrxMatchMakingConnectionID id = CDrxMatchMaking::AddRemoteConnection(h, connectionID, uid, numUsers);

	if (id != DrxMatchMakingInvalidConnectionID)
	{
		m_sessions[h].remoteConnection[id].gameInformedConnectionEstablished = true;
	}

	return id;
}

void CDrxDedicatedServer::TickArbitratorNameLookup(CTimeValue tv)
{
	ENameRequestResult result = m_pNameReq->GetResult();
	CLobbyCVars& cvars = CLobbyCVars::Get();

	switch (result)
	{
	case eNRR_Pending:
		break;

	case eNRR_Succeeded:
		NetLog("Name resolution for dedicated server arbitrator '%s:%d' succeeded", cvars.pDedicatedServerArbitratorIP->GetString(), cvars.dedicatedServerArbitratorPort);
		m_arbitratorAddr = m_pNameReq->GetAddrs()[0];
		m_pNameReq = NULL;
		m_arbitratorNextSendTime.SetMilliSeconds(g_time.GetMilliSecondsAsInt64() + DEDICATED_SERVER_FIRST_FREE_SEND_TIME);
		break;

	case eNRR_Failed:
		NetLog("Name resolution for dedicated server arbitrator '%s:%d' failed", cvars.pDedicatedServerArbitratorIP->GetString(), cvars.dedicatedServerArbitratorPort);
		m_pNameReq = NULL;
		break;
	}
}

void CDrxDedicatedServer::TickFree(CTimeValue tv)
{
	if (tv > m_arbitratorNextSendTime)
	{
		const uint32 MaxBufferSize = DrxLobbyPacketUnReliableHeaderSize;
		uint8 buffer[MaxBufferSize];
		CDrxSharedLobbyPacket packet;

		packet.SetWriteBuffer(buffer, MaxBufferSize);
		packet.StartWrite(eLobbyPT_D2A_DedicatedServerIsFree, false);

		m_pLobby->Send(&packet, m_arbitratorAddr, DrxLobbyInvalidConnectionID, NULL);

		m_arbitratorNextSendTime.SetMilliSeconds(tv.GetMilliSecondsAsInt64() + DEDICATED_SERVER_IS_FREE_SEND_INTERVAL);
	}
}

void CDrxDedicatedServer::TickAllocated(CTimeValue tv)
{
	SSession* pSession = &m_sessions[m_allocatedSession];
	bool haveConnections = false;

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

		if (pConnection->used)
		{
			if (pConnection->connectionID != DrxLobbyInvalidConnectionID)
			{
				EDrxLobbyConnectionState connectionState = m_pLobby->ConnectionGetState(pConnection->connectionID);

				if ((connectionState == eCLCS_Pending) || (connectionState == eCLCS_Connected))
				{
					pSession->checkFreeStartTime = tv;
					haveConnections = true;
					break;
				}
			}
		}
	}

	if (!haveConnections && ((tv - pSession->checkFreeStartTime).GetMilliSecondsAsInt64() > FREE_SELF_TIME))
	{
		FreeDedicatedServer();
	}
}

void CDrxDedicatedServer::Tick(CTimeValue tv)
{
	CDrxMatchMaking::Tick(tv);

	if (m_pNameReq)
	{
		TickArbitratorNameLookup(tv);
	}
	else
	{
		if (m_allocatedSession == DrxLobbyInvalidSessionHandle)
		{
			TickFree(tv);
		}
		else
		{
			TickAllocated(tv);
		}
	}
}

void CDrxDedicatedServer::ProcessDedicatedServerIsFreeAck(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
}

void CDrxDedicatedServer::DispatchUserPacket(TMemHdl mh, uint32 length)
{
	CDrxMatchMaking::DispatchUserPacket(mh, length, DrxSessionInvalidHandle);
}

void CDrxDedicatedServer::ProcessAllocateDedicatedServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	pPacket->StartRead();

	SDrxMatchMakingConnectionUID uid = pPacket->ReadConnectionUID();
	uint32 clientIP = pPacket->ReadUINT32();
	uint16 clientPort = pPacket->ReadUINT16();
	uint32 clientCookie = pPacket->ReadUINT32();
	TNetAddress clientAddr(SIPv4Addr(clientIP, clientPort));

	uint8 userPacketType = pPacket->ReadUINT8();
	uint16 userDataSize = pPacket->ReadUINT16();
	uint32 writeUserPacketSize = DrxLobbyPacketUnReliableHeaderSize + userDataSize;
	CDrxLobbyPacket userPacket;
	bool acceptRequest = false;

	if (m_allocatedSession == DrxLobbyInvalidSessionHandle)
	{
		// Currently not allocated so accept request, create a session and send the user packet to game.
		if (userPacket.CreateWriteBuffer(writeUserPacketSize))
		{
			uint32 createFlags = DRXSESSION_CREATE_FLAG_NUB;
	#if NETWORK_HOST_MIGRATION
			createFlags |= DRXSESSION_CREATE_FLAG_MIGRATABLE;
	#endif
			if (CreateSessionHandle(&m_allocatedSession, true, createFlags, 0) == eCLE_Success)
			{
				SSession* pSession = &m_sessions[m_allocatedSession];

				pSession->localConnection.uid = uid;

				pSession->sid = uid.m_sid;
				pSession->allocToAddr = clientAddr;
				pSession->cookie = clientCookie;
				pSession->checkFreeStartTime = g_time;

				NetLog("Received allocate server from arbitrator " PRFORMAT_ADDR " for client " PRFORMAT_ADDR " cookie %x informing arbitrator accepted", PRARG_ADDR(addr), PRARG_ADDR(clientAddr), clientCookie);

				acceptRequest = true;
				userPacket.StartWrite(userPacketType, false);

				if (userDataSize > 0)
				{
					userPacket.WriteData(pPacket->GetReadBuffer() + pPacket->GetReadBufferPos(), userDataSize);
				}

				uint32 length;
				TMemHdl mh = PacketToMemoryBlock(&userPacket, length);
				TO_GAME_FROM_LOBBY(&CDrxDedicatedServer::DispatchUserPacket, this, mh, length);
			}

			userPacket.FreeWriteBuffer();
		}
	}
	else
	{
		// Currently allocated so if this request is for our current allocation send accept else send refuse.
		SSession* pSession = &m_sessions[m_allocatedSession];

		if ((pSession->sid == uid.m_sid) && (pSession->allocToAddr == clientAddr) && (pSession->cookie == clientCookie))
		{
			NetLog("Received allocate server from arbitrator " PRFORMAT_ADDR " for client " PRFORMAT_ADDR " cookie %x informing arbitrator accepted", PRARG_ADDR(addr), PRARG_ADDR(clientAddr), clientCookie);
			pSession->checkFreeStartTime = g_time;
			acceptRequest = true;
		}
		else
		{
			NetLog("Received allocate server from arbitrator " PRFORMAT_ADDR " for client " PRFORMAT_ADDR " cookie %x but already allocated to client " PRFORMAT_ADDR " cookie %x informing arbitrator not accepted", PRARG_ADDR(addr), PRARG_ADDR(clientAddr), clientCookie, PRARG_ADDR(pSession->allocToAddr), pSession->cookie);
		}
	}

	const uint32 MaxBufferSize = DrxLobbyPacketUnReliableHeaderSize + DrxLobbyPacketBoolSize + DrxLobbyPacketConnectionUIDSize + DrxLobbyPacketUINT32Size + DrxLobbyPacketUINT16Size + DrxLobbyPacketUINT32Size;
	uint8 buffer[MaxBufferSize];
	CDrxSharedLobbyPacket packet;

	packet.SetWriteBuffer(buffer, MaxBufferSize);
	packet.StartWrite(eLobbyPT_D2A_AllocateDedicatedServerAck, false);
	packet.WriteBool(acceptRequest);
	packet.WriteConnectionUID(uid);
	packet.WriteUINT32(clientIP);
	packet.WriteUINT16(clientPort);
	packet.WriteUINT32(clientCookie);

	m_pLobby->Send(&packet, addr, DrxLobbyInvalidConnectionID, NULL);
}

void CDrxDedicatedServer::DispatchFreeDedicatedServer()
{
	UDrxLobbyEventData eventData;
	memset(&eventData, 0, sizeof(eventData));
	m_pLobby->DispatchEvent(eCLSE_DedicatedServerRelease, eventData);
}

void CDrxDedicatedServer::FreeDedicatedServer()
{
	SSession* pSession = &m_sessions[m_allocatedSession];

	// Disconnect our local connection
	SessionDisconnectRemoteConnectionViaNub(m_allocatedSession, DrxMatchMakingInvalidConnectionID, eDS_Local, DrxMatchMakingInvalidConnectionID, eDC_UserRequested, "Session deleted");

	// Free any remaining remote connections
	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

		if (pConnection->used)
		{
			FreeRemoteConnection(m_allocatedSession, i);
		}
	}

	FreeSessionHandle(m_allocatedSession);
	m_allocatedSession = DrxLobbyInvalidSessionHandle;
	m_arbitratorNextSendTime.SetMilliSeconds(g_time.GetMilliSecondsAsInt64() + DEDICATED_SERVER_FIRST_FREE_SEND_TIME);
	TO_GAME_FROM_LOBBY(&CDrxDedicatedServer::DispatchFreeDedicatedServer, this);
}

void CDrxDedicatedServer::ProcessFreeDedicatedServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	if (m_allocatedSession != DrxLobbyInvalidSessionHandle)
	{
		FreeDedicatedServer();
	}
}

void CDrxDedicatedServer::ProcessSessionRequestJoinServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	SDrxMatchMakingConnectionUID uid;

	pPacket->StartRead();
	uid = pPacket->GetFromConnectionUID();

	if (m_allocatedSession != DrxLobbyInvalidSessionHandle)
	{
		DrxLobbyConnectionID c;

		if (m_pLobby->ConnectionFromAddress(&c, addr))
		{
			SSession* pSession = &m_sessions[m_allocatedSession];

			if (pSession->sid == uid.m_sid)
			{
				DrxMatchMakingConnectionID id = AddRemoteConnection(m_allocatedSession, c, uid, 0);

				if (id != DrxMatchMakingInvalidConnectionID)
				{
					bool host = pPacket->ReadBool();

					if (host)
					{
						pSession->hostConnectionID = id;
					}

					// Send back accept
					const uint32 MaxBufferSize = DrxLobbyPacketReliableHeaderSize + DrxLobbyPacketBoolSize + DrxLobbyPacketConnectionUIDSize;
					uint8 buffer[MaxBufferSize];
					CDrxSharedLobbyPacket packet;

					packet.SetWriteBuffer(buffer, MaxBufferSize);
					packet.StartWrite(eLobbyPT_SessionRequestJoinServerResult, true);
					packet.WriteBool(true);
					packet.WriteConnectionUID(uid);

					Send(DrxMatchMakingInvalidTaskID, &packet, m_allocatedSession, id);

					return;
				}
				else
				{
					NetLog("refusing join request from " PRFORMAT_ADDR " failed to add connection", PRARG_ADDR(addr));
				}
			}
			else
			{
				NetLog("refusing join request from " PRFORMAT_ADDR " session sid %llx does not match connection sid %llx", PRARG_ADDR(addr), pSession->sid, uid.m_sid);
			}
		}
		else
		{
			NetLog("refusing join request from " PRFORMAT_ADDR " failed to get connection from address", PRARG_ADDR(addr));
		}
	}
	else
	{
		NetLog("refusing join request from " PRFORMAT_ADDR " we're not in a session", PRARG_ADDR(addr));
	}

	// Send back fail
	const uint32 MaxBufferSize = DrxLobbyPacketReliableHeaderSize + DrxLobbyPacketBoolSize + DrxLobbyPacketConnectionUIDSize;
	uint8 buffer[MaxBufferSize];
	CDrxSharedLobbyPacket packet;

	packet.SetWriteBuffer(buffer, MaxBufferSize);
	packet.StartWrite(eLobbyPT_SessionRequestJoinServerResult, true);
	packet.WriteBool(false);
	packet.WriteConnectionUID(uid);

	Send(DrxMatchMakingInvalidTaskID, &packet, m_allocatedSession, addr);
}

void CDrxDedicatedServer::ProcessUpdateSessionID(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	if (m_allocatedSession != DrxLobbyInvalidSessionHandle)
	{
		SDrxMatchMakingConnectionUID fromUID = pPacket->GetFromConnectionUID();
		DrxMatchMakingConnectionID connID = DrxMatchMakingInvalidConnectionID;

		if (FindConnectionFromSessionUID(m_allocatedSession, fromUID, &connID))
		{
			SSession* pSession = &m_sessions[m_allocatedSession];
			if (pSession->hostConnectionID == connID)
			{
				pPacket->StartRead();
				SDrxMatchMakingConnectionUID newUID = pPacket->ReadConnectionUID();

				// update sid on session and connections
				uint64 newSID = newUID.m_sid;

				pSession->sid = newSID;
				pSession->localConnection.uid.m_sid = newSID;
	#if NETWORK_HOST_MIGRATION
				pSession->newHostUID.m_sid = newSID;
	#endif

				for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
				{
					SSession::SRConnection* pConnection = &pSession->remoteConnection[i];

					if (pConnection->used)
					{
						pConnection->uid.m_sid = newSID;
					}
				}
			}
		}
	}
}

	#if NETWORK_HOST_MIGRATION
void CDrxDedicatedServer::HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause)
{
	NetLog("HostMigrationInitialise " PRFORMAT_SH " cause %d", PRARG_SH(h), cause);

	if (m_allocatedSession != DrxLobbyInvalidSessionHandle)
	{
		SSession* pSession = &m_sessions[m_allocatedSession];
		DrxMatchMakingConnectionID connID = DrxMatchMakingInvalidConnectionID;

		for (int i = 0; i < pSession->newHostPriorityCount; ++i)
		{
			CDrxMatchMaking::SSession::SRConnection* pConnection = pSession->newHostPriority[i];
			for (uint32 connectionIndex = 0; connectionIndex < MAX_LOBBY_CONNECTIONS; ++connectionIndex)
			{
				CDrxMatchMaking::SSession::SRConnection* pRemoteConnection = &pSession->remoteConnection[connectionIndex];
				if (pConnection == pRemoteConnection)
				{
					if (connectionIndex != pSession->hostConnectionID)
					{
						connID = connectionIndex;
					}
					break;
				}
			}

			if (connID != DrxMatchMakingInvalidConnectionID)
			{
				// found a valid id, stop searching
				break;
			}
		}

		if (connID != DrxMatchMakingInvalidConnectionID)
		{
			// set as new host
			pSession->hostConnectionID = connID;

			// tells clients who new host should be
			const uint32 maxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketConnectionUIDSize;
			uint8 buffer[maxBufferSize];
			CDrxLobbyPacket packet;

			packet.SetWriteBuffer(buffer, maxBufferSize);
			packet.StartWrite(eLobbyPT_D2C_HostMigrationStart, true);
			packet.WriteConnectionUID(pSession->remoteConnection[connID].uid);

			SendToAll(DrxMatchMakingInvalidTaskID, &packet, m_allocatedSession, DrxMatchMakingInvalidConnectionID);

			NetLog("[Host Migration]: selected client " PRFORMAT_SHMMCINFO " as new host", PRARG_SHMMCINFO(m_allocatedSession, connID, pSession->remoteConnection[connID].connectionID, pSession->remoteConnection[connID].uid));
		}
		else
		{
			FreeDedicatedServer();
		}
	}
	else
	{
		NetLog("[Host Migration]: trying to initialise host migration but we don't have a valid session");
	}
}
	#endif // NETWORK_HOST_MIGRATION

void CDrxDedicatedServer::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;

	if (addr == m_arbitratorAddr)
	{
		m_arbitratorLastRecvTime = g_time;
	}

	switch (pSPacket->StartRead())
	{
	case eLobbyPT_A2D_DedicatedServerIsFreeAck:
		ProcessDedicatedServerIsFreeAck(addr, pSPacket);
		break;

	case eLobbyPT_A2D_AllocateDedicatedServer:
		ProcessAllocateDedicatedServer(addr, pSPacket);
		break;

	case eLobbyPT_A2D_FreeDedicatedServer:
		ProcessFreeDedicatedServer(addr, pSPacket);
		break;

	case eLobbyPT_SessionRequestJoinServer:
		ProcessSessionRequestJoinServer(addr, pSPacket);
		break;

	case eLobbyPT_C2D_UpdateSessionID:
		ProcessUpdateSessionID(addr, pSPacket);
		break;

	default:
		CDrxMatchMaking::OnPacket(addr, pSPacket);
		break;
	}
}

void CDrxDedicatedServer::SSession::Reset()
{
	PARENT::Reset();

	//	uint64												sid;
	//	TNetAddress										allocToAddr;
	//	CTimeValue										checkFreeStartTime;
	//	uint32												cookie;
	//	SLConnection									localConnection;
	//	DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS>	remoteConnection;
}

#endif // USE_DRX_DEDICATED_SERVER
