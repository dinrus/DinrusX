// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"
	#include "DrxPSN2Response.h"
	#include "DrxPSN2Friends.h"
	#include "DrxPSN2WebApi.h"
	#include "DrxPSN2WebApi_FriendsList.h"

	#if USE_PSN
		#if USE_DRX_FRIENDS

			#define FRIENDS_RESULTS_LIST        0     // mem + count num
			#define FRIENDS_WEBAPI_REQUEST_SLOT 1     // mem + jobid num

void CDrxPSNFriends::ProcessWebApiEvent(SDrxPSNSupportCallbackEventData& data)
{
	for (uint32 i = 0; i < MAX_FRIENDS_TASKS; i++)
	{
		STask* pTask = &m_task[i];
		if (pTask->used && pTask->running)
		{
			switch (pTask->subTask)
			{
			case eST_WaitingForFriendsListWebApiCallback:
				{
					if (pTask->paramsNum[FRIENDS_WEBAPI_REQUEST_SLOT] == data.m_webApiEvent.m_id)
					{
						EventWebApiGetFriends(i, data);
						data.m_webApiEvent.m_returnFlags |= SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED;
					}
				}
				break;
			}
		}
	}
}

void CDrxPSNFriends::SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg)
{
	CDrxPSNFriends* _this = static_cast<CDrxPSNFriends*>(pArg);

	switch (ecb)
	{
	case eCE_ErrorEvent:
		{
			for (uint32 i = 0; i < MAX_FRIENDS_TASKS; i++)
			{
				STask* pTask = &_this->m_task[i];

				if (pTask->used && pTask->running)
				{
					_this->UpdateTaskError(i, MapSupportErrorToLobbyError(data.m_errorEvent.m_error));
				}
			}
		}
		break;
	case eCE_WebApiEvent:
		{
			_this->ProcessWebApiEvent(data);
		}
		break;
	}
}

CDrxPSNFriends::CDrxPSNFriends(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport) : CDrxFriends(pLobby, pService)
{
	m_pPSNSupport = pSupport;

	// Make the CDrxLobbyUI base pointers point to our data so we can use the common code in CDrxLobbyUI
	for (uint32 i = 0; i < MAX_FRIENDS_TASKS; i++)
	{
		CDrxFriends::m_pTask[i] = &m_task[i];
	}
}

EDrxLobbyError CDrxPSNFriends::StartTask(ETask etask, bool startRunning, uint32 user, DrxFriendsTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg)
{
	DrxFriendsTaskID tmpFTaskID;
	DrxFriendsTaskID* pUseFTaskID = pFTaskID ? pFTaskID : &tmpFTaskID;
	EDrxLobbyError error = CDrxFriends::StartTask(etask, startRunning, pUseFTaskID, pLTaskID, h, pCb, pCbArg);
	return error;
}

void CDrxPSNFriends::StartSubTask(ETask etask, DrxFriendsTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	pTask->subTask = etask;
}

void CDrxPSNFriends::StartTaskRunning(DrxFriendsTaskID fTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_FriendsGetFriendsList:
			StartFriendsGetFriendsList(fTaskID);
			break;

		case eT_FriendsSendGameInvite:
			StopTaskRunning(fTaskID);
			break;
		}
	}
}

