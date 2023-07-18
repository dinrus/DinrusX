// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_LOBBY_H__
#define __DRXPSN2_LOBBY_H__

#pragma once

#if DRX_PLATFORM_ORBIS

	#include "DrxLobby.h"

	#if USE_PSN

		#include "PSNOrbis/DrxPSN2Support.h"
		#include "PSNOrbis/DrxPSN2MatchMaking.h"
		#include "PSNOrbis/DrxPSN2Stats.h"
		#include "PSNOrbis/DrxPSN2LobbyUI.h"
		#include "PSNOrbis/DrxPSN2Friends.h"
		#include "PSNOrbis/DrxPSN2Reward.h"
//#include "Lobby/PSN/DrxPSNVoice.h"

		<DinrusX/np.h>

		#include "DrxTCPServiceFactory.h"

struct SDrxPSNUserID : public SDrxSharedUserID
{
	virtual bool operator==(const SDrxUserID& other) const
	{
		return (sceNpCmpNpId(&npId, &(((SDrxPSNUserID&)other).npId)) == 0);
	}

	virtual bool operator<(const SDrxUserID& other) const
	{
		int order = 0;
		sceNpCmpNpIdInOrder(&npId, &(((SDrxPSNUserID&)other).npId), &order);
		return order < 0;
	}

	virtual DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> GetGUIDAsString() const
	{
		DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> temp;

		temp = DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH>(npId.handle.data);

		return temp;
	}

	SDrxPSNUserID& operator=(const SDrxPSNUserID& other)
	{
		npId = other.npId;
		return *this;
	}

	SceNpId npId;
};

enum DrxPSNLobbyPacketType
{
	ePSNPT_HostMigrationStart = eLobbyPT_EndType,
	ePSNPT_HostMigrationServer,
	ePSNPT_JoinSessionAck,

	ePSNPT_EndType
};

class CDrxPSNLobbyService : public CDrxLobbyService
{
public:
	CDrxPSNLobbyService(CDrxLobby* pLobby, EDrxLobbyService service);

	virtual EDrxLobbyError   Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual EDrxLobbyError   Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual void             Tick(CTimeValue tv);
	virtual IDrxMatchMaking* GetMatchMaking()
	{
		#if USE_DRX_MATCHMAKING
		return m_pMatchmaking;
		#else // USE_DRX_MATCHMAKING
		return NULL;
		#endif //
	}
	virtual IDrxVoice* GetVoice()
	{
		#if USE_PSN_VOICE
		return m_pVoice;
		#else//USE_PSN_VOICE
		return NULL;
		#endif//USE_PSN_VOICE
	}
	virtual IDrxReward* GetReward() { return m_pReward; }
	virtual IDrxStats*  GetStats()
	{
		#if USE_DRX_STATS
		return m_pStats;
		#else // USE_DRX_STATS
		return NULL;
		#endif // USE_DRX_STATS
	}
	virtual IDrxOnlineStorage* GetOnlineStorage() { return NULL; }                        //No implementation yet
	virtual IDrxLobbyUI*       GetLobbyUI()       { return m_pLobbyUI; }
	virtual IDrxFriends*       GetFriends()
	{
		#if USE_DRX_FRIENDS
		return m_pFriends;
		#else // USE_DRX_FRIENDS
		return NULL;
		#endif // USE_DRX_FRIENDS
	}
	virtual IDrxFriendsManagement* GetFriendsManagement() { return NULL; }

	CDrxPSNSupport* const          GetPSNSupport() const  { return m_pLobbySupport; }

	virtual DrxUserID              GetUserID(uint32 user);
	virtual EDrxLobbyError         GetUserPrivileges(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyPrivilegeCallback pCB, void* pCBArg);

	virtual EDrxLobbyError         GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime);

	virtual void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	virtual void                   OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	virtual void                   OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID);

	void                           GetConfigurationInformation(SConfigurationParams* infos, uint32 cnt);

	virtual void                   GetSocketPorts(uint16& connectPort, uint16& listenPort);
	virtual void                   MakeAddrPCCompatible(TNetAddress& addr);

	static void                    SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg);

		#if defined(WARFACE_CONSOLE_VERSION)
			#if defined(GUID_STYLE_DB)
	virtual DrxUserID      GetUserIDFromGUID(PlatformGUID64 xuid);
	virtual DrxUserID      GetUserIDFromGUIDAsString(const char* xuidAsString) { return GetUserIDFromGUID(_atoi64(xuidAsString)); }
	virtual PlatformGUID64 GetGUIDFromUserID(DrxUserID id);
			#endif //GUID_STYLE_DB
		#endif//WARFACE_CONSOLE_VERSION

private:

	void StartTaskRunning(DrxLobbyServiceTaskID lsTaskID);
	void StopTaskRunning(DrxLobbyServiceTaskID lsTaskID);
	void EndTask(DrxLobbyServiceTaskID lsTaskID);

	void StartGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID);
	void TickGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID);

	_smart_ptr<CDrxPSNSupport>     m_pLobbySupport;
		#if USE_DRX_MATCHMAKING
	_smart_ptr<CDrxPSNMatchMaking> m_pMatchmaking;
		#endif // USE_DRX_MATCHMAKING
		#if USE_PSN_VOICE
	//	_smart_ptr<CDrxPSNVoice>				m_pVoice;
		#endif//USE_PSN_VOICE
		#if USE_DRX_STATS
	_smart_ptr<CDrxPSNStats>   m_pStats;
		#endif // USE_DRX_STATS
	_smart_ptr<CDrxPSNLobbyUI> m_pLobbyUI;
		#if USE_DRX_FRIENDS
	_smart_ptr<CDrxPSNFriends> m_pFriends;
		#endif // USE_DRX_FRIENDS
	_smart_ptr<CDrxPSNReward>  m_pReward;

	bool                       m_NPInited;
};

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif//__DRXPSN2_LOBBY_H__
