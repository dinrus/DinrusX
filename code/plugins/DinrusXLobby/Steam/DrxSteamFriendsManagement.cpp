// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamFriendsManagement.h
//  Created:     26/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxFriends
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "Steam/DrxSteamLobby.h" // DrxSteamFriends.h included here

#if USE_STEAM && USE_DRX_FRIENDS

////////////////////////////////////////////////////////////////////////////////

	#define FRIENDS_MANAGEMENT_PARAM_NUM_IDS         (0) // int
	#define FRIENDS_MANAGEMENT_PARAM_ID_LIST         (0) // ptr

	#define FRIENDS_MANAGEMENT_PARAM_MAX_RESULTS     (0) // int
	#define FRIENDS_MANAGEMENT_PARAM_SEARCH_INFO     (0) // ptr

	#define FRIENDS_MANAGEMENT_PARAM_RESULTS         (1) // ptr
	#define FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME (1) // ptr

////////////////////////////////////////////////////////////////////////////////

CDrxSteamFriendsManagement::CDrxSteamFriendsManagement(CDrxLobby* pLobby, CDrxLobbyService* pService)
	: CDrxFriendsManagement(pLobby, pService)
{
	// Make the CDrxFriendsManagement base pointers point to our data so we can use the common code in CDrxFriendsManagement
	for (uint32 index = 0; index < MAX_FRIENDS_MANAGEMENT_TASKS; ++index)
	{
		CDrxFriendsManagement::m_pTask[index] = &m_task[index];
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::Tick(CTimeValue tv)
{
	// Currently, no steam specific tasks need ticking, but leave this in case something crops up
	//for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
	//{
	//	STask* pTask = &m_task[i];

	//	if (pTask->used && pTask->running)
	//	{
	//		switch (pTask->subTask)
	//		{
	//		default:
	//			break;
	//		}
	//	}
	//}
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::InitialiseTask(ETask task, uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, void* pCb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	DrxFriendsTaskID fTaskID;
	EDrxLobbyError error = StartTask(task, false, user, &fTaskID, pTaskID, pCb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[fTaskID];
		pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS] = numUserIDs;

		if (numUserIDs > 0)
		{
			error = CreateTaskParamMem(fTaskID, FRIENDS_MANAGEMENT_PARAM_ID_LIST, NULL, numUserIDs * sizeof(CSteamID));

			if (error == eCLE_Success)
			{
				CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);

				for (uint32 index = 0; index < numUserIDs; index++)
				{
					if (pUserIDs[index].IsValid())
					{
						pSteamID[index] = ((const SDrxSteamUserID*)pUserIDs[index].get())->m_steamID;
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
			FROM_GAME_TO_LOBBY(&CDrxSteamFriendsManagement::StartTaskRunning, this, fTaskID);
		}
		else
		{
			FreeTask(fTaskID);
		}
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::StartTask(ETask task, bool startRunning, uint32 user, DrxFriendsManagementTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg)
{
	DrxFriendsTaskID tmpFTaskID;
	DrxFriendsTaskID* pUseFTaskID = pFTaskID ? pFTaskID : &tmpFTaskID;
	EDrxLobbyError error = CDrxFriendsManagement::StartTask(task, startRunning, pUseFTaskID, pLTaskID, pCb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[*pUseFTaskID];
		pTask->m_timeStarted = g_time;
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartTaskRunning(DrxFriendsManagementTaskID fTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_FriendsManagementIsUserFriend:
			StartFriendsManagementIsUserFriend(fTaskID);
			break;
		case eT_FriendsManagementIsUserBlocked:
			StartFriendsManagementIsUserBlocked(fTaskID);
			break;
		case eT_FriendsManagementAcceptInvite:
			StartFriendsManagementAcceptInvite(fTaskID);
			break;
		case eT_FriendsManagementGetName:
			StartFriendsManagementGetName(fTaskID);
			break;
		case eT_FriendsManagementGetStatus:
			StartFriendsManagementGetStatus(fTaskID);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::EndTask(DrxFriendsManagementTaskID fTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		if (pTask->pCb)
		{
			switch (pTask->startedTask)
			{
			case eT_FriendsManagementIsUserFriend:
				EndFriendsManagementIsUserFriend(fTaskID);
				break;

			case eT_FriendsManagementIsUserBlocked:
				EndFriendsManagementIsUserBlocked(fTaskID);
				break;

			case eT_FriendsManagementAcceptInvite:        // intentional fall-through
				((DrxFriendsManagementCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
				break;

			case eT_FriendsManagementGetName:
				EndFriendsManagementGetName(fTaskID);
				break;

			case eT_FriendsManagementGetStatus:
				EndFriendsManagementGetStatus(fTaskID);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] CDrxSteamFriendsManagement::EndTask() %d (%d) Result %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		FreeTask(fTaskID);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StopTaskRunning(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxSteamFriendsManagement::EndTask, this, fTaskID);
	}
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementSendFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementAcceptFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementRejectFriendRequest(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementRevokeFriendStatus(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementIsUserFriend(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementInfoCallback pCb, void* pCbArg)
{
	EDrxLobbyError error = InitialiseTask(eT_FriendsManagementIsUserFriend, user, pUserIDs, numUserIDs, pTaskID, (void*)pCb, pCbArg);
	NetLog("[STEAM] CDrxSteamFriendsManagement::FriendsManagementIsUserFriend(), result %d", error);
	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementFindUser(uint32 user, SFriendManagementSearchInfo* pUserName, uint32 maxResults, DrxLobbyTaskID* pTaskID, DrxFriendsManagementSearchCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementBlockUser(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementUnblockUser(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	return eCLE_ServiceNotSupported;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementIsUserBlocked(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementInfoCallback pCb, void* pCbArg)
{
	EDrxLobbyError error = InitialiseTask(eT_FriendsManagementIsUserBlocked, user, pUserIDs, numUserIDs, pTaskID, (void*)pCb, pCbArg);
	NetLog("[STEAM] CDrxSteamFriendsManagement::FriendsManagementIsBlocked(), result %d", error);
	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementAcceptInvite(uint32 user, DrxUserID* pUserID, DrxLobbyTaskID* pTaskID, DrxFriendsManagementCallback pCb, void* pCbArg)
{
	EDrxLobbyError error = InitialiseTask(eT_FriendsManagementAcceptInvite, user, pUserID, 1, pTaskID, (void*)pCb, pCbArg);
	NetLog("[STEAM] CDrxSteamFriendsManagement::FriendsManagementAcceptInvite(), result %d", error);
	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementGetName(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementSearchCallback pCb, void* pCbArg)
{
	EDrxLobbyError error = InitialiseTask(eT_FriendsManagementGetName, user, pUserIDs, numUserIDs, pTaskID, (void*)pCb, pCbArg);
	NetLog("[STEAM] CDrxSteamFriendsManagement::FriendsManagementGetName(), result %d", error);
	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamFriendsManagement::FriendsManagementGetStatus(uint32 user, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxFriendsManagementStatusCallback pCb, void* pCbArg)
{
	EDrxLobbyError error = InitialiseTask(eT_FriendsManagementGetStatus, user, pUserIDs, numUserIDs, pTaskID, (void*)pCb, pCbArg);
	NetLog("[STEAM] CDrxSteamFriendsManagement::FriendsManagementGetStatus(), result %d", error);
	return error;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartFriendsManagementIsUserFriend(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
	EDrxLobbyError error;
	ISteamFriends* pSteamFriends = SteamFriends();
		
	if (pSteamFriends)
	{
		error = CreateTaskParamMem(fTaskID, FRIENDS_MANAGEMENT_PARAM_RESULTS, NULL, numUserIDs * sizeof(bool));
	}
	else
	{
		error = eCLE_SteamInitFailed;
	}

	if (error == eCLE_Success)
	{
		bool* pFriend = (bool*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS]);

		for (uint32 index = 0; index < numUserIDs; index++)
		{
			switch (pSteamFriends->GetFriendRelationship(pSteamID[index]))
			{
			case k_EFriendRelationshipFriend:
				pFriend[index] = true;
				break;

			case k_EFriendRelationshipNone:               // Intentional fall-through
			case k_EFriendRelationshipBlocked:            // Intentional fall-through
			case k_EFriendRelationshipRequestRecipient:   // Intentional fall-through
			case k_EFriendRelationshipRequestInitiator:   // Intentional fall-through
			case k_EFriendRelationshipIgnored:            // Intentional fall-through
			case k_EFriendRelationshipIgnoredFriend:      // Intentional fall-through
			default:
				pFriend[index] = false;
				break;
			}
		}
	}
	else
	{
		UpdateTaskError(fTaskID, error);
	}

	StopTaskRunning(fTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::EndFriendsManagementIsUserFriend(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	bool* pFriend = (bool*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];

	TMemHdl memHdl = m_pLobby->MemAlloc(numUserIDs * sizeof(SFriendManagementInfo));
	SFriendManagementInfo* pResults = (SFriendManagementInfo*)m_pLobby->MemGetPtr(memHdl);

	if (pResults != NULL)
	{
		for (uint32 index = 0; index < numUserIDs; ++index)
		{
			pResults[index].result = pFriend[index];
			pResults[index].userID = new SDrxSteamUserID(pSteamID[index]);

			if (pResults[index].userID == NULL)
			{
				pTask->error = eCLE_OutOfMemory;
				break;
			}
		}
	}
	else
	{
		pTask->error = eCLE_OutOfMemory;
	}

	if (pTask->error == eCLE_Success)
	{
		((DrxFriendsManagementInfoCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pResults, numUserIDs, pTask->pCbArg);
	}
	else
	{
		((DrxFriendsManagementInfoCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartFriendsManagementIsUserBlocked(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
	EDrxLobbyError error;
	ISteamFriends* pSteamFriends = SteamFriends();

	if (pSteamFriends)
	{
		error = CreateTaskParamMem(fTaskID, FRIENDS_MANAGEMENT_PARAM_RESULTS, NULL, numUserIDs * sizeof(bool));
	}
	else
	{
		error = eCLE_SteamInitFailed;
	}

	if (error == eCLE_Success)
	{
		bool* pFriend = (bool*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS]);

		for (uint32 index = 0; index < numUserIDs; index++)
		{
			switch (pSteamFriends->GetFriendRelationship(pSteamID[index]))
			{
			case k_EFriendRelationshipBlocked:
				pFriend[index] = true;
				break;

			case k_EFriendRelationshipNone:               // Intentional fall-through
			case k_EFriendRelationshipRequestRecipient:   // Intentional fall-through
			case k_EFriendRelationshipFriend:             // Intentional fall-through
			case k_EFriendRelationshipRequestInitiator:   // Intentional fall-through
			case k_EFriendRelationshipIgnored:            // Intentional fall-through
			case k_EFriendRelationshipIgnoredFriend:      // Intentional fall-through
			default:
				pFriend[index] = false;
				break;
			}
		}
	}
	else
	{
		UpdateTaskError(fTaskID, error);
	}

	StopTaskRunning(fTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::EndFriendsManagementIsUserBlocked(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	bool* pFriend = (bool*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];

	TMemHdl memHdl = m_pLobby->MemAlloc(numUserIDs * sizeof(SFriendManagementInfo));
	SFriendManagementInfo* pResults = (SFriendManagementInfo*)m_pLobby->MemGetPtr(memHdl);

	if (pResults != NULL)
	{
		for (uint32 index = 0; index < numUserIDs; ++index)
		{
			pResults[index].result = pFriend[index];
			pResults[index].userID = new SDrxSteamUserID(pSteamID[index]);

			if (pResults[index].userID == NULL)
			{
				pTask->error = eCLE_OutOfMemory;
				break;
			}
		}
	}
	else
	{
		pTask->error = eCLE_OutOfMemory;
	}

	if (pTask->error == eCLE_Success)
	{
		((DrxFriendsManagementInfoCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pResults, numUserIDs, pTask->pCbArg);
	}
	else
	{
		((DrxFriendsManagementInfoCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartFriendsManagementAcceptInvite(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);

	SDrxSteamSessionID* pSessionID = new SDrxSteamSessionID(*pSteamID, true);
	CDrxSteamLobbyService* pSteamLobbyService = static_cast<CDrxSteamLobbyService*>(m_pLobby->GetLobbyService(eCLS_Online));

	TO_GAME_FROM_LOBBY(&CDrxSteamLobbyService::InviteAccepted, pSteamLobbyService, uint32(0), DrxSessionID(pSessionID));

	NetLog("[STEAM]: CDrxSteamFriendsManagement::StartFriendsManagementAcceptInvite(): invite accepted");
	StopTaskRunning(fTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartFriendsManagementGetName(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
	ISteamFriends* pSteamFriends = SteamFriends();
	if (pSteamFriends)
	{
		UpdateTaskError(fTaskID, CreateTaskParamMem(fTaskID, FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME, NULL, numUserIDs * sizeof(SFriendInfo)));
		SFriendInfo* pResults = (SFriendInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME]);

		if (pResults != NULL)
		{
			memset(pResults, 0, numUserIDs * sizeof(SFriendInfo));

			for (uint32 index = 0; index < numUserIDs; ++index)
			{
				pResults[index].userID = new SDrxSteamUserID(pSteamID[index]);
				if (pResults[index].userID == NULL)
				{
					pTask->error = eCLE_OutOfMemory;
					break;
				}

				drx_strcpy(pResults[index].name, pSteamFriends->GetFriendPersonaName(pSteamID[index]));
			}
		}
	}
	else
	{
		UpdateTaskError(fTaskID, eCLE_SteamInitFailed);
	}

	StopTaskRunning(fTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::EndFriendsManagementGetName(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	if (pTask->error == eCLE_Success)
	{
		uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
		SFriendInfo* pResults = (SFriendInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME]);

		((DrxFriendsManagementSearchCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pResults, numUserIDs, pTask->pCbArg);
	}
	else
	{
		((DrxFriendsManagementSearchCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::StartFriendsManagementGetStatus(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];
	CSteamID* pSteamID = (CSteamID*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_ID_LIST]);
	uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
	ISteamFriends* pSteamFriends = SteamFriends();
	if (pSteamFriends)
	{
		UpdateTaskError(fTaskID, CreateTaskParamMem(fTaskID, FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME, NULL, numUserIDs * sizeof(SFriendStatusInfo)));
		SFriendStatusInfo* pResults = (SFriendStatusInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME]);

		if (pResults != NULL)
		{
			memset(pResults, 0, numUserIDs * sizeof(SFriendStatusInfo));

			for (uint32 index = 0; index < numUserIDs; ++index)
			{
				pResults[index].userID = new SDrxSteamUserID(pSteamID[index]);
				if (pResults[index].userID == NULL)
				{
					pTask->error = eCLE_OutOfMemory;
					break;
				}

				switch (pSteamFriends->GetFriendPersonaState(pSteamID[index]))
				{
				case k_EPersonaStateOffline:        // friend is not currently logged on
					pResults[index].status = eFMS_Offline;
					break;
				case k_EPersonaStateLookingToTrade: // Online, trading
				case k_EPersonaStateLookingToPlay:  // Online, wanting to play
				case k_EPersonaStateOnline:         // friend is logged on
					pResults[index].status = eFMS_Online;
					break;
				case k_EPersonaStateBusy:           // user is on, but busy
				case k_EPersonaStateAway:           // auto-away feature
				case k_EPersonaStateSnooze:         // auto-away for a long time
					pResults[index].status = eFMS_Away;
					break;
				}
			}
		}
	}
	else
	{
		UpdateTaskError(fTaskID, eCLE_SteamInitFailed);
	}

	StopTaskRunning(fTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamFriendsManagement::EndFriendsManagementGetStatus(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = &m_task[fTaskID];

	if (pTask->error == eCLE_Success)
	{
		uint32 numUserIDs = pTask->paramsNum[FRIENDS_MANAGEMENT_PARAM_NUM_IDS];
		SFriendStatusInfo* pResults = (SFriendStatusInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[FRIENDS_MANAGEMENT_PARAM_RESULTS_TO_GAME]);

		((DrxFriendsManagementStatusCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pResults, numUserIDs, pTask->pCbArg);
	}
	else
	{
		((DrxFriendsManagementStatusCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_FRIENDS
