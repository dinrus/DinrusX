// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamFriends.h
//  Created:     26/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxFriends
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSTEAMFRIENDS_H__
#define __DRXSTEAMFRIENDS_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxSteamMatchMaking.h"

#if USE_STEAM && USE_DRX_FRIENDS

	#include "DrxFriends.h"

////////////////////////////////////////////////////////////////////////////////

class CDrxSteamFriends : public CDrxFriends
{
public:
	CDrxSteamFriends(CDrxLobby* pLobby, CDrxLobbyService* pService);

	// IDrxFriends
	virtual EDrxLobbyError FriendsGetFriendsList(uint32 user, uint32 start, uint32 num, DrxLobbyTaskID* pTaskID, DrxFriendsGetFriendsListCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsSendGameInvite(uint32 user, DrxSessionHandle h, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsCallback pCb, void* pCbArg);
	// ~IDrxFriends

	void Tick(CTimeValue tv);

private:
	enum ETask
	{
		eT_FriendsGetFriendsList,
		eT_FriendsSendGameInvite
	};

	//////////////////////////////////////////////////////////////////////////////

	struct  STask : public CDrxFriends::STask
	{
		CTimeValue m_timeStarted;
	};

	//////////////////////////////////////////////////////////////////////////////

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxFriendsTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartTaskRunning(DrxFriendsTaskID fTaskID);
	void           EndTask(DrxFriendsTaskID fTaskID);
	void           StopTaskRunning(DrxFriendsTaskID fTaskID);

	//////////////////////////////////////////////////////////////////////////////

	void StartFriendsGetFriendsList(DrxFriendsTaskID fTaskID);
	void TickFriendsGetFriendsList(DrxFriendsTaskID fTaskID);
	void EndFriendsGetFriendsList(DrxFriendsTaskID fTaskID);

	void StartFriendsSendGameInvite(DrxFriendsTaskID fTaskID);

	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Steam callbacks are broadcast to all listeners
	DRXSTEAM_CALLBACK(CDrxSteamFriends, OnFriendRichPresenceUpdate, FriendRichPresenceUpdate_t, m_SteamOnFriendRichPresenceUpdate);
	DRXSTEAM_CALLBACK(CDrxSteamFriends, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, m_SteamOnGameLobbyJoinRequested);
	//////////////////////////////////////////////////////////////////////////////

	STask m_task[MAX_FRIENDS_TASKS];
};

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_FRIENDS
#endif // __DRXSTEAMFRIENDS_H__
