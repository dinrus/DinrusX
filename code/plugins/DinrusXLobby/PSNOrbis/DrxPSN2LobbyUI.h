// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_LOBBYUI_H__
#define __DRXPSN2_LOBBYUI_H__
#pragma once

#include "DrxLobbyUI.h"

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		#define PSN_INVALID_COMMERCE_CONTEXT    0
		#define PSN_INVALID_COMMERCE_REQUEST_ID 0

class CDrxPSNLobbyUI : public CDrxLobbyUI
{
public:
	CDrxPSNLobbyUI(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport);

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();
	void                   Tick(CTimeValue tv);

	virtual EDrxLobbyError ShowGamerCard(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowGameInvite(uint32 user, DrxSessionHandle h, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowFriends(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowFriendRequest(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError SetRichPresence(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowOnlineRetailBrowser(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError CheckOnlineRetailStatus(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIOnlineRetailStatusCallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowMessageList(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual void           PostLocalizationChecks();
	virtual EDrxLobbyError AddRecentPlayers(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, const char* gameModeStr, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError AddActivityFeed(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);

	virtual EDrxLobbyError GetConsumableOffers(uint32 user, DrxLobbyTaskID* pTaskID, const TStoreOfferID* pOfferIDs, uint32 offerIdCount, DrxLobbyUIGetConsumableOffersCallback cb, void* pCbArg);
	virtual EDrxLobbyError ShowDownloadOffer(uint32 user, TStoreOfferID offerId, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);
	virtual EDrxLobbyError GetConsumableAssets(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIGetConsumableAssetsCallback cb, void* pCbArg);
	virtual EDrxLobbyError ConsumeAsset(uint32 user, TStoreAssetID assetID, uint32 quantity, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);

	virtual bool           IsDead() const { return false; }

	static void            SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg);

protected:
	enum ETask
	{
		eT_ShowGamerCard,
		eT_ShowGameInvite,
		eT_ShowFriends,
		eT_ShowFriendRequest,
		eT_SetRichPresence,
		eST_WaitingForRichPresenceCallback,
		eT_ShowOnlineRetailBrowser,
		eT_CheckOnlineRetailStatus,
		eT_JoinFriendsGame,
		eT_ShowMessageList,
		eT_AddRecentPlayers,
		eST_WaitingForAddRecentPlayersCallback,
		eT_AddActivityFeed,
		eST_WaitingForAddActivityFeedCallback,
		eT_GetConsumableOffers,
		eST_WaitingForConsumableOffersCallback,
		eT_GetConsumableAssets,
		eST_WaitingForConsumableAssetsCallback,
		eT_ConsumeAsset,
		eST_WaitingForConsumeAssetCallback,
		eT_ShowDownloadOffer,
	};

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxLobbyUITaskID* pUITaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartSubTask(ETask etask, DrxLobbyUITaskID sTaskID);
	void           StartTaskRunning(DrxLobbyUITaskID uiTaskID);
	void           EndTask(DrxLobbyUITaskID uiTaskID);
	void           StopTaskRunning(DrxLobbyUITaskID uiTaskID);

	void           StartShowGamerCard(DrxLobbyUITaskID uiTaskID);
	void           TickShowGamerCard(DrxLobbyUITaskID uiTaskID);
	void           EndShowGamerCard(DrxLobbyUITaskID uiTaskID);

	void           StartShowGameInvite(DrxLobbyUITaskID uiTaskID);
	void           TickShowGameInvite(DrxLobbyUITaskID uiTaskID);
	void           EndShowGameInvite(DrxLobbyUITaskID uiTaskID);

	void           StartShowFriends(DrxLobbyUITaskID uiTaskID);
	void           TickShowFriends(DrxLobbyUITaskID uiTaskID);
	void           EndShowFriends(DrxLobbyUITaskID uiTaskID);

	void           StartShowFriendRequest(DrxLobbyUITaskID uiTaskID);

	void           StartSetRichPresence(DrxLobbyUITaskID uiTaskID);
	void           TickSetRichPresence(DrxLobbyUITaskID uiTaskID);

	void           StartShowOnlineRetailBrowser(DrxLobbyUITaskID uiTaskID);
	void           StartShowMessageList(DrxLobbyUITaskID uiTaskID);

	void           StartCheckOnlineRetailStatus(DrxLobbyUITaskID uiTaskID);
	void           EndCheckOnlineRetailStatus(DrxLobbyUITaskID uiTaskID);

	void           JoinFriendGame(const SceNpId* pNpId);
	void           StartJoinFriendGame(DrxLobbyUITaskID uiTaskID);
	void           DispatchJoinEvent(DrxSessionID session, EDrxLobbyError error);

	void           StartAddRecentPlayers(DrxLobbyUITaskID uiTaskID);
	void           TickAddRecentPlayers(DrxLobbyUITaskID uiTaskID);

	void           StartAddActivityFeed(DrxLobbyUITaskID uiTaskID);
	void           TickAddActivityFeed(DrxLobbyUITaskID uiTaskID);

	void           StartGetConsumableOffers(DrxLobbyUITaskID uiTaskID);
	void           TickGetConsumableOffers(DrxLobbyUITaskID uiTaskID);
	void           EventWebApiGetConsumableOffers(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data);
	void           EndGetConsumableOffers(DrxLobbyUITaskID uiTaskID);

	void           StartGetConsumableAssets(DrxLobbyUITaskID uiTaskID);
	void           TickGetConsumableAssets(DrxLobbyUITaskID uiTaskID);
	void           EventWebApiGetConsumableAssets(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data);
	void           EndGetConsumableAssets(DrxLobbyUITaskID uiTaskID);

	void           StartConsumeAsset(DrxLobbyUITaskID uiTaskID);
	void           TickConsumeAsset(DrxLobbyUITaskID uiTaskID);
	void           EventWebApiConsumeAsset(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data);
	void           EndConsumeAsset(DrxLobbyUITaskID uiTaskID);

	void           StartShowDownloadOffer(DrxLobbyUITaskID uiTaskID);
	void           TickShowDownloadOffer(DrxLobbyUITaskID uiTaskID);
	void           EndShowDownloadOffer(DrxLobbyUITaskID uiTaskID);

	STask           m_task[MAX_LOBBYUI_TASKS];

	CDrxPSNSupport* m_pPSNSupport;
};

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_LOBBYUI_H__
