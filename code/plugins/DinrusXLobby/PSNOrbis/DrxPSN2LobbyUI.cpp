// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		<DinrusX/libsysmodule.h>
		<DinrusX/CoreX/String/StringUtils.h>
		<DinrusX/invitation_dialog.h>
		<DinrusX/np_profile_dialog.h>
		<DinrusX/np_friendlist_dialog.h>
		<DinrusX/np_commerce_dialog.h>

		#include "DrxPSN2WebApi_Presence.h"
		#include "DrxPSN2WebApi_RecentPlayers.h"
		#include "DrxPSN2WebApi_FriendsList.h"
		#include "DrxPSN2WebApi_ActivityFeed.h"
		#include "DrxPSN2WebApi_Store.h"

		#define LOBBYUI_PARAM_NPID_LIST                   0 //-- mem
		#define LOBBYUI_PARAM_ONLINEID_LIST               0 //-- mem
		#define LOBBYUI_PARAM_SESSION_ID                  1 //-- mem

		#define LOBBYUI_PARAM_NUM_NPIDS                   0 //-- num

		#define LOBBYUI_PARAM_RICHPRESENCE                0 //-- ptr and id

		#define LOBBYUI_PARAM_ADDRECENTPLAYERS            0 //-- ptr and id

		#define LOBBYUI_PARAM_ADDACTIVITYFEED             0 //-- ptr and id

		#define LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST 0 //-- ptr and id
		#define LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS 1 //-- ptr and id

		#define LOBBYUI_PARAM_GETCONSUMABLEASSETS_REQUEST 0 //-- id only
		#define LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS 1 //-- ptr and id

		#define LOBBYUI_PARAM_CONSUMEASSET_REQUEST        0 //-- ptr and id
		#define LOBBYUI_PARAM_CONSUMEASSET_RESULTS        1 //-- number

		#define LOBBYUI_PARAM_SHOWDOWNLOADOFFER_ID        0 //-- ptr

void CDrxPSNLobbyUI::SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg)
{
	CDrxPSNLobbyUI* _this = static_cast<CDrxPSNLobbyUI*>(pArg);

	switch (ecb)
	{
	case eCE_WebApiEvent:
		{
			// A WebAPI request completed. Chances are it was triggered by a task in LobbyUI.
			// Check to see if any running task matches the WebApiJobId.
			for (uint32 i = 0; i < MAX_LOBBYUI_TASKS; i++)
			{
				STask* pTask = &_this->m_task[i];

				if (pTask->used && pTask->running)
				{
					switch (pTask->subTask)
					{
					case eST_WaitingForRichPresenceCallback:
						{
							// running set rich presence task
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_RICHPRESENCE])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error != PSN_OK)
								{
									// some kind of error occurred, but we don't care for rich presence
									_this->UpdateTaskError(i, eCLE_Success);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					case eST_WaitingForAddRecentPlayersCallback:
						{
							// running add recent players task
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_ADDRECENTPLAYERS])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error != PSN_OK)
								{
									// some kind of error occurred.
									_this->UpdateTaskError(i, eCLE_InternalError);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					case eST_WaitingForAddActivityFeedCallback:
						{
							// running add activity feed task
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_ADDACTIVITYFEED])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error != PSN_OK)
								{
									// some kind of error occurred.
									_this->UpdateTaskError(i, eCLE_InternalError);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					case eST_WaitingForConsumableOffersCallback:
						{
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error == PSN_OK)
								{
									_this->EventWebApiGetConsumableOffers(i, data);
								}
								else
								{
									// some kind of error occurred.
									_this->UpdateTaskError(i, eCLE_InternalError);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					case eST_WaitingForConsumableAssetsCallback:
						{
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_REQUEST])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error == PSN_OK)
								{
									_this->EventWebApiGetConsumableAssets(i, data);
								}
								else
								{
									// some kind of error occurred.
									_this->UpdateTaskError(i, eCLE_InternalError);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					case eST_WaitingForConsumeAssetCallback:
						{
							if (data.m_webApiEvent.m_id == pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_REQUEST])
							{
								// match! Update the task error state and stop the task running
								if (data.m_webApiEvent.m_error == PSN_OK)
								{
									_this->EventWebApiConsumeAsset(i, data);
								}
								else
								{
									// some kind of error occurred.
									_this->UpdateTaskError(i, eCLE_InternalError);
								}
								_this->StopTaskRunning(i);
								data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
							}
						}
						break;
					}
				}
			}
		}
		break;

	case eCE_ErrorEvent:
		{
			for (uint32 i = 0; i < MAX_LOBBYUI_TASKS; i++)
			{
				STask* pTask = &_this->m_task[i];

				if (pTask->used && pTask->running)
				{
					_this->UpdateTaskError(i, MapSupportErrorToLobbyError(data.m_errorEvent.m_error));
				}
			}
		}
		break;
	}
}

CDrxPSNLobbyUI::CDrxPSNLobbyUI(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport) : CDrxLobbyUI(pLobby, pService)
{
	m_pPSNSupport = pSupport;

	// Make the CDrxLobbyUI base pointers point to our data so we can use the common code in CDrxLobbyUI
	for (uint32 i = 0; i < MAX_LOBBYUI_TASKS; i++)
	{
		CDrxLobbyUI::m_pTask[i] = &m_task[i];
	}
}

// TODO: These are for internal tests only during development! Need to be removed later!
void TestWebAPIFeature(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArgCount() == 2)
	{
		INetwork* pNetwork = gEnv->pNetwork;
		IDrxLobbyUI* pLobbyUI = NULL;
		if (pNetwork != NULL)
		{
			IDrxLobby* pLobby = pNetwork->GetLobby();
			if (pLobby != NULL)
			{
				IDrxLobbyService* pLobbyService = pLobby->GetLobbyService(eCLS_Online);
				if (pLobbyService != NULL)
				{
					pLobbyUI = pLobbyService->GetLobbyUI();
				}
			}
		}
		if (pLobbyUI)
		{
			if (!strcmp(pArgs->GetArg(1), "presence"))
			{
				// send idle presence
				SDrxLobbyUserData data;
				data.m_id = 1;
				data.m_type = eCLUDT_Int32;
				data.m_int32 = 4;
				EDrxLobbyError error = pLobbyUI->SetRichPresence(0, &data, 1, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "activity"))
			{
				EDrxLobbyError error = pLobbyUI->AddActivityFeed(0, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "friendslist"))
			{
				((CDrxPSNLobbyService*)(gEnv->pNetwork->GetLobby()->GetLobbyService(eCLS_Online)))->GetFriends()->FriendsGetFriendsList(0, 0, 20, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "showfriends"))
			{
				EDrxLobbyError error = pLobbyUI->ShowFriends(0, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "recentplayers"))
			{
				DrxUserID id = new SDrxPSNUserID;
				drx_sprintf(((SDrxPSNUserID*)id.get())->npId.handle.data, "DrxDerpy");
				EDrxLobbyError error = pLobbyUI->AddRecentPlayers(0, &id, 1, "TestRecentPlayers", NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "gamercard"))
			{
				DrxUserID id = new SDrxPSNUserID;
				drx_sprintf(((SDrxPSNUserID*)id.get())->npId.handle.data, "DrxDerpy");
				EDrxLobbyError error = pLobbyUI->ShowGamerCard(0, id, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "storeproducts"))
			{
				TStoreOfferID offers[2];
				offers[0].productId = "500CUK0000000000";
				offers[1].productId = "100CUK0000000000";
				EDrxLobbyError error = pLobbyUI->GetConsumableOffers(0, NULL, offers, 2, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "downloadoffer"))
			{
				TStoreOfferID offer;
				offer.productId = "500CUK0000000000";
				offer.skuId = "E003";
				EDrxLobbyError error = pLobbyUI->ShowDownloadOffer(0, offer, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "storeassets"))
			{
				EDrxLobbyError error = pLobbyUI->GetConsumableAssets(0, NULL, NULL, NULL);
			}
			else if (!strcmp(pArgs->GetArg(1), "consumeasset"))
			{
				TStoreAssetID assetId = "500CUK";
				uint32 quantity = 1;
				EDrxLobbyError error = pLobbyUI->ConsumeAsset(0, assetId, quantity, NULL, NULL, NULL);
			}
		}
	}
}

