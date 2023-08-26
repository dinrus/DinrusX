// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXLANLOBBY_H__
#define __DRXLANLOBBY_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxLobby.h"
#include "DrxTCPServiceFactory.h"
#include "LAN/DrxLANMatchMaking.h"
#include "DrxReward.h"

#if USE_LAN

class CDrxDedicatedServerArbitrator;
class CDrxDedicatedServer;

enum DrxLANLobbyPacketType
{
	eLANPT_SessionRequestJoinResult = eLobbyPT_SessionRequestJoinResult,

	eLANPT_MM_RequestServerData     = DRXLANLOBBY_PACKET_START,
	eLANPT_MM_ServerData,
	eLANPT_SessionRequestJoin,
	eLANPT_SessionAddRemoteConnections,
	eLANPT_HostMigrationStart,
	eLANPT_HostMigrationServer,
	eLANPT_HostMigrationClient,
	eLANPT_UserData,

	eLANPT_EndType
};

struct SDrxLANUserID : public SDrxSharedUserID
{
	virtual bool operator==(const SDrxUserID& other) const
	{
		return id == ((SDrxLANUserID&)other).id;
	}

	virtual bool operator<(const SDrxUserID& other) const
	{
		return id < ((SDrxLANUserID&)other).id;
	}

	virtual DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> GetGUIDAsString() const
	{
		DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> temp;

		temp.Format("%016llx", id);

		return temp;
	}

	SDrxLANUserID& operator=(const SDrxLANUserID& other)
	{
		id = other.id;
		return *this;
	}

	uint64 id;
};

class CDrxLANLobbyService : public CDrxLobbyService
{
public:
	CDrxLANLobbyService(CDrxLobby* pLobby, EDrxLobbyService service);

	virtual EDrxLobbyError         Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual EDrxLobbyError         Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual void                   Tick(CTimeValue tv);
	virtual IDrxMatchMaking*       GetMatchMaking()       { return m_pMatchmaking; }
	virtual IDrxVoice*             GetVoice()             { return NULL; }
	virtual IDrxReward*            GetReward()            { return NULL; }
	virtual IDrxStats*             GetStats()             { return NULL; }
	virtual IDrxLobbyUI*           GetLobbyUI()           { return NULL; }
	virtual IDrxOnlineStorage*     GetOnlineStorage()     { return NULL; }
	virtual IDrxFriends*           GetFriends()           { return NULL; }
	virtual IDrxFriendsManagement* GetFriendsManagement() { return NULL; }
	virtual DrxUserID              GetUserID(uint32 user) { return DrxUserInvalidID; }
	virtual EDrxLobbyError         GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime);

	virtual void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	virtual void                   OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	virtual void                   OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID);

	virtual void                   GetSocketPorts(uint16& connectPort, uint16& listenPort);

	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	CDrxDedicatedServerArbitrator* GetDedicatedServerArbitrator() { return m_pDedicatedServerArbitrator; }
	#endif
	#if USE_DRX_DEDICATED_SERVER
	CDrxDedicatedServer* GetDedicatedServer() { return m_pDedicatedServer; }
	#endif
private:
	_smart_ptr<CDrxLANMatchMaking> m_pMatchmaking;
	#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	CDrxDedicatedServerArbitrator* m_pDedicatedServerArbitrator;
	#endif
	#if USE_DRX_DEDICATED_SERVER
	_smart_ptr<CDrxDedicatedServer> m_pDedicatedServer;
	#endif
};

#endif // USE_LAN
#endif // __DRXLANLOBBY_H__