void CDrxPSNFriends::EndTask(DrxFriendsTaskID fTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		if (pTask->pCb)
		{
			switch (pTask->startedTask)
			{
			case eT_FriendsGetFriendsList:
				EndFriendsGetFriendsList(fTaskID);
				break;

			case eT_FriendsSendGameInvite:
				((DrxFriendsCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] Friends EndTask %d (%d) Result %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		FreeTask(fTaskID);
	}
}

void CDrxPSNFriends::StopTaskRunning(DrxFriendsTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		pTask->running = false;
		TO_GAME_FROM_LOBBY(&CDrxPSNFriends::EndTask, this, fTaskID);
	}
}

EDrxLobbyError CDrxPSNFriends::FriendsGetFriendsList(uint32 user, uint32 start, uint32 num, DrxLobbyTaskID* pTaskID, DrxFriendsGetFriendsListCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxFriendsTaskID fTaskID;

	EDrxLobbyError error = StartTask(eT_FriendsGetFriendsList, false, user, &fTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	if (error == eCLE_Success)
	{
		UpdateTaskError(fTaskID, CreateTaskParamMem(fTaskID, FRIENDS_WEBAPI_REQUEST_SLOT, NULL, sizeof(SDrxPSNOrbisWebApiGetFriendsListInput)));
		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[fTaskID];
			SDrxPSNOrbisWebApiGetFriendsListInput* pFriendsInput = (SDrxPSNOrbisWebApiGetFriendsListInput*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_WEBAPI_REQUEST_SLOT]);
			pFriendsInput->startIndex = start;
			pFriendsInput->count = num;

			FROM_GAME_TO_LOBBY(&CDrxPSNFriends::StartTaskRunning, this, fTaskID);
		}
		else
		{
			FreeTask(fTaskID);
		}
	}
	return error;
}

void CDrxPSNFriends::StartFriendsGetFriendsList(DrxFriendsTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	SDrxPSNOrbisWebApiGetFriendsListInput* pFriendsInput = (SDrxPSNOrbisWebApiGetFriendsListInput*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_WEBAPI_REQUEST_SLOT]);
	if (pFriendsInput)
	{
		memcpy(&pFriendsInput->id, &m_pPSNSupport->GetLocalNpId()->handle, sizeof(SceNpOnlineId));

		UpdateTaskError(fTaskID, CreateTaskParamMem(fTaskID, FRIENDS_RESULTS_LIST, NULL, pFriendsInput->count * sizeof(SFriendInfo)));
		if (pTask->error == eCLE_Success)
		{
			StartSubTask(eST_WaitingForFriendsListWebApiCallback, fTaskID);

			SFriendInfo* pFriends = (SFriendInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_RESULTS_LIST]);
			memset(pFriends, 0, pFriendsInput->count * sizeof(SFriendInfo));
			pTask->paramsNum[FRIENDS_RESULTS_LIST] = 0;

			pTask->paramsNum[FRIENDS_WEBAPI_REQUEST_SLOT] = m_pPSNSupport->GetWebApiInterface().AddJob(CDrxPSNOrbisWebApiThread::GetFriendsList, pTask->paramsMem[FRIENDS_WEBAPI_REQUEST_SLOT]);
			if (pTask->paramsNum[FRIENDS_WEBAPI_REQUEST_SLOT] == INVALID_WEBAPI_JOB_ID)
			{
				// failed
				UpdateTaskError(fTaskID, eCLE_InternalError);
			}
		}
		else
		{
			//-- Out of memory
			UpdateTaskError(fTaskID, eCLE_OutOfMemory);
		}
	}
	else
	{
		//-- Out of memory
		UpdateTaskError(fTaskID, eCLE_OutOfMemory);
	}

	if (pTask->error != eCLE_Success)
	{
		StopTaskRunning(fTaskID);
	}
}