EDrxLobbyError CDrxPSNLobbyUI::Initialise()
{
	EDrxLobbyError error = CDrxLobbyUI::Initialise();
	if (error == eCLE_Success)
	{
		int ret;

		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_PROFILE_DIALOG);
		if (ret < PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_PROFILE_DIALOG) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_FRIEND_LIST_DIALOG);
		if (ret < PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_FRIEND_LIST_DIALOG) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_INVITATION_DIALOG);
		if (ret < PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_INVITATION_DIALOG) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE);
		if (ret < PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_COMMERCE) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}

		ret = sceCommonDialogInitialize();
		if (ret < PSN_OK)
		{
			NetLog("sceCommonDialogInitialize failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
	}

	REGISTER_COMMAND("webapi", TestWebAPIFeature, VF_CHEAT, "Debug test a PSN WebAPI feature");

	return error;
}

EDrxLobbyError CDrxPSNLobbyUI::Terminate()
{
	sceInvitationDialogTerminate();
	sceNpFriendListDialogTerminate();
	sceNpProfileDialogTerminate();
	sceNpCommerceDialogTerminate();

	int ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_COMMERCE);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_COMMERCE) failed. ret = 0x%x\n", ret);
	}
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_INVITATION_DIALOG);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_INVITATION_DIALOG) failed. ret = 0x%x\n", ret);
	}
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_FRIEND_LIST_DIALOG);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_FRIEND_LIST_DIALOG) failed. ret = 0x%x\n", ret);
	}
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_PROFILE_DIALOG);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_PROFILE_DIALOG) failed. ret = 0x%x\n", ret);
	}

	return CDrxLobbyUI::Terminate();
}

void CDrxPSNLobbyUI::Tick(CTimeValue tv)
{
	for (uint32 i = 0; i < MAX_LOBBYUI_TASKS; i++)
	{
		STask* pTask = &m_task[i];

		if (pTask->used && pTask->running)
		{
			switch (pTask->subTask)
			{
			case eT_ShowGamerCard:
				TickShowGamerCard(i);
				break;
			case eT_ShowGameInvite:
				TickShowGameInvite(i);
				break;
			case eT_ShowFriends:
				TickShowFriends(i);
				break;
			case eT_SetRichPresence:
				TickSetRichPresence(i);
				break;
			case eT_AddRecentPlayers:
				TickAddRecentPlayers(i);
				break;
			case eT_AddActivityFeed:
				TickAddActivityFeed(i);
				break;
			case eT_GetConsumableOffers:
				TickGetConsumableOffers(i);
				break;
			case eT_GetConsumableAssets:
				TickGetConsumableAssets(i);
				break;
			case eT_ConsumeAsset:
				TickConsumeAsset(i);
				break;
			case eT_ShowDownloadOffer:
				TickShowDownloadOffer(i);
				break;
			}
		}
	}
}

EDrxLobbyError CDrxPSNLobbyUI::StartTask(ETask etask, bool startRunning, uint32 user, DrxLobbyUITaskID* pUITaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg)
{
	DrxLobbyUITaskID tmpUITaskID;
	DrxLobbyUITaskID* pUseUITaskID = pUITaskID ? pUITaskID : &tmpUITaskID;
	EDrxLobbyError error = CDrxLobbyUI::StartTask(etask, startRunning, pUseUITaskID, pLTaskID, h, pCb, pCbArg);
	return error;
}

void CDrxPSNLobbyUI::StartSubTask(ETask etask, DrxLobbyUITaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	pTask->subTask = etask;
}

void CDrxPSNLobbyUI::StartTaskRunning(DrxLobbyUITaskID uiTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_ShowGamerCard:
			StartShowGamerCard(uiTaskID);
			break;

		case eT_ShowGameInvite:
			StartShowGameInvite(uiTaskID);
			break;

		case eT_ShowFriends:
			StartShowFriends(uiTaskID);
			break;

		case eT_ShowFriendRequest:
			StartShowFriendRequest(uiTaskID);
			break;

		case eT_SetRichPresence:
			StartSetRichPresence(uiTaskID);
			break;

		case eT_ShowOnlineRetailBrowser:
			StartShowOnlineRetailBrowser(uiTaskID);
			break;

		case eT_JoinFriendsGame:
			StartJoinFriendGame(uiTaskID);
			break;

		case eT_ShowMessageList:
			StartShowMessageList(uiTaskID);
			break;

		case eT_AddRecentPlayers:
			StartAddRecentPlayers(uiTaskID);
			break;

		case eT_AddActivityFeed:
			StartAddActivityFeed(uiTaskID);
			break;

		case eT_GetConsumableOffers:
			StartGetConsumableOffers(uiTaskID);
			break;
		case eT_GetConsumableAssets:
			StartGetConsumableAssets(uiTaskID);
			break;
		case eT_ConsumeAsset:
			StartConsumeAsset(uiTaskID);
			break;

		case eT_ShowDownloadOffer:
			StartShowDownloadOffer(uiTaskID);
			break;
		}
	}
}

void CDrxPSNLobbyUI::EndTask(DrxLobbyUITaskID uiTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		if (pTask->pCb)
		{
			switch (pTask->startedTask)
			{
			case eT_ShowGamerCard:
				EndShowGamerCard(uiTaskID);
				break;
			case eT_ShowFriends:
				EndShowFriends(uiTaskID);
				break;
			case eT_ShowGameInvite:
				EndShowGameInvite(uiTaskID);
				break;

			case eT_ShowFriendRequest:
			case eT_SetRichPresence:
			case eT_ShowOnlineRetailBrowser:
			case eT_JoinFriendsGame:
			case eT_ShowMessageList:
			case eT_AddRecentPlayers:
			case eT_AddActivityFeed:
				((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
				break;

			case eT_CheckOnlineRetailStatus:
				EndCheckOnlineRetailStatus(uiTaskID);
				break;

			case eT_GetConsumableOffers:
				EndGetConsumableOffers(uiTaskID);
				break;
			case eT_GetConsumableAssets:
				EndGetConsumableAssets(uiTaskID);
				break;
			case eT_ConsumeAsset:
				EndConsumeAsset(uiTaskID);
				break;

			case eT_ShowDownloadOffer:
				EndShowDownloadOffer(uiTaskID);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] LobbyUI EndTask %d (%d) Result %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		FreeTask(uiTaskID);
	}
}

void CDrxPSNLobbyUI::StopTaskRunning(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		pTask->running = false;
		TO_GAME_FROM_LOBBY(&CDrxPSNLobbyUI::EndTask, this, uiTaskID);
	}
}

EDrxLobbyError CDrxPSNLobbyUI::ShowGamerCard(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	if (userID.IsValid())
	{
		DrxLobbyUITaskID uiTaskID;

		error = StartTask(eT_ShowGamerCard, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

		if (error == eCLE_Success)
		{
			SceNpId npId = ((SDrxPSNUserID*)userID.get())->npId;

			error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_NPID_LIST, &npId, sizeof(npId));

			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
			}
			else
			{
				FreeTask(uiTaskID);
			}
		}
	}
	else
	{
		error = eCLE_InvalidUser;
	}

	return error;
}

