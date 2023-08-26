// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamFriendsManagement.h
//  Created:     26/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxFriends
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSTEAMFRIENDSMANAGEMENT_H__
#define __DRXSTEAMFRIENDSMANAGEMENT_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxSteamMatchMaking.h"

#if USE_STEAM && USE_DRX_FRIENDS

	#include "DrxFriendsManagement.h"

////////////////////////////////////////////////////////////////////////////////

class CDrxSteamFriendsManagement : public CDrxFriendsManagement
{
public:
	CDrxSteamFriendsManagement(CDrxLobby* pLobby, CDrxLobbyService* pService);

	// IDrxFriendsManagment
	virtual EDrxLobbyError FriendsManagementSendFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementAcceptFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementRejectFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementRevokeFriendStatus(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementIsUserFriend(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementInfoCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementFindUser(uint32 user, SFriendManagementSearchInfo* pUserName, uint32 maxResults, DrxLobbyTaskID* pTaskID, DrxFriendsManagementSearchCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementBlockUser(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementUnblockUser(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementIsUserBlocked(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementInfoCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementAcceptInvite(uint32 user, DrxUserID* pUserID, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementGetName(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementSearchCallback pCb, void* pCbArg);
	virtual EDrxLobbyError FriendsManagementGetStatus(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementStatusCallback pCb, void* pCbArg);
	// ~IDrxFriendsManagment

	void Tick(CTimeValue tv);

private:
	enum ETask
	{
		eT_FriendsManagementIsUserFriend,
		eT_FriendsManagementIsUserBlocked,
		eT_FriendsManagementAcceptInvite,
		eT_FriendsManagementGetName,
		eT_FriendsManagementGetStatus
	};

	//////////////////////////////////////////////////////////////////////////////

	struct  STask : public CDrxFriendsManagement::STask
	{
		CTimeValue m_timeStarted;
	};

	//////////////////////////////////////////////////////////////////////////////

	EDrxLobbyError InitialiseTask(ETask task, uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, void* pCb, void* pCbArg);
	EDrxLobbyError StartTask(ETask task, bool startRunning, uint32 user, DrxFriendsManagementTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg);
	void           StartTaskRunning(DrxFriendsManagementTaskID fTaskID);
	void           EndTask(DrxFriendsManagementTaskID fTaskID);
	void           StopTaskRunning(DrxFriendsManagementTaskID fTaskID);

	//////////////////////////////////////////////////////////////////////////////

	void StartFriendsManagementIsUserFriend(DrxFriendsManagementTaskID fTaskID);
	void EndFriendsManagementIsUserFriend(DrxFriendsManagementTaskID fTaskID);

	void StartFriendsManagementIsUserBlocked(DrxFriendsManagementTaskID fTaskID);
	void EndFriendsManagementIsUserBlocked(DrxFriendsManagementTaskID fTaskID);

	void StartFriendsManagementAcceptInvite(DrxFriendsManagementTaskID fTaskID);

	void StartFriendsManagementGetName(DrxFriendsManagementTaskID fTaskID);
	void EndFriendsManagementGetName(DrxFriendsManagementTaskID fTaskID);

	void StartFriendsManagementGetStatus(DrxFriendsManagementTaskID fTaskID);
	void EndFriendsManagementGetStatus(DrxFriendsManagementTaskID fTaskID);

	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Steam callbacks are broadcast to all listeners
	//////////////////////////////////////////////////////////////////////////////

	STask m_task[MAX_FRIENDS_MANAGEMENT_TASKS];
};

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_FRIENDS
#endif // __DRXSTEAMFRIENDSMANAGEMENT_H__