void CDrxPSNFriends::EventWebApiGetFriends(DrxFriendsTaskID fTaskID, SDrxPSNSupportCallbackEventData& data)
{
	// parse json response to array
	if (data.m_webApiEvent.m_error == PSN_OK)
	{
		if (data.m_webApiEvent.m_pResponseBody)
		{
			if (data.m_webApiEvent.m_pResponseBody->eType == SDrxPSNWebApiResponseBody::E_JSON)
			{
				// parse json to get details
				// CDrxPSNWebApiJobController::PrintResponseJSONTree(data.m_webApiEvent.m_pResponseBody);

				STask* pTask = &m_task[fTaskID];
				SDrxPSNOrbisWebApiGetFriendsListInput* pFriendsInput = (SDrxPSNOrbisWebApiGetFriendsListInput*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_WEBAPI_REQUEST_SLOT]);
				SFriendInfo* pFriends = (SFriendInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_RESULTS_LIST]);

				const sce::Json::Value& numTotalFriends = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["totalResults"];
				if (numTotalFriends.getType() == sce::Json::kValueTypeUInteger)
				{
					uint32 nTotal = numTotalFriends.getUInteger();
					const sce::Json::Value& numReturnedFriends = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["size"];
					if (numReturnedFriends.getType() == sce::Json::kValueTypeUInteger)
					{
						const sce::Json::Value& returnedFriendsArray = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["friendList"];
						if (returnedFriendsArray.getType() == sce::Json::kValueTypeArray)
						{
							uint32 nReturned = numTotalFriends.getUInteger();
							const sce::Json::Value& numReturnedOffset = data.m_webApiEvent.m_pResponseBody->jsonTreeRoot["start"];
							if (numReturnedOffset.getType() == sce::Json::kValueTypeUInteger)
							{
								uint32 nOffset = numReturnedOffset.getUInteger();

								// sanity tests.
								// 1. The offset returned should be within the range of our initial request
								if ((nOffset < pFriendsInput->startIndex) || (nOffset >= (pFriendsInput->startIndex + pFriendsInput->count)))
								{
									// offset out of bounds
									UpdateTaskError(fTaskID, eCLE_InvalidRequest);
									StopTaskRunning(fTaskID);
									return;
								}

								// 2. size returned should be <= size requested.
								if (nReturned > pFriendsInput->count)
								{
									// returned count is more than the requested range. This shouldn't happen!
									UpdateTaskError(fTaskID, eCLE_InvalidRequest);
									StopTaskRunning(fTaskID);
									return;
								}

								// if size returned is 0, we're done!
								if (nReturned == 0)
								{
									UpdateTaskError(fTaskID, eCLE_Success);
									StopTaskRunning(fTaskID);
									return;
								}

								// copy friends
								for (uint32 i = 0; i < nReturned; i++)
								{
									SFriendInfo& friendr = pFriends[pTask->paramsNum[FRIENDS_RESULTS_LIST]];
									const sce::Json::Value& friendObj = returnedFriendsArray[i];
									if (friendObj.getType() == sce::Json::kValueTypeObject)
									{
										SDrxPSNUserID* pId = new SDrxPSNUserID;
										if (pId)
										{
											friendr.status = eLFS_Offline;

											sceNpWebApiUtilityParseNpId(friendObj["npId"].toString().c_str(), &pId->npId);
											friendr.userID = pId;

											drx_strcpy(friendr.name, friendObj["onlineId"].toString().c_str());

											const sce::Json::Value& pres = friendObj["presence"];
											if (pres.getType() == sce::Json::kValueTypeObject)
											{
												const sce::Json::Value& pripres = pres["primaryInfo"];
												if (pripres.getType() == sce::Json::kValueTypeObject)
												{
													const sce::Json::Value& status = pripres["gameStatus"];
													if (status.getType() == sce::Json::kValueTypeString)
													{
														drx_strcpy(friendr.presence, status.toString().c_str());
													}

													const sce::Json::Value& onlineStatus = pripres["onlineStatus"];
													if (!strcmp(onlineStatus.toString().c_str(), "offline"))
													{
														friendr.status = eLFS_Offline;
													}
													else if (!strcmp(onlineStatus.toString().c_str(), "standby"))
													{
														// treating standby mode as offline too
														friendr.status = eLFS_Offline;
													}
													else if (!strcmp(onlineStatus.toString().c_str(), "online"))
													{
														// there doesn't seem to be a way to distinguish between online-in-context and online-out-of-context.
														// The gameStatus is supposed to be invalid if not in the same NP communication id, so we'll try that.
														if (status == sce::Json::kValueTypeNull)
														{
															// friend not in same game
															friendr.status = eLFS_Online;
														}
														else
														{
															// friend is in same game.
															friendr.status = eLFS_OnlineSameTitle;
														}
													}
												}
											}

											pTask->paramsNum[FRIENDS_RESULTS_LIST]++;
										}
									}
									else
									{
										UpdateTaskError(fTaskID, eCLE_OutOfMemory);
										StopTaskRunning(fTaskID);
										return;
									}
								}

								// Are we finished?
								if ((pTask->paramsNum[FRIENDS_RESULTS_LIST] == pFriendsInput->count)
								    || ((nTotal < pFriendsInput->count) && (nReturned == nTotal))
								    || ((pFriendsInput->startIndex + pTask->paramsNum[FRIENDS_RESULTS_LIST]) >= nTotal))
								{
									// got all the friends we requested, so finished
									UpdateTaskError(fTaskID, eCLE_Success);
									StopTaskRunning(fTaskID);
									return;
								}

								// if we're here, we are still expecting more results to come back in a future event, so task is still running.
								// return now
								return;
							}
						}
					}
				}
			}
		}
	}

	// if we reach here, the json is bad, or we have failed for some uncaught reason
	UpdateTaskError(fTaskID, eCLE_InternalError);
	StopTaskRunning(fTaskID);
}