void CDrxPSNLobbyUI::StartShowGamerCard(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	SceNpId* pNpId = (SceNpId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_NPID_LIST]);

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);
	if (ret == PSN_OK)
	{
		ret = sceNpProfileDialogInitialize();
		if ((ret == PSN_OK) || (ret == SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED))
		{
			SceNpProfileDialogParam param;
			sceNpProfileDialogParamInitialize(&param);
			param.mode = SCE_NP_PROFILE_DIALOG_MODE_NORMAL;
			param.userId = userId;
			param.targetOnlineId = pNpId->handle;

			ret = sceNpProfileDialogOpen(&param);
			if (ret == PSN_OK)
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				NetLog("sceNpProfileDialogOpen failed. ret = 0x%x\n", ret);
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			NetLog("sceNpProfileDialogInitialize failed. ret = 0x%x\n", ret);
			UpdateTaskError(uiTaskID, eCLE_InternalError);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_InternalError);
	}

	if (pTask->error != eCLE_Success)
	{
		sceNpProfileDialogTerminate();
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickShowGamerCard(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->canceled)
	{
		sceNpProfileDialogTerminate();
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
		return;
	}

	int ret = sceNpProfileDialogUpdateStatus();
	if (ret == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		// completed.
		SceNpProfileDialogResult result;
		memset(&result, 0, sizeof(result));

		ret = sceNpProfileDialogGetResult(&result);
		if (ret == PSN_OK)
		{
			if (result.result == PSN_OK)
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			// Error handling
			UpdateTaskError(uiTaskID, eCLE_InternalError);
		}

		sceNpProfileDialogTerminate();

		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::EndShowGamerCard(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::ShowGameInvite(uint32 user, DrxSessionHandle gh, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
		#if USE_DRX_MATCHMAKING
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	CDrxMatchMaking* pMatchMaking = (CDrxMatchMaking*)m_pLobby->GetMatchMaking();

	if (pMatchMaking)
	{
		DrxLobbySessionHandle h = pMatchMaking->GetSessionHandleFromGameSessionHandle(gh);

		if (h != DrxLobbyInvalidSessionHandle)
		{
			DrxLobbyUITaskID uiTaskID;

			error = StartTask(eT_ShowGameInvite, false, user, &uiTaskID, pTaskID, h, (void*)cb, pCbArg);

			if (error == eCLE_Success)
			{
				STask* pTask = &m_task[uiTaskID];

				pTask->paramsNum[LOBBYUI_PARAM_NUM_NPIDS] = numUserIDs;

				if (numUserIDs > 0)
				{
					error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_ONLINEID_LIST, NULL, numUserIDs * sizeof(SceNpOnlineId));
					if (error == eCLE_Success)
					{
						SceNpOnlineId* pOnlineIds = (SceNpOnlineId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_ONLINEID_LIST]);

						for (uint32 i = 0; i < numUserIDs; i++)
						{
							if (pUserIDs[i].IsValid())
							{
								memcpy(&pOnlineIds[i], &(((SDrxPSNUserID*)pUserIDs[i].get())->npId.handle), sizeof(SceNpOnlineId));
							}
							else
							{
								error = eCLE_InvalidUser;
								break;
							}
						}
					}
				}

				if (error == eCLE_Success)
				{
					error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_SESSION_ID, NULL, sizeof(SceNpSessionId));
					if (error == eCLE_Success)
					{
						SceNpSessionId* pSessionId = (SceNpSessionId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_SESSION_ID]);
						DrxSessionID id = pMatchMaking->SessionGetDrxSessionIDFromDrxSessionHandle(gh);
						if (id != NULL)
						{
							memcpy(pSessionId, &(((SDrxPSNSessionID*)id.get())->m_sessionId), sizeof(SceNpSessionId));
						}
						else
						{
							error = eCLE_InvalidSession;
						}
					}
				}

				if (error == eCLE_Success)
				{
					FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
				}
				else
				{
					FreeTask(uiTaskID);
				}
			}
		}
		else
		{
			error = eCLE_InvalidSession;
		}
	}
	else
	{
		error = eCLE_ServiceNotSupported;
	}

	return error;
		#else // USE_DRX_MATCHMAKING
	return eCLE_ServiceNotSupported;
		#endif // USE_DRX_MATCHMAKING
}

void CDrxPSNLobbyUI::StartShowGameInvite(DrxLobbyUITaskID uiTaskID)
{
		#if USE_DRX_MATCHMAKING
	STask* pTask = &m_task[uiTaskID];
	uint32 numUserIDs = pTask->paramsNum[LOBBYUI_PARAM_NUM_NPIDS];
	SceNpOnlineId* pOnlineIds = NULL;
	if (numUserIDs > 0)
	{
		pOnlineIds = (SceNpOnlineId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_ONLINEID_LIST]);
	}
	SceNpSessionId* pSessionId = (SceNpSessionId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_SESSION_ID]);

	char body[SCE_INVITATION_DIALOG_MAX_USER_MSG_SIZE] = "Undefined";

	SDrxLobbyXMBString data;
	data.m_pStringBuffer = (uint8*)body;
	data.m_sizeOfStringBuffer = SCE_INVITATION_DIALOG_MAX_USER_MSG_SIZE - 1;

	SConfigurationParams neededInfo = {
		CLCC_PSN_INVITE_BODY_STRING, { NULL }
	};
	neededInfo.m_pData = &data;

	m_pLobby->GetConfigurationInformation(&neededInfo, 1);

	if (data.m_sizeOfStringBuffer >= SCE_INVITATION_DIALOG_MAX_USER_MSG_SIZE)
	{
		drx_sprintf(body, "Error: Body text is too long!");
	}

	body[SCE_INVITATION_DIALOG_MAX_USER_MSG_SIZE - 1] = 0;

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);
	if (ret == PSN_OK)
	{
		ret = sceInvitationDialogInitialize();
		if ((ret == PSN_OK) || (ret == SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED))
		{
			SceInvitationDialogDataParam dataParam;
			memset(&dataParam, 0, sizeof(dataParam));
			dataParam.SendInfo.sessionId = pSessionId;
			dataParam.SendInfo.userMessage = body;
			dataParam.SendInfo.addressParam.addressType = SCE_INVITATION_DIALOG_ADDRESS_TYPE_USERDISABLE;
			dataParam.SendInfo.addressParam.addressInfo.UserSelectDisableAddress.onlineIdsCount = numUserIDs;
			dataParam.SendInfo.addressParam.addressInfo.UserSelectDisableAddress.onlineIds = pOnlineIds;

			SceInvitationDialogParam param;
			sceInvitationDialogParamInitialize(&param);
			param.userId = userId;
			param.mode = SCE_INVITATION_DIALOG_MODE_SEND;
			param.dataParam = &dataParam;

			ret = sceInvitationDialogOpen(&param);
			if (ret == PSN_OK)
			{
				//-- success!
				NetLog("INVITE OPEN: session %s", pSessionId->data);
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			NetLog("sceInvitationDialogInitialize failed. ret = 0x%x\n", ret);
			UpdateTaskError(uiTaskID, eCLE_InternalError);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_InternalError);
	}

	if (pTask->error != eCLE_Success)
	{
		sceInvitationDialogTerminate();
		StopTaskRunning(uiTaskID);
	}
		#endif // USE_DRX_MATCHMAKING
}

