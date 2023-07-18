// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXLOBBYUI_H__
#define __DRXLOBBYUI_H__

#pragma once

<DinrusX/CoreX/Lobby/IDrxLobbyUI.h>
#include "DrxLobby.h"
#include "DrxMatchMaking.h"

#define MAX_LOBBYUI_PARAMS 2
#define MAX_LOBBYUI_TASKS  2

typedef uint32 DrxLobbyUITaskID;
const DrxLobbyUITaskID DrxLobbyUIInvalidTaskID = 0xffffffff;

class CDrxLobbyUI : public CMultiThreadRefCount, public IDrxLobbyUI
{
public:
	CDrxLobbyUI(CDrxLobby* pLobby, CDrxLobbyService* pService);

	virtual EDrxLobbyError ShowParty(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                                   { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError ShowCommunitySessions(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                       { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError ShowOnlineRetailBrowser(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                     { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError CheckOnlineRetailStatus(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIOnlineRetailStatusCallback cb, void* pCbArg)                                                   { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError ShowMessageList(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                             { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError GetConsumableOffers(uint32 user, DrxLobbyTaskID* pTaskID, const TStoreOfferID* pOfferIDs, uint32 offerIdCount, DrxLobbyUIGetConsumableOffersCallback cb, void* pCbArg) { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError ShowDownloadOffer(uint32 user, TStoreOfferID offerId, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                    { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError GetConsumableAssets(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIGetConsumableAssetsCallback cb, void* pCbArg)                                                      { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError ConsumeAsset(uint32 user, TStoreAssetID assetID, uint32 quantity, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                        { return eCLE_ServiceNotSupported; }
	virtual void           CancelTask(DrxLobbyTaskID lTaskID);

	virtual void           PostLocalizationChecks()                                                                                                                                     {}
	virtual EDrxLobbyError AddRecentPlayers(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, const char* gameModeStr, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg) { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError AddActivityFeed(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                   { return eCLE_ServiceNotSupported; }

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();

	virtual bool           IsDead() const { return false; }

protected:

	struct  STask
	{
		DrxLobbyTaskID        lTaskID;
		EDrxLobbyError        error;
		uint32                startedTask;
		uint32                subTask;
		DrxLobbySessionHandle session;
		void*                 pCb;
		void*                 pCbArg;
		TMemHdl               paramsMem[MAX_LOBBYUI_PARAMS];
		uint32                paramsNum[MAX_LOBBYUI_PARAMS];
		bool                  used;
		bool                  running;
		bool                  canceled;
	};

	EDrxLobbyError StartTask(uint32 eTask, bool startRunning, DrxLobbyUITaskID* pUITaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           FreeTask(DrxLobbyUITaskID uiTaskID);
	EDrxLobbyError CreateTaskParamMem(DrxLobbyUITaskID uiTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	void           UpdateTaskError(DrxLobbyUITaskID uiTaskID, EDrxLobbyError error);

	STask*            m_pTask[MAX_LOBBYUI_TASKS];
	CDrxLobby*        m_pLobby;
	CDrxLobbyService* m_pService;
};

#endif // __DRXLOBBYUI_H__
