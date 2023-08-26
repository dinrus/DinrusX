// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamLobby.h
//  Created:     11/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxLobby
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSTEAMLOBBY_H__
#define __DRXSTEAMLOBBY_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxLobby.h"

#if USE_STEAM

////////////////////////////////////////////////////////////////////////////////

	#include "DrxSteamFriends.h"
	#include "DrxSteamFriendsManagement.h"
	#include "DrxSteamMatchMaking.h"
	#include "DrxSteamStats.h"
	#include "DrxSteamReward.h"
	#include "DrxSteamVoice.h"

////////////////////////////////////////////////////////////////////////////////

enum DrxSteamLobbyPacketType
{
	eSteamPT_SessionRequestJoinResult = eLobbyPT_SessionRequestJoinResult,

	eSteamPT_SessionRequestJoin       = DRXONLINELOBBY_PACKET_START,
	eSteamPT_SessionAddRemoteConnections,
	eSteamPT_UserData,
	eSteamPT_VoiceData,

	eSteamPT_EndType
};

////////////////////////////////////////////////////////////////////////////////

class CDrxSteamLobbyService : public CDrxLobbyService
{
public:
	CDrxSteamLobbyService(CDrxLobby* pLobby, EDrxLobbyService service);
	virtual ~CDrxSteamLobbyService(void);

	virtual EDrxLobbyError   Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual EDrxLobbyError   Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual void             Tick(CTimeValue tv);
	virtual IDrxMatchMaking* GetMatchMaking()
	{
	#if USE_DRX_MATCHMAKING
		return m_pMatchmaking;
	#else
		return NULL;
	#endif // USE_DRX_MATCHMAKING
	}
	#if USE_DRX_VOICE && USE_STEAM_VOICE
	virtual IDrxVoice*  GetVoice()  { return m_pVoice; }
	#else
	virtual IDrxVoice*  GetVoice()  { return NULL; }
	#endif // USE_DRX_VOICE
	virtual IDrxReward* GetReward() { return m_pReward; }
	virtual IDrxStats*  GetStats()
	{
	#if USE_DRX_STATS
		return m_pStats;
	#else
		return NULL;
	#endif // USE_DRX_STATS
	}
	virtual IDrxLobbyUI*       GetLobbyUI()       { return NULL; }
	virtual IDrxOnlineStorage* GetOnlineStorage() { return NULL; }
	virtual IDrxFriends*       GetFriends()
	{
	#if USE_DRX_FRIENDS
		return m_pFriends;
	#else
		return NULL;
	#endif // USE_DRX_FRIENDS
	}
	virtual IDrxFriendsManagement* GetFriendsManagement()
	{
	#if USE_DRX_FRIENDS
		return m_pFriendsManagement;
	#else
		return NULL;
	#endif // USE_DRX_FRIENDS
	}
	virtual DrxUserID      GetUserID(uint32 user) { return DrxUserInvalidID; }
	virtual EDrxLobbyError GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime);

	virtual void           OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	virtual void           OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	virtual void           OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID);

	virtual void           GetSocketPorts(uint16& connectPort, uint16& listenPort);

	void                   InviteAccepted(uint32 user, DrxSessionID id);

private:
	#if USE_DRX_MATCHMAKING
	_smart_ptr<CDrxSteamMatchMaking> m_pMatchmaking;
	#endif // USE_DRX_MATCHMAKING
	#if USE_DRX_STATS
	_smart_ptr<CDrxSteamStats> m_pStats;
	#endif // USE_DRX_STATS

	_smart_ptr<CDrxSteamReward> m_pReward;

	#if USE_DRX_VOICE && USE_STEAM_VOICE
	_smart_ptr<CDrxSteamVoice> m_pVoice;
	#endif

	#if USE_DRX_FRIENDS
	_smart_ptr<CDrxSteamFriends>           m_pFriends;
	_smart_ptr<CDrxSteamFriendsManagement> m_pFriendsManagement;
	#endif // USE_DRX_FRIENDS

	#if !defined(RELEASE)
	AppId_t m_SteamAppID;
	#endif // !defined(RELEASE)
};

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM
#endif // __DRXSTEAMLOBBY_H__