void CDrxPSNLobbyUI::TickShowGameInvite(DrxLobbyUITaskID uiTaskID)
{
	int ret;
	STask* pTask = &m_task[uiTaskID];

	if (pTask->canceled)
	{
		sceInvitationDialogTerminate();
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
		return;
	}

	ret = sceInvitationDialogUpdateStatus();
	if (ret == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		// completed.
		SceInvitationDialogResult result;
		memset(&result, 0, sizeof(result));

		ret = sceInvitationDialogGetResult(&result);
		if (ret == PSN_OK)
		{
			if (result.errorCode == PSN_OK)
			{
				if (result.result == SCE_COMMON_DIALOG_RESULT_OK)
				{
					NetLog("INVITE SENT");
				}
				else
				{
					NetLog("INVITE CANCELLED");
				}
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			// Error handling
			UpdateTaskError(uiTaskID, eCLE_InternalError);
		}

		sceInvitationDialogTerminate();

		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::EndShowGameInvite(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::ShowFriends(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_ShowFriends, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
	}

	return error;
}

void CDrxPSNLobbyUI::StartShowFriends(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);
	if (ret == PSN_OK)
	{
		ret = sceNpFriendListDialogInitialize();
		if ((ret == PSN_OK) || (ret == SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED))
		{
			SceNpFriendListDialogParam param;
			sceNpFriendListDialogParamInitialize(&param);
			param.mode = SCE_NP_FRIEND_LIST_DIALOG_MODE_FRIEND_LIST;
			param.userId = userId;
			param.menuItems = SCE_NP_FRIEND_LIST_DIALOG_MENU_ITEM_ONLINE | SCE_NP_FRIEND_LIST_DIALOG_MENU_ITEM_ALL_FRIENDS;

			ret = sceNpFriendListDialogOpen(&param);
			if (ret == PSN_OK)
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				NetLog("sceNpFriendListDialogOpen failed. ret = 0x%x\n", ret);
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			NetLog("sceNpFriendListDialogInitialize failed. ret = 0x%x\n", ret);
			UpdateTaskError(uiTaskID, eCLE_InternalError);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_InternalError);
	}

	if (pTask->error != eCLE_Success)
	{
		sceNpFriendListDialogTerminate();
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickShowFriends(DrxLobbyUITaskID uiTaskID)
{
	int ret;
	STask* pTask = &m_task[uiTaskID];

	if (pTask->canceled)
	{
		sceNpFriendListDialogTerminate();
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
		return;
	}

	ret = sceNpFriendListDialogUpdateStatus();
	if (ret == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		// completed.
		SceNpFriendListDialogResult result;
		memset(&result, 0, sizeof(result));

		ret = sceNpFriendListDialogGetResult(&result);
		if (ret == PSN_OK)
		{
			if (result.result == PSN_OK)
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			// Error handling
			UpdateTaskError(uiTaskID, eCLE_InternalError);
		}

		sceNpFriendListDialogTerminate();

		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::EndShowFriends(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::ShowFriendRequest(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	/*
	   if (userID.IsValid())
	   {
	    DrxLobbyUITaskID uiTaskID;

	    error = StartTask(eT_ShowFriendRequest, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	    if (error == eCLE_Success)
	    {
	      SceNpId npId = ((SDrxPSNUserID*)userID.get())->npId;

	      error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_NPID_LIST, &npId, sizeof(npId));
	      if (error == eCLE_Success)
	      {
	        FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
	      }
	      else
	      {
	        FreeTask(uiTaskID);
	      }
	    }
	   }
	   else
	   {
	    error = eCLE_InvalidUser;
	   }
	 */
	return error;
}

void CDrxPSNLobbyUI::StartShowFriendRequest(DrxLobbyUITaskID uiTaskID)
{
	/*
	   STask* pTask = &m_task[uiTaskID];
	   SceNpId* pNpId = (SceNpId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_NPID_LIST]);

	   char subject[SCE_NP_BASIC_SUBJECT_CHARACTER_MAX] = "Undefined";
	   char body[SCE_NP_BASIC_BODY_CHARACTER_MAX] = "Undefined";

	   SDrxLobbyXMBString data[2];
	   data[0].m_pStringBuffer = (uint8*)subject;
	   data[0].m_sizeOfStringBuffer = SCE_NP_BASIC_SUBJECT_CHARACTER_MAX;
	   data[1].m_pStringBuffer = (uint8*)body;
	   data[1].m_sizeOfStringBuffer = SCE_NP_BASIC_BODY_CHARACTER_MAX;

	   SConfigurationParams neededInfo[2] = {{CLCC_PSN_FRIEND_REQUEST_SUBJECT_STRING, {NULL}},
	                                        {CLCC_PSN_FRIEND_REQUEST_BODY_STRING, {NULL}}};
	   neededInfo[0].m_pData = &data[0];
	   neededInfo[1].m_pData = &data[1];

	   m_pLobby->GetConfigurationInformation(neededInfo, 2);

	   if (data[0].m_sizeOfStringBuffer >= SCE_NP_BASIC_SUBJECT_CHARACTER_MAX)
	   {
	    drx_sprintf(subject, "E 0x%x", SCE_NP_BASIC_ERROR_EXCEEDS_MAX);
	   }
	   if (data[1].m_sizeOfStringBuffer >= SCE_NP_BASIC_BODY_CHARACTER_MAX)
	   {
	    drx_sprintf(body, "E 0x%x", SCE_NP_BASIC_ERROR_EXCEEDS_MAX);
	   }

	   subject[SCE_NP_BASIC_SUBJECT_CHARACTER_MAX-1] = 0;
	   body[SCE_NP_BASIC_BODY_CHARACTER_MAX-1] = 0;

	   SceNpBasicMessageDetails msg;
	   memset(&msg, 0, sizeof(msg));

	   msg.mainType = SCE_NP_BASIC_MESSAGE_MAIN_TYPE_ADD_FRIEND;
	   msg.subType = SCE_NP_BASIC_MESSAGE_ADD_FRIEND_SUBTYPE_NONE;
	   msg.msgFeatures = SCE_NP_BASIC_MESSAGE_FEATURES_MULTI_RECEIPIENTS;
	   msg.npids = pNpId;
	   msg.count = 1;
	   msg.subject = subject;
	   msg.body = body;
	   msg.data = NULL;
	   msg.size = 0;

	   int ret = sceNpBasicSendMessageGui(&msg, SYS_MEMORY_CONTAINER_ID_INVALID);
	   if (ret == 0)
	   {
	    //-- success!
	    UpdateTaskError(uiTaskID, eCLE_Success);
	   }
	   else
	   {
	    UpdateTaskError(uiTaskID, eCLE_InternalError);
	   }
	 */
	UpdateTaskError(uiTaskID, eCLE_ServiceNotSupported);
	StopTaskRunning(uiTaskID);
}

EDrxLobbyError CDrxPSNLobbyUI::SetRichPresence(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbyUITaskID uiTaskID;

	if ((numData > 0) && pData)
	{
		error = StartTask(eT_SetRichPresence, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[uiTaskID];

			pTask->paramsNum[LOBBYUI_PARAM_RICHPRESENCE] = INVALID_WEBAPI_JOB_ID;
			error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_RICHPRESENCE, 0, sizeof(SDrxPSNOrbisWebApiSetPresenceInput));
			if (error == eCLE_Success)
			{
				SDrxPSNOrbisWebApiSetPresenceInput* pPresenceData = (SDrxPSNOrbisWebApiSetPresenceInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_RICHPRESENCE]);
				memset(pPresenceData, 0, sizeof(SDrxPSNOrbisWebApiSetPresenceInput));

				SDrxLobbyPresenceConverter convert;
				convert.m_pData = pData;
				convert.m_numData = numData;
				convert.m_pStringBuffer = (uint8*)pPresenceData->presence;
				convert.m_sizeOfStringBuffer = DRX_WEBAPI_SETPRESENCE_MAX_SIZE;
				convert.m_sessionId = DrxSessionInvalidID;

				SConfigurationParams presenceItem = {
					CLCC_DRXLOBBY_PRESENCE_CONVERTER, { NULL }
				};
				presenceItem.m_pData = &convert;

				m_pLobby->GetConfigurationInformation(&presenceItem, 1);

				if ((convert.m_sizeOfStringBuffer > 0) && (convert.m_sizeOfStringBuffer < DRX_WEBAPI_SETPRESENCE_MAX_SIZE))
				{
					pPresenceData->presence[DRX_WEBAPI_SETPRESENCE_MAX_SIZE - 1] = 0;
					FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
				}
				else
				{
					error = eCLE_InvalidRequest;
					FreeTask(uiTaskID);
				}
			}
			else
			{
				FreeTask(uiTaskID);
			}
		}
	}
	else
	{
		error = eCLE_InvalidParam;
	}

	return error;
}

void CDrxPSNLobbyUI::StartSetRichPresence(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->paramsMem[LOBBYUI_PARAM_RICHPRESENCE] != TMemInvalidHdl)
	{
		if (!m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
		{
			UpdateTaskError(uiTaskID, eCLE_Success);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickSetRichPresence(DrxLobbyUITaskID uiTaskID)
{
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		if (pTask->paramsMem[LOBBYUI_PARAM_RICHPRESENCE] != TMemInvalidHdl)
		{
			SDrxPSNOrbisWebApiSetPresenceInput* pPresenceData = (SDrxPSNOrbisWebApiSetPresenceInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_RICHPRESENCE]);
			memcpy(&pPresenceData->id, &m_pPSNSupport->GetLocalNpId()->handle, sizeof(SceNpOnlineId));

			pTask->paramsNum[LOBBYUI_PARAM_RICHPRESENCE] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::SetPresence, pTask->paramsMem[LOBBYUI_PARAM_RICHPRESENCE]);
			if (pTask->paramsNum[LOBBYUI_PARAM_RICHPRESENCE] != INVALID_WEBAPI_JOB_ID)
			{
				StartSubTask(eST_WaitingForRichPresenceCallback, uiTaskID);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
				StopTaskRunning(uiTaskID);
			}
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
	}
}

EDrxLobbyError CDrxPSNLobbyUI::ShowOnlineRetailBrowser(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_ShowOnlineRetailBrowser, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
	}

	return error;
}

void CDrxPSNLobbyUI::StartShowOnlineRetailBrowser(DrxLobbyUITaskID uiTaskID)
{
	StopTaskRunning(uiTaskID);
}

EDrxLobbyError CDrxPSNLobbyUI::CheckOnlineRetailStatus(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIOnlineRetailStatusCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_InternalError;
	return error;
}

void CDrxPSNLobbyUI::EndCheckOnlineRetailStatus(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	SDrxLobbyUIOnlineRetailCounts counts;

	((DrxLobbyUIOnlineRetailStatusCallback)pTask->pCb)(pTask->lTaskID, pTask->error, &counts, pTask->pCbArg);
}

void CDrxPSNLobbyUI::JoinFriendGame(const SceNpId* pNpId)
{
		#if USE_DRX_MATCHMAKING
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbyUITaskID uiTaskID;

	if (pNpId)
	{
		error = StartTask(eT_JoinFriendsGame, false, 0, &uiTaskID, NULL, DrxLobbyInvalidSessionHandle, NULL, NULL);
		if (error == eCLE_Success)
		{
			error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_NPID_LIST, pNpId, sizeof(SceNpId));
			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
			}
			else
			{
				FreeTask(uiTaskID);
			}
		}
	}
		#endif // USE_DRX_MATCHMAKING
}

void CDrxPSNLobbyUI::StartJoinFriendGame(DrxLobbyUITaskID uiTaskID)
{
		#if USE_DRX_MATCHMAKING
	/*
	   STask* pTask = &m_task[uiTaskID];
	   EDrxLobbyError error = eCLE_Success;
	   SceNpBasicPresenceDetails2 details;
	   SDrxPSNSessionID* pSessionInfo = NULL;

	   memset(&details, 0x00, sizeof(details));
	   details.struct_size = sizeof(details);

	   SceNpId* pNpId = (SceNpId*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_NPID_LIST]);

	   int ret = sceNpBasicGetFriendPresenceByNpId2(pNpId, &details, 0);
	   if (ret == 0)
	   {
	    details.status[SCE_NP_BASIC_PRESENCE_EXTENDED_STATUS_SIZE_MAX-1] = 0;
	    NetLog("Friend Presence details: %s", details.status);
	    NetLog("Friend Presence details: looking for %d bytes for session data, got %d bytes.", sizeof(SSynchronisedSessionID), details.size);
	    if (details.size == sizeof(SSynchronisedSessionID))
	    {
	      //-- we should have the sessionId for the invite in our buffer now.
	      pSessionInfo = new SDrxPSNSessionID;
	      if (pSessionInfo)
	      {
	        memcpy(&pSessionInfo->m_sessionInfo, details.data, details.size);
	        pSessionInfo->m_sessionInfo.m_fromInvite = false;
	        m_pPSNSupport->PrepareForInviteOrJoin(pSessionInfo);

	        NetLog("JOIN RECV: server=0x%x, world=0x%x, room=0x%llx", pSessionInfo->m_sessionInfo.m_serverId, pSessionInfo->m_sessionInfo.m_worldId, (uint64)pSessionInfo->m_sessionInfo.m_roomId);
	      }
	      else
	      {
	        error = eCLE_OutOfMemory;
	      }
	    }
	    else
	    {
	      error = eCLE_InvalidJoinFriendData;
	    }
	   }
	   else
	   {
	    NetLog("Friend Presence details: error 0x%x returned.", ret);
	    if (ret == SCE_NP_BASIC_ERROR_BUSY)
	    {
	      error = eCLE_SystemIsBusy;
	    }
	    else
	    {
	      error = eCLE_InvalidJoinFriendData;
	    }
	   }

	   TO_GAME_FROM_LOBBY(&CDrxPSNLobbyUI::DispatchJoinEvent, this, DrxSessionID(pSessionInfo), error);

	   UpdateTaskError(uiTaskID, error);
	   StopTaskRunning(uiTaskID);
	 */
		#endif // USE_DRX_MATCHMAKING
}

EDrxLobbyError CDrxPSNLobbyUI::ShowMessageList(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_ShowMessageList, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
	}

	return error;
}

void CDrxPSNLobbyUI::StartShowMessageList(DrxLobbyUITaskID uiTaskID)
{
	/*
	   int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
	   if (ret == 0)
	   {
	    //-- success!
	    UpdateTaskError(uiTaskID, eCLE_Success);
	   }
	   else
	   {
	    UpdateTaskError(uiTaskID, eCLE_InternalError);
	   }
	 */

	UpdateTaskError(uiTaskID, eCLE_ServiceNotSupported);
	StopTaskRunning(uiTaskID);
}

void CDrxPSNLobbyUI::PostLocalizationChecks()
{
}

EDrxLobbyError CDrxPSNLobbyUI::AddRecentPlayers(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, const char* gameModeStr, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_AddRecentPlayers, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];
		pTask->paramsNum[LOBBYUI_PARAM_ADDRECENTPLAYERS] = INVALID_WEBAPI_JOB_ID;

		error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_ADDRECENTPLAYERS, NULL, sizeof(SDrxPSNOrbisWebApiAddRecentPlayersInput));
		if (error == eCLE_Success)
		{
			SDrxPSNOrbisWebApiAddRecentPlayersInput* pRecentPlayersData = (SDrxPSNOrbisWebApiAddRecentPlayersInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_ADDRECENTPLAYERS]);

			sce::Json::Object source;
			source["meta"] = sce::Json::String(m_pPSNSupport->GetLocalUserName());
			source["type"] = sce::Json::String("ONLINE_ID");

			sce::Json::String storyType = sce::Json::String("PLAYED_WITH");

			sce::Json::Array targets;
			for (uint32 i = 0; i < numUserIDs; ++i)
			{
				if (pUserIDs[i].IsValid())
				{
					SceNpOnlineId* pOID = &((SDrxPSNUserID*)pUserIDs[i].get())->npId.handle;
					sce::Json::Object itarget;
					itarget["meta"] = sce::Json::String(pOID->data);
					itarget["type"] = sce::Json::String("ONLINE_ID");
					targets.push_back(itarget);
				}
			}
			SConfigurationParams titleInfo[1] = {
				{ CLCC_PSN_TITLE_ID, { NULL }
				}
			};
			m_pPSNSupport->GetLobby()->GetConfigurationInformation(titleInfo, 1);
			sce::Json::Object titletarget;
			titletarget["meta"] = sce::Json::String((const char*)titleInfo[0].m_pData);
			titletarget["type"] = sce::Json::String("TITLE_ID");
			targets.push_back(titletarget);
			sce::Json::Object desctarget;
			desctarget["meta"] = sce::Json::String(gameModeStr);
			desctarget["type"] = sce::Json::String("PLAYED_DESCRIPTION");
			targets.push_back(desctarget);

			sce::Json::Object jsonDoc;
			jsonDoc["source"] = source;
			jsonDoc["storyType"] = storyType;
			jsonDoc["targets"] = targets;

			sce::Json::Value jsonRoot;
			jsonRoot.clear();
			jsonRoot.set(jsonDoc);

			sce::Json::String jsonString;
			jsonRoot.serialize(jsonString);

			drx_sprintf(pRecentPlayersData->feed, jsonString.c_str(), jsonString.length());
		}

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxPSNLobbyUI::StartAddRecentPlayers(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->paramsMem[LOBBYUI_PARAM_ADDRECENTPLAYERS] != TMemInvalidHdl)
	{
		if (!m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
		{
			UpdateTaskError(uiTaskID, eCLE_Success);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickAddRecentPlayers(DrxLobbyUITaskID uiTaskID)
{
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		if (pTask->paramsMem[LOBBYUI_PARAM_ADDRECENTPLAYERS] != TMemInvalidHdl)
		{
			pTask->paramsNum[LOBBYUI_PARAM_ADDRECENTPLAYERS] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::AddRecentPlayers, pTask->paramsMem[LOBBYUI_PARAM_ADDRECENTPLAYERS]);
			if (pTask->paramsNum[LOBBYUI_PARAM_ADDRECENTPLAYERS] != INVALID_WEBAPI_JOB_ID)
			{
				StartSubTask(eST_WaitingForAddRecentPlayersCallback, uiTaskID);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
				StopTaskRunning(uiTaskID);
			}
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
	}
}

EDrxLobbyError CDrxPSNLobbyUI::AddActivityFeed(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_AddActivityFeed, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];
		pTask->paramsNum[LOBBYUI_PARAM_ADDACTIVITYFEED] = INVALID_WEBAPI_JOB_ID;

		error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_ADDACTIVITYFEED, NULL, sizeof(SDrxPSNOrbisWebApiAddActivityFeedInput));
		if (error == eCLE_Success)
		{
			SDrxPSNOrbisWebApiAddActivityFeedInput* pActivityData = (SDrxPSNOrbisWebApiAddActivityFeedInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_ADDACTIVITYFEED]);

			SConfigurationParams titleInfo[1] = {
				{ CLCC_PSN_TITLE_ID, { NULL }
				}
			};
			m_pPSNSupport->GetLobby()->GetConfigurationInformation(titleInfo, 1);

			drx_sprintf(pActivityData->feed, DRX_WEBAPI_ACTIVITYFEED_INGAME_POST_BODY_SAMPLE, m_pPSNSupport->GetLocalUserName(), titleInfo[0].m_pData);
		}

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxPSNLobbyUI::StartAddActivityFeed(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->paramsMem[LOBBYUI_PARAM_ADDACTIVITYFEED] != TMemInvalidHdl)
	{
		if (!m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
		{
			UpdateTaskError(uiTaskID, eCLE_Success);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickAddActivityFeed(DrxLobbyUITaskID uiTaskID)
{
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		if (pTask->paramsMem[LOBBYUI_PARAM_ADDACTIVITYFEED] != TMemInvalidHdl)
		{
			pTask->paramsNum[LOBBYUI_PARAM_ADDACTIVITYFEED] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::PostActivity, pTask->paramsMem[LOBBYUI_PARAM_ADDACTIVITYFEED]);
			if (pTask->paramsNum[LOBBYUI_PARAM_ADDACTIVITYFEED] != INVALID_WEBAPI_JOB_ID)
			{
				StartSubTask(eST_WaitingForAddActivityFeedCallback, uiTaskID);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
				StopTaskRunning(uiTaskID);
			}
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
	}
}

EDrxLobbyError CDrxPSNLobbyUI::GetConsumableOffers(uint32 user, DrxLobbyTaskID* pTaskID, const TStoreOfferID* pOfferIDs, uint32 offerIdCount, DrxLobbyUIGetConsumableOffersCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_GetConsumableOffers, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];
		pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST] = INVALID_WEBAPI_JOB_ID;
		pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS] = 0;

		error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST, NULL, sizeof(SDrxPSNOrbisWebApiGetCommerceInput));
		if (error == eCLE_Success)
		{
			SDrxPSNOrbisWebApiGetCommerceInput* pData = (SDrxPSNOrbisWebApiGetCommerceInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST]);
			new(pData) SDrxPSNOrbisWebApiGetCommerceInput();

			pData->numProducts = 0;
			for (uint32 i = 0; (i < offerIdCount) && (pData->numProducts < DRX_WEBAPI_COMMERCE_MAX_PRODUCTS); ++i)
			{
				bool bFound = false;
				for (uint32 f = 0; f < pData->numProducts; ++f)
				{
					if (pData->products[f].productId == pOfferIDs[i].productId)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					pData->products[pData->numProducts++].productId = pOfferIDs[i].productId;
				}
			}
		}

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxPSNLobbyUI::StartGetConsumableOffers(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST] != TMemInvalidHdl)
	{
		m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickGetConsumableOffers(DrxLobbyUITaskID uiTaskID)
{
	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		if (pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST] != TMemInvalidHdl)
		{
			pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::GetCommerceList, pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST]);
			if (pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_REQUEST] != INVALID_WEBAPI_JOB_ID)
			{
				StartSubTask(eST_WaitingForConsumableOffersCallback, uiTaskID);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
				StopTaskRunning(uiTaskID);
			}
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
			StopTaskRunning(uiTaskID);
		}
	}
}