void CDrxPSNFriends::EndFriendsGetFriendsList(DrxFriendsTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	if (pTask->paramsMem[FRIENDS_RESULTS_LIST] != TMemInvalidHdl)
	{
		SFriendInfo* pFriends = (SFriendInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_RESULTS_LIST]);

		if (pTask->error == eCLE_Success)
		{
			((DrxFriendsGetFriendsListCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pFriends, pTask->paramsNum[FRIENDS_RESULTS_LIST], pTask->pCbArg);
		}
		else
		{
			((DrxFriendsGetFriendsListCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
		}

		//-- remove our references to the user Ids
		for (uint32 i = 0; i < pTask->paramsNum[FRIENDS_RESULTS_LIST]; i++)
		{
			pFriends[i].userID = NULL;
		}
	}
	else
	{
		((DrxFriendsGetFriendsListCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

void CDrxPSNFriends::LobbyUIGameInviteCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg)
{
	/*
	   CDrxPSNFriends* _this = static_cast<CDrxPSNFriends*>(pArg);

	   for (uint32 i = 0; i < MAX_FRIENDS_TASKS; i++)
	   {
	    STask* pTask = &_this->m_task[i];
	    if (pTask->used && pTask->running && (pTask->subTask == eT_FriendsSendGameInvite))
	    {
	      if (pTask->lTaskID == taskID)
	      {
	        //-- us?
	        _this->UpdateTaskError(i, error);
	        _this->StopTaskRunning(i);
	        return;
	      }
	    }
	   }
	 */
}

EDrxLobbyError CDrxPSNFriends::FriendsSendGameInvite(uint32 user, DrxSessionHandle h, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	/*
	   EDrxLobbyError error = eCLE_Success;

	   if (pUserIDs && (numUserIDs > 0))
	   {
	    DrxFriendsTaskID fTaskID;

	    error = StartTask(eT_FriendsSendGameInvite, false, user, &fTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
	    if (error == eCLE_Success)
	    {
	      error = m_pLobby->GetLobbyUI()->ShowGameInvite(user, h, pUserIDs, numUserIDs, pTaskID, LobbyUIGameInviteCallback, this);
	      if (error == eCLE_Success)
	      {
	        FROM_GAME_TO_LOBBY(&CDrxPSNFriends::StartTaskRunning, this, fTaskID);
	      }
	      else
	      {
	        FreeTask(fTaskID);
	      }
	    }
	   }
	   else
	   {
	    error = eCLE_InvalidUser;
	   }

	   return error;
	 */
	return eCLE_ServiceNotSupported;
}

		#endif // USE_DRX_FRIENDS
	#endif   // USE_PSN
#endif     // DRX_PLATFORM_ORBIS
