// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXDURANGOLIVELOBBY_H__
#define __DRXDURANGOLIVELOBBY_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxLobby.h"

#if USE_DURANGOLIVE

	#pragma warning(push)
	#pragma warning(disable:28204)
	<DinrusX/wrl.h>
	#pragma warning(pop)
	<DinrusX/robuffer.h>

	#include "DrxDurangoLiveMatchMaking.h"
	#include "DrxDurangoLiveOnlineStorage.h"
	#include "DrxDurangoLiveLobbyUI.h"

	#pragma warning(push)
	#pragma warning(disable:6103)

enum DrxDurangoLiveLobbyPacketType
{
	eDurangoLivePT_SessionRequestJoinResult = eLobbyPT_SessionRequestJoinResult,

	eDurangoLivePT_SessionRequestJoin       = DRXONLINELOBBY_PACKET_START,
	eDurangoLivePT_Chat,      // XXX - ATG

	eDurangoLivePT_EndType
};

struct SDrxDurangoLiveUserID : public SDrxSharedUserID
{
	SDrxDurangoLiveUserID(Live::Xuid num)
	{
		xuid = num;
	}

	virtual bool operator==(const SDrxUserID& other) const
	{
		return xuid == ((SDrxDurangoLiveUserID&)other).xuid;
	}

	virtual bool operator<(const SDrxUserID& other) const
	{
		return xuid < ((SDrxDurangoLiveUserID&)other).xuid;
	}

	virtual DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> GetGUIDAsString() const
	{
		DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> temp;

		temp.Format("%" PRId64, xuid);

		return temp;
	}

	SDrxDurangoLiveUserID& operator=(const SDrxDurangoLiveUserID& other)
	{
		xuid = other.xuid;
		return *this;
	}

	Live::Xuid xuid;
};

class CDrxDurangoLiveLobbyService : public CDrxLobbyService
{
public:
	CDrxDurangoLiveLobbyService(CDrxLobby* pLobby, EDrxLobbyService service);
	virtual ~CDrxDurangoLiveLobbyService();

	virtual EDrxLobbyError   Initialise(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual EDrxLobbyError   Terminate(EDrxLobbyServiceFeatures features, DrxLobbyServiceCallback pCB);
	virtual void             Tick(CTimeValue tv);
	virtual IDrxMatchMaking* GetMatchMaking()
	{
	#if USE_DRX_MATCHMAKING
		return m_pMatchmaking;
	#else // USE_DRX_MATCHMAKING
		return NULL;
	#endif // USE_DRX_MATCHMAKING
	}
	virtual IDrxVoice*         GetVoice()  { return NULL; }
	virtual IDrxReward*        GetReward() { return NULL; }
	virtual IDrxStats*         GetStats()  { return NULL; }
	virtual IDrxOnlineStorage* GetOnlineStorage()
	{
	#if USE_DRX_ONLINE_STORAGE
		return m_pOnlineStorage;
	#else
		return NULL;
	#endif // USE_DRX_ONLINE_STORAGE
	}
	virtual IDrxLobbyUI*           GetLobbyUI()           { return m_pLobbyUI; }
	virtual IDrxFriends*           GetFriends()           { return NULL; }
	virtual IDrxFriendsManagement* GetFriendsManagement() { return NULL; }
	virtual DrxUserID              GetUserID(uint32 user);
	virtual EDrxLobbyError         GetUserPrivileges(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyPrivilegeCallback pCB, void* pCBArg);
	virtual EDrxLobbyError         GetSystemTime(uint32 user, SDrxSystemTime* pSystemTime);

	virtual void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	virtual void                   OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	virtual void                   OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID);
	void                           GetSocketPorts(uint16& connectPort, uint16& listenPort);

	DWORD                          GetTitleID() const { return m_titleID; }
	HRESULT                        GetServiceConfigID(HSTRING* scid) const
	{
		if (!scid)
			return E_INVALIDARG;

		return WindowsDuplicateString(m_serviceConfigId.Get(), scid);
	}
private:
	void StartTaskRunning(DrxLobbyServiceTaskID lsTaskID);
	void StopTaskRunning(DrxLobbyServiceTaskID lsTaskID);
	void EndTask(DrxLobbyServiceTaskID lsTaskID);

	void StartGetUserPrivileges(DrxLobbyServiceTaskID lsTaskID);

	DWORD                             m_titleID;
	Microsoft::WRL::Wrappers::HString m_serviceConfigId;

	#if USE_DRX_MATCHMAKING
	_smart_ptr<CDrxDurangoLiveMatchMaking> m_pMatchmaking;
	#endif // USE_DRX_MATCHMAKING
	_smart_ptr<CDrxDurangoLiveLobbyUI>     m_pLobbyUI;

	#if USE_DRX_ONLINE_STORAGE
	_smart_ptr<CDrxDurangoLiveOnlineStorage> m_pOnlineStorage;
	#endif //USE_DRX_ONLINE_STORAGE
};

extern EDrxLobbyError DrxDurangoLiveLobbyGetErrorFromDurangoLive(HRESULT error);

	#pragma warning(pop)

#endif // USE_DURANGOLIVE
#endif // __DRXDURANGOLIVELOBBY_H__