void CDrxPSNLobbyUI::EventWebApiGetConsumableOffers(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data)
{
	STask* pTask = &m_task[uiTaskID];
	pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS] = 0;

	if (data.m_webApiEvent.m_error == PSN_OK)
	{
		if (data.m_webApiEvent.m_pResponseBody)
		{
			if (data.m_webApiEvent.m_pResponseBody->eType == SDrxPSNWebApiResponseBody::E_JSON)
			{
				// count json items - annoyingly the "total_results" field in the json document always seems to be 0.
				const sce::Json::Value& root = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot;
				if (root.getType() == sce::Json::kValueTypeArray)
				{
					const sce::Json::Array& rootArray = root.getArray();
					uint32 offercount = (uint32)rootArray.size();
					uint32 skucount = 0;
					if (offercount > 0)
					{
						for (uint32 i = 0; i < offercount; ++i)
						{
							const sce::Json::Value& offerObj = root[i];
							if (offerObj.getType() == sce::Json::kValueTypeObject)
							{
								const sce::Json::Value& skus = offerObj["skus"];
								if (skus.getType() == sce::Json::kValueTypeArray)
								{
									const sce::Json::Array& skusArray = skus.getArray();
									skucount += skusArray.size();
								}
							}
						}

						EDrxLobbyError error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS, NULL, sizeof(SDrxLobbyConsumableOfferData) * skucount);
						if (error == eCLE_Success)
						{
							SDrxLobbyConsumableOfferData* pOffers = (SDrxLobbyConsumableOfferData*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS]);

							for (uint32 i = 0; i < offercount; ++i)
							{
								const sce::Json::Value& offerObj = root[i];
								if (offerObj.getType() == sce::Json::kValueTypeObject)
								{
									const sce::Json::Value& offerId = offerObj["label"];
									const sce::Json::Value& offerName = offerObj["name"];
									const sce::Json::Value& offerDesc = offerObj["long_desc"];

									const sce::Json::Value& skus = offerObj["skus"];
									if (skus.getType() == sce::Json::kValueTypeArray)
									{
										const sce::Json::Array& skusArray = skus.getArray();
										for (uint32 j = 0; j < (uint32)skusArray.size(); ++j)
										{
											const sce::Json::Value& skuObj = skus[j];
											if (skuObj.getType() == sce::Json::kValueTypeObject)
											{
												SDrxLobbyConsumableOfferData* pCurrentOffer = &pOffers[pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS]];
												new(pCurrentOffer) SDrxLobbyConsumableOfferData();
												pCurrentOffer->Clear();

												if (offerId.getType() == sce::Json::kValueTypeString)
												{
													pCurrentOffer->offerID.productId = offerId.toString().c_str();
												}
												if (offerName.getType() == sce::Json::kValueTypeString)
												{
													pCurrentOffer->name = DrxStringUtils::UTF8ToWStr(offerName.toString().c_str());
												}
												if (offerDesc.getType() == sce::Json::kValueTypeString)
												{
													pCurrentOffer->description = DrxStringUtils::UTF8ToWStr(offerDesc.toString().c_str());
												}

												const sce::Json::Value& assetId = skuObj["label"];
												if (assetId.getType() == sce::Json::kValueTypeString)
												{
													pCurrentOffer->assetID = assetId.toString().c_str();
													pCurrentOffer->offerID.skuId = assetId.toString().c_str();
												}
												const sce::Json::Value& price = skuObj["display_price"];
												if (price.getType() == sce::Json::kValueTypeString)
												{
													pCurrentOffer->price = DrxStringUtils::UTF8ToWStr(price.toString().c_str());
												}
												const sce::Json::Value& usecount = skuObj["use_count"];
												if (usecount.getType() == sce::Json::kValueTypeUInteger)
												{
													pCurrentOffer->purchaseQuantity = usecount.getUInteger();
												}

												pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS]++;
											}
										}
									}
								}
							}

							// All finished - success!
							UpdateTaskError(uiTaskID, eCLE_Success);
							StopTaskRunning(uiTaskID);
							return;
						}
					}
					// nothing in array
				}
				// not an array - fail.
			}
			// response data not json
		}
		// response error
	}

	// if we reach here, the json is bad, or we have failed for some uncaught reason
	UpdateTaskError(uiTaskID, eCLE_InternalError);
	StopTaskRunning(uiTaskID);
}

