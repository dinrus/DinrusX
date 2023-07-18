// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXDEDICATEDSERVERARBITRATOR_H__
#define __DRXDEDICATEDSERVERARBITRATOR_H__

#pragma once

#include "DrxLobby.h"

#if USE_DRX_DEDICATED_SERVER_ARBITRATOR

class CDrxSharedLobbyPacket;

class CDrxDedicatedServerArbitrator
{
public:
	CDrxDedicatedServerArbitrator(CDrxLobby* pLobby, CDrxLobbyService* pService);

	EDrxLobbyError Initialise();
	EDrxLobbyError Terminate();
	void           Tick(CTimeValue tv);
	void           OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	void           OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	void           ListFreeServers();

protected:
	// There are 2 addresses for a dedicated server
	// serverPublicAddr is the address used by the external clients
	// serverPrivateAddr is the address used by the arbitrator
	// These addresses may or may not be the same depending on setup.

	struct SFreeServerData
	{
		TNetAddress serverPublicAddr;
		CTimeValue  lastRecvTime;
	};

	struct SAllocatedServerData
	{
		CTimeValue  allocTime;
		TNetAddress serverPublicAddr;
		TNetAddress serverPrivateAddr;
		uint32      clientCookie;
		uint8       clientTaskID;
		bool        serverReady;
	};

	struct SFreeingServerData
	{
		CTimeValue lastSendTime;
		uint32     sendCount;
	};

	typedef std::map<TNetAddress, SFreeServerData>    TFreeServersMap;          // TNetAddress is the dedicated servers private address
	typedef std::map<uint64, SAllocatedServerData>    TAllocatedServersMap;
	typedef std::map<TNetAddress, SFreeingServerData> TFreeingServersMap;       // TNetAddress is the dedicated servers private address

	TFreeServersMap::iterator      AddToFreeServers(const TNetAddress& serverPrivateAddr, const TNetAddress& serverPublicAddr);
	TAllocatedServersMap::iterator AddToAllocatedServers(uint64 sid, const TNetAddress& serverPrivateAddr, const TNetAddress& serverPublicAddr, uint32 clientCookie, uint8 clientTaskID);
	TFreeingServersMap::iterator   AddToFreeingServers(const TNetAddress& serverPrivateAddr);
	void                           ProcessDedicatedServerIsFree(const TNetAddress& serverPrivateAddr, CDrxSharedLobbyPacket* pPacket);
	void                           SendRequestSetupDedicatedServerResult(bool success, uint32 clientTaskID, uint32 clientCookie, const TNetAddress& clientAddr, const TNetAddress& serverPublicAddr, SDrxMatchMakingConnectionUID serverUID);
	void                           ProcessRequestSetupDedicatedServer(const TNetAddress& clientAddr, CDrxSharedLobbyPacket* pPacket);
	void                           ProcessAllocateDedicatedServerAck(const TNetAddress& serverPrivateAddr, CDrxSharedLobbyPacket* pPacket);
	void                           ProcessRequestReleaseDedicatedServer(const TNetAddress& clientAddr, CDrxSharedLobbyPacket* pPacket);

	TFreeServersMap      m_freeServers;
	TAllocatedServersMap m_allocatedServers;
	TFreeingServersMap   m_freeingServers;
	CDrxLobby*           m_pLobby;
	CDrxLobbyService*    m_pService;
};

#endif // USE_DRX_DEDICATED_SERVER_ARBITRATOR
#endif // __DRXDEDICATEDSERVERARBITRATOR_H__
