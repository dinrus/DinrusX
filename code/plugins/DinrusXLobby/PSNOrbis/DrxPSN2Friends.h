// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSNFRIENDS_H__
#define __DRXPSNFRIENDS_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		#include "DrxFriends.h"
		#include "DrxPSN2Support.h"

class CDrxPSNFriends : public CDrxFriends
{
public:
	CDrxPSNFriends(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport);

	virtual EDrxLobbyError FriendsGetFriendsList(uint32 user, uint32 start, uint32 num, DrxLobbyTaskID* pTaskID, DrxFriendsGetFriendsListCallback cb, void* pCbArg);
	virtual EDrxLobbyError FriendsSendGameInvite(uint32 user, DrxSessionHandle h, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsCallback cb, void* pCbArg);

	static void            SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg);
	static void            LobbyUIGameInviteCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);

private:
	enum ETask
	{
		eT_FriendsGetFriendsList,
		eST_WaitingForFriendsListWebApiCallback,
		eT_FriendsSendGameInvite
	};

	void           ProcessWebApiEvent(SDrxPSNSupportCallbackEventData& data);

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxFriendsTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartSubTask(ETask etask, DrxFriendsTaskID fTaskID);
	void           StartTaskRunning(DrxFriendsTaskID fTaskID);
	void           EndTask(DrxFriendsTaskID fTaskID);
	void           StopTaskRunning(DrxFriendsTaskID fTaskID);

	void           StartFriendsGetFriendsList(DrxFriendsTaskID fTaskID);
	void           EventWebApiGetFriends(DrxFriendsTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);
	void           EndFriendsGetFriendsList(DrxFriendsTaskID fTaskID);

	void           StartFriendsSendGameInvite(DrxFriendsTaskID fTaskID);

	STask           m_task[MAX_FRIENDS_TASKS];

	CDrxPSNSupport* m_pPSNSupport;
};

	#endif // USE_DRX_FRIENDS
#endif   // USE_PSN

#endif // __DRXPSNFRIENDS_H__