void CDrxPSNLobbyUI::EndGetConsumableOffers(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	SDrxLobbyConsumableOfferData* pOffers = NULL;

	if ((pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS] > 0) &&
	    (pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS] != TMemInvalidHdl))
	{
		pOffers = (SDrxLobbyConsumableOfferData*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS]);
	}

	((DrxLobbyUIGetConsumableOffersCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEOFFERS_RESULTS], pOffers, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::GetConsumableAssets(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUIGetConsumableAssetsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_GetConsumableAssets, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];
		pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_REQUEST] = INVALID_WEBAPI_JOB_ID;
		pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS] = 0;

		FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
	}

	return error;
}

void CDrxPSNLobbyUI::StartGetConsumableAssets(DrxLobbyUITaskID uiTaskID)
{
	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
}

void CDrxPSNLobbyUI::TickGetConsumableAssets(DrxLobbyUITaskID uiTaskID)
{
	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_REQUEST] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::GetEntitlementList, NULL);
		if (pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_REQUEST] != INVALID_WEBAPI_JOB_ID)
		{
			StartSubTask(eST_WaitingForConsumableAssetsCallback, uiTaskID);
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_InternalError);
			StopTaskRunning(uiTaskID);
		}
	}
}

void CDrxPSNLobbyUI::EventWebApiGetConsumableAssets(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data)
{
	STask* pTask = &m_task[uiTaskID];
	pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS] = 0;

	if (data.m_webApiEvent.m_error == PSN_OK)
	{
		if (data.m_webApiEvent.m_pResponseBody)
		{
			if (data.m_webApiEvent.m_pResponseBody->eType == SDrxPSNWebApiResponseBody::E_JSON)
			{
				const sce::Json::Value& num_entitlements = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["total_results"];
				if (num_entitlements.getType() == sce::Json::kValueTypeUInteger)
				{
					uint32 entitlementCount = num_entitlements.getUInteger();
					if (entitlementCount > 0)
					{
						const sce::Json::Value& entitlements = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["entitlements"];
						if (entitlements.getType() == sce::Json::kValueTypeArray)
						{
							const sce::Json::Array& entitlementArray = entitlements.getArray();
							uint32 numAssets = entitlementArray.size();

							EDrxLobbyError error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS, NULL, sizeof(SDrxLobbyConsumableAssetData) * numAssets);
							if (error == eCLE_Success)
							{
								SDrxLobbyConsumableAssetData* pAssets = (SDrxLobbyConsumableAssetData*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS]);

								for (uint32 i = 0; i < numAssets; ++i)
								{
									SDrxLobbyConsumableAssetData* pCurrentAsset = &pAssets[pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS]];
									new(pCurrentAsset) SDrxLobbyConsumableAssetData();

									const sce::Json::Value& assetObj = entitlements[i];
									if (assetObj.getType() == sce::Json::kValueTypeObject)
									{
										const sce::Json::Value& assetConsumable = assetObj["is_consumable"];
										if (assetConsumable.getType() == sce::Json::kValueTypeBoolean)
										{
											if (assetConsumable.getBoolean() == true)
											{
												const sce::Json::Value& assetlabel = assetObj["id"];
												if (assetlabel.getType() == sce::Json::kValueTypeString)
												{
													pCurrentAsset->assetID = assetlabel.getString().c_str();
												}
												const sce::Json::Value& assetUseLimit = assetObj["use_limit"];
												if (assetUseLimit.getType() == sce::Json::kValueTypeUInteger)
												{
													pCurrentAsset->assetQuantity = assetUseLimit.getUInteger();
												}

												pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS]++;
											}
										}
									}
								}
							}

							// All finished - success!
							UpdateTaskError(uiTaskID, eCLE_Success);
							StopTaskRunning(uiTaskID);
							return;
						}
					}
					// nothing in array
				}
				// not an array - fail.
			}
			// response data not json
		}
		// response error
	}

	// if we reach here, the json is bad, or we have failed for some uncaught reason
	UpdateTaskError(uiTaskID, eCLE_InternalError);
	StopTaskRunning(uiTaskID);
}

void CDrxPSNLobbyUI::EndGetConsumableAssets(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	SDrxLobbyConsumableAssetData* pAssets = NULL;

	if ((pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS] > 0) &&
	    (pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS] != TMemInvalidHdl))
	{
		pAssets = (SDrxLobbyConsumableAssetData*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS]);
	}

	((DrxLobbyUIGetConsumableAssetsCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->paramsNum[LOBBYUI_PARAM_GETCONSUMABLEASSETS_RESULTS], pAssets, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::ShowDownloadOffer(uint32 user, TStoreOfferID offerId, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_ShowDownloadOffer, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];

		error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_SHOWDOWNLOADOFFER_ID, NULL, (DRX_LOBBYUI_CONSUMABLE_OFFER_ID_LENGTH + 1 + DRX_LOBBYUI_CONSUMABLE_OFFER_SKU_LENGTH + 1));
		if (error == eCLE_Success)
		{
			char* pOfferString = (char*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_SHOWDOWNLOADOFFER_ID]);
			sprintf(pOfferString, "%s-%s", offerId.productId.c_str(), offerId.skuId.c_str());

			FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxPSNLobbyUI::StartShowDownloadOffer(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);
	if (ret == PSN_OK)
	{
		ret = sceNpCommerceDialogInitialize();
		if ((ret == PSN_OK) || (ret == SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED))
		{
			const char* pOffer = (const char*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_SHOWDOWNLOADOFFER_ID]);

			SceNpCommerceDialogParam param;
			sceNpCommerceDialogParamInitialize(&param);
			param.mode = SCE_NP_COMMERCE_DIALOG_MODE_CHECKOUT;
			param.userId = userId;
			param.targets = &pOffer;
			param.numTargets = 1;

			ret = sceNpCommerceDialogOpen(&param);
			if (ret == PSN_OK)
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			else
			{
				NetLog("sceNpCommerceDialogOpen failed. ret = 0x%x\n", ret);
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
		}
		else
		{
			NetLog("sceNpCommerceDialogInitialize failed. ret = 0x%x\n", ret);
			UpdateTaskError(uiTaskID, eCLE_InternalError);
			StopTaskRunning(uiTaskID);
		}
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_InternalError);
	}

	if (pTask->error != eCLE_Success)
	{
		sceNpCommerceDialogTerminate();
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickShowDownloadOffer(DrxLobbyUITaskID uiTaskID)
{
	int ret;
	STask* pTask = &m_task[uiTaskID];

	if (pTask->canceled)
	{
		sceNpCommerceDialogTerminate();
		UpdateTaskError(uiTaskID, eCLE_Success);
		StopTaskRunning(uiTaskID);
		return;
	}

	ret = sceNpCommerceDialogUpdateStatus();
	if (ret == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		// completed.
		SceNpCommerceDialogResult result;
		memset(&result, 0, sizeof(result));

		ret = sceNpCommerceDialogGetResult(&result);
		switch (ret)
		{
		case SCE_COMMON_DIALOG_RESULT_OK:
		case SCE_NP_COMMERCE_DIALOG_RESULT_PURCHASED:
			{
				UpdateTaskError(uiTaskID, eCLE_Success);
			}
			break;
		case SCE_COMMON_DIALOG_RESULT_USER_CANCELED:
			{
				UpdateTaskError(uiTaskID, eCLE_Cancelled);
			}
			break;
		default:
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
			}
			break;
		}

		sceNpCommerceDialogTerminate();

		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::EndShowDownloadOffer(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
}

EDrxLobbyError CDrxPSNLobbyUI::ConsumeAsset(uint32 user, TStoreAssetID assetID, uint32 quantity, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;

	EDrxLobbyError error = StartTask(eT_ConsumeAsset, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];
		pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_REQUEST] = INVALID_WEBAPI_JOB_ID;
		pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_RESULTS] = 0;

		error = CreateTaskParamMem(uiTaskID, LOBBYUI_PARAM_CONSUMEASSET_REQUEST, NULL, sizeof(SDrxPSNOrbisWebApiConsumeEntitlementInput));
		if (error == eCLE_Success)
		{
			SDrxPSNOrbisWebApiConsumeEntitlementInput* pData = (SDrxPSNOrbisWebApiConsumeEntitlementInput*)m_pLobby->MemGetPtr(pTask->paramsMem[LOBBYUI_PARAM_CONSUMEASSET_REQUEST]);
			new(pData) SDrxPSNOrbisWebApiConsumeEntitlementInput();

			pData->entitlementLabel = assetID;
			pData->consumption = quantity;

			FROM_GAME_TO_LOBBY(&CDrxPSNLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxPSNLobbyUI::StartConsumeAsset(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->paramsMem[LOBBYUI_PARAM_CONSUMEASSET_REQUEST] != TMemInvalidHdl)
	{
		m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	}
	else
	{
		UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
		StopTaskRunning(uiTaskID);
	}
}

void CDrxPSNLobbyUI::TickConsumeAsset(DrxLobbyUITaskID uiTaskID)
{
	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		STask* pTask = &m_task[uiTaskID];

		if (pTask->paramsMem[LOBBYUI_PARAM_CONSUMEASSET_REQUEST] != TMemInvalidHdl)
		{
			pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_REQUEST] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::ConsumeEntitlement, pTask->paramsMem[LOBBYUI_PARAM_CONSUMEASSET_REQUEST]);
			if (pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_REQUEST] != INVALID_WEBAPI_JOB_ID)
			{
				StartSubTask(eST_WaitingForConsumeAssetCallback, uiTaskID);
			}
			else
			{
				UpdateTaskError(uiTaskID, eCLE_InternalError);
				StopTaskRunning(uiTaskID);
			}
		}
		else
		{
			UpdateTaskError(uiTaskID, eCLE_OutOfMemory);
			StopTaskRunning(uiTaskID);
		}
	}
}

void CDrxPSNLobbyUI::EventWebApiConsumeAsset(DrxLobbyUITaskID uiTaskID, SDrxPSNSupportCallbackEventData& data)
{
	STask* pTask = &m_task[uiTaskID];
	pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_RESULTS] = 0;

	if (data.m_webApiEvent.m_error == PSN_OK)
	{
		if (data.m_webApiEvent.m_pResponseBody)
		{
			if (data.m_webApiEvent.m_pResponseBody->eType == SDrxPSNWebApiResponseBody::E_JSON)
			{
				const sce::Json::Value& root = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot;
				if (root.getType() == sce::Json::kValueTypeObject)
				{
					const sce::Json::Value& consumed = root["use_limit"];
					if (consumed.getType() == sce::Json::kValueTypeUInteger)
					{
						pTask->paramsNum[LOBBYUI_PARAM_CONSUMEASSET_RESULTS] = consumed.getUInteger();
					}

					// All finished - success!
					UpdateTaskError(uiTaskID, eCLE_Success);
					StopTaskRunning(uiTaskID);
					return;
				}
				// not an object - fail.
			}
			// response data not json
		}
		// response error
	}

	// if we reach here, the json is bad, or we have failed for some uncaught reason
	UpdateTaskError(uiTaskID, eCLE_InternalError);
	StopTaskRunning(uiTaskID);
}

void CDrxPSNLobbyUI::EndConsumeAsset(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	SDrxLobbyConsumableOfferData* pOffers = NULL;

	((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
}

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS
