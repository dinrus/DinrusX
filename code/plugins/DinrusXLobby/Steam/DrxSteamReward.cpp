// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxSteamReward.cpp
//  Version:     v1.00
//  Created:     21/01/2013 by MichielM.
//  Компиляторы:   mingw-w64-clang-x86_64 2010
//  Описание: Handles reward/achievements for the Steam API
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DrxSteamReward.h"
#include "DrxLobby.h"

#if USE_STEAM

#include "steam/steam_api.h"

// Achievement array which will hold data about the achievements and their state
// Make sure the names in EAchievement match the API Name of the achievements

// Defining our achievements
enum EAchievements
{
	Achievement1 = 0,
	Achievement2,
	Achievement3,
	Achievement4,
	Achievement5,
};

// Create the achievement buffer
DrxSteamReward g_Achievements[] =
{
	_ACH_ID(Achievement1, "Achievement1"),
	_ACH_ID(Achievement2, "Achievement2"),
	_ACH_ID(Achievement3, "Achievement3"),
	_ACH_ID(Achievement4, "Achievement4"),
	_ACH_ID(Achievement5, "Achievement5"),
};

CDrxSteamReward::CDrxSteamReward(CDrxLobby* pLobby)
	: CDrxReward(pLobby)
	, m_count(0)
	, m_iNumAchievements(NUMBER_OF_ACHIEVEMENTS)
	, m_bInitialized(false)
	, m_CallbackUserStatsReceived(this, &CDrxSteamReward::OnUserStatsReceived)
	, m_CallbackUserStatsStored(this, &CDrxSteamReward::OnUserStatsStored)
	, m_CallbackAchievementStored(this, &CDrxSteamReward::OnAchievementStored)
{
	m_achievements = g_Achievements;

	m_status = eCRS_Initialising;

	// Reset tasks
	for (uint32 i = 0; i < MAX_REWARD_TASKS; i++)
	{
		m_tasks[i].m_bFinished = false;
		m_tasks[i].m_bUsed = false;
		m_tasks[i].lTaskID = 0;
		m_tasks[i].m_eStartedTask = eT_GetSteamData;
		m_tasks[i].pCb = NULL;
		m_tasks[i].pCbArg = NULL;
		m_tasks[i].achievementID = 0;
	}

	ZeroMemory(m_pendingAchievements, sizeof(m_pendingAchievements));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Callback containing all information about the achievements for this title (network thread)
//////  Small hack: Clear out achievements on initialization for developers
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::OnUserStatsReceived(UserStatsReceived_t* pParam)
{
	// we may get callbacks for other games' stats arriving, ignore them
	ISteamUtils* pSteamUtils = SteamUtils();
	ISteamUserStats* pSteamUserStats = SteamUserStats();
	if (!pSteamUtils || !pSteamUserStats)
	{
		return;
	}

	if (pSteamUtils->GetAppID() == pParam->m_nGameID)
	{
		if (k_EResultOK == pParam->m_eResult)
		{
			NetLog("[STEAM]: Received stats and achievements from Steam\n");

			// load achievements
			for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
			{
				DrxSteamReward& ach = m_achievements[iAch];

				pSteamUserStats->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
				drx_sprintf(ach.m_sName, "%s",
				            pSteamUserStats->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
				                                                             "name"));
				drx_sprintf(ach.m_sDescription, "%s",
				            pSteamUserStats->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
				                                                             "desc"));
			}

			EndTask(m_CallbackUserStatsReceived.m_taskID);

			// Set initialize and if first time, clear awards for development if necessary
			if (!m_bInitialized)
			{
				m_bInitialized = true;

				if (CLobbyCVars::Get().resetSteamAchievementsOnBoot != 0)
				{
	#if !defined(_RELEASE)
					//DEV ONLY - clear achievements so we can re-award
					DrxSteamRewardTaskId taskID;
					DrxLobbyTaskID lTaskId = m_pLobby->CreateTask();
					if (lTaskId != DrxLobbyInvalidTaskID)
					{
						StartTask(eT_ClearAwards, lTaskId, taskID);
					}
					else
					{
						NetLog("[STEAM]: Clear awards task could not be started");
					}
	#endif
				}
			}

		}
		else
		{
			NetLog("RequestStats - failed, %d\n", pParam->m_eResult);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Asks Steam API to award an achievement to the user
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDrxSteamReward::InternalAward(int achievementID)
{
	ISteamUserStats* pSteamUserStats = SteamUserStats();
	if (m_bInitialized && pSteamUserStats)
	{
		for (int i = 0; i < m_iNumAchievements; i++)
		{
			if (m_achievements[i].m_eAchievementID == achievementID)
			{
				pSteamUserStats->SetAchievement(m_achievements[i].m_pchAchievementID);
				return pSteamUserStats->StoreStats();
			}
		}
	}

	return false;
}

	#if !defined(_RELEASE)
////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Only to be used by developers to test achievements, clears out all achievements (run on network thread)
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDrxSteamReward::ClearAwards(DrxSteamRewardTaskId taskId)
{
	ISteamUserStats* pSteamUserStats = SteamUserStats();
	if (!pSteamUserStats)
	{
		return false;
	}
	if (m_bInitialized)
	{
		for (int i = 0; i < m_iNumAchievements; i++)
		{
			pSteamUserStats->ClearAchievement(m_achievements[i].m_pchAchievementID);
		}
	}

	m_CallbackUserStatsStored.m_taskID = taskId;

	return pSteamUserStats->StoreStats();
}
	#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Runs on network thread, means data has been changed (e.g clearAwards)
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::OnUserStatsStored(UserStatsStored_t* pParam)
{
	if (m_CallbackUserStatsStored.m_taskID != DrxLobbyInvalidTaskID && m_tasks[m_CallbackUserStatsStored.m_taskID].m_eStartedTask == eT_ClearAwards)
	{
		m_tasks[m_CallbackUserStatsStored.m_taskID].error = eCLE_Success;
		EndTask(m_CallbackUserStatsStored.m_taskID);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Runs on network thread, meaning an achievement was sucessfully stored
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::OnAchievementStored(UserAchievementStored_t* pParam)
{
	EndTask(m_CallbackAchievementStored.m_taskID);
	m_tasks[m_CallbackAchievementStored.m_taskID].error = eCLE_Success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Ask Steam to send us the achievement data
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDrxSteamReward::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	ISteamUser* pSteamUser = SteamUser();
	ISteamUserStats* pSteamUserStats = SteamUserStats();
	if (NULL == pSteamUserStats || NULL == pSteamUser)
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if (!pSteamUser->BLoggedOn())
	{
		return false;
	}

	// Request user stats.
	return pSteamUserStats->RequestCurrentStats();
}

CDrxSteamReward::~CDrxSteamReward(void)
{
	DRX_ASSERT_MESSAGE(m_count == 0, "DrxSteamReward: still have pending awards at destruction");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Called on game thread, does basic checking and tries to queue the achievement
////////////////////////////////////////////////////////////////////////////////////////////////////
EDrxRewardError CDrxSteamReward::Award(uint32 playerID, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxRewardError ret_error = eCRE_Failed;

	if (m_count < MAXIMUM_PENDING_ACHIEVEMENTS - 1)
	{
		DrxLobbyTaskID lobbyTaskID = m_pLobby->CreateTask();
		if (lobbyTaskID != DrxLobbyInvalidTaskID)
		{
			FROM_GAME_TO_LOBBY(&CDrxSteamReward::StartAward, this, (uint32)awardID, (uint32)lobbyTaskID, (DrxRewardCallback)cb, pCbArg);

			if (pTaskID)
			{
				*pTaskID = lobbyTaskID;
			}

			ret_error = eCRE_Queued;
		}
	}
	else
	{
		// Pending queue full... please try again later.
		ret_error = eCRE_Busy;
	}

	return ret_error;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Called from game thread, runs on network thread. queues an achievement for processing
/////   NB: Does not create the task yet (only reserved a spot by calling CreateTask on lobby)
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::StartAward(uint32 awardID, DrxLobbyTaskID lTaskID, DrxRewardCallback cb, void* pCbArg)
{
	if (m_count < MAXIMUM_PENDING_ACHIEVEMENTS - 1)
	{
		m_pendingAchievements[m_count].lTaskID = lTaskID;
		m_pendingAchievements[m_count].pCb = cb;
		m_pendingAchievements[m_count].pCbArg = pCbArg;
		m_pendingAchievements[m_count].achievementID = awardID;
		m_count++;
	}
	else
	{
		//we thought we had room but we didnt (can happen if 2 award calls are called quickly after each other and there was only 1 spot)
		NetLog("[STEAM]: Pending queue filled up while trying to add Award: %i to the pendingAchievements list", awardID);
		cb(lTaskID, 0, awardID, eCLE_TooManyTasks, false, pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Initialize the system by grabbing the data from steam, the task will set the system into Idle when done
////////////////////////////////////////////////////////////////////////////////////////////////////
EDrxLobbyError CDrxSteamReward::Initialise()
{
	EDrxLobbyError ret = StartTaskGetSteamData();
	if (ret == eCLE_Success)
	{
		m_status = eCRS_Initialising;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  in Idle the system will check for a pending achievement and kick off the award chain
//////  status Working means it is currently processing an achievement and updating the local data
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::Tick(CTimeValue tv)
{
	switch (m_status)
	{
	case eCRS_Idle:
		if (m_count > 0)
		{
			DrxSteamRewardTaskId rewardTaskId;
			StartTask(eT_InsertAward, m_pendingAchievements[0].lTaskID, rewardTaskId);
			m_status = eCRS_Working;
		}
		break;
	case eCRS_Working:
		break;
	case eCRS_Initialising:
	case eCRS_Uninitialised:
		NetLog("[STEAM]: Steam reward not initialized");
		break;
	}

	TickTasks(tv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Ticks the tasks, currently only checks for time-out
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::TickTasks(CTimeValue tv)
{
	for (uint32 i = 0; i < MAX_REWARD_TASKS; i++)
	{
		if (m_tasks[i].m_bUsed)
		{
			if (tv.GetDifferenceInSeconds(m_tasks[i].m_StartTime) > REWARD_TASK_TIMEOUT)
			{
				//time out
				m_tasks[i].error = eCLE_TimeOut;
				EndTask(i);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  Gets called on the GAME thread to trigger the user defined callback
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::TriggerCallbackOnGameThread(STask task)
{
	if (task.pCb)
	{
		task.pCb(task.lTaskID, 0, task.achievementID, task.error, false, task.pCbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  only to be called on the NETWORK thread, finishes a task and calls the callback on the GAME thread
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::EndTask(DrxSteamRewardTaskId taskID)
{
	STask task = m_tasks[taskID];

	TO_GAME_FROM_LOBBY(&CDrxSteamReward::TriggerCallbackOnGameThread, this, task);

	m_tasks[taskID].m_bUsed = false;
	m_pLobby->ReleaseTask(m_tasks[taskID].lTaskID);

	switch (task.m_eStartedTask)
	{
	case eT_GetSteamData:
		NetLog("[STEAM]: End GetSteamData task");
		EndGetSteamData(taskID, task.error);
		break;
	case eT_InsertAward:
		NetLog("[STEAM]: End InsertAward task");
		EndAddAwardToPending(taskID, task.error);
		break;
	#if !defined(_RELEASE)
	case eT_ClearAwards:
		NetLog("[STEAM]: End ClearAwards task");
		EndClearAwards(taskID, task.error);
		break;
	#endif
	}
	;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////// Only to be called on the network thread, starts a desired task
////////////////////////////////////////////////////////////////////////////////////////////////////
EDrxLobbyError CDrxSteamReward::StartTask(ETask task, DrxLobbyTaskID id, DrxSteamRewardTaskId& rewardTaskID)
{
	for (uint32 i = 0; i < MAX_REWARD_TASKS; i++)
	{
		if (!m_tasks[i].m_bUsed)
		{
			m_tasks[i].lTaskID = id;
			m_tasks[i].m_bUsed = true;
			m_tasks[i].m_StartTime = gEnv->pTimer->GetAsyncTime();
			m_tasks[i].m_eStartedTask = task;
			rewardTaskID = i;

			switch (task)
			{
			case eT_GetSteamData:
				NetLog("[STEAM]: Started GetSteamData task");
				StartGetSteamData(i);
				break;
			case eT_InsertAward:
				NetLog("[STEAM]: Started insert award task");
				StartAddAwardToPending(i);
				break;
	#if !defined(_RELEASE)
			case eT_ClearAwards:
				NetLog("[STEAM]: Started ClearAwards task");
				StartClearAwards(i);
				break;
	#endif
			default:
				return eCLE_InternalError;   // Task not found
			}
			;

			return eCLE_Success;   // task started
		}
	}
	return eCLE_SystemIsBusy;   // all reward tasks are taken
}

	#if !defined(_RELEASE)
////////////////////////////////////////////////////////////////////////////////////////////////////
//////  eT_ClearAwards
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::StartClearAwards(DrxSteamRewardTaskId task)
{
	m_status = eCRS_Working;
	ClearAwards(task);
}

// Clearing is done, refresh our local data with Steam data
void CDrxSteamReward::EndClearAwards(DrxSteamRewardTaskId task, EDrxLobbyError error)
{
	StartTaskGetSteamData();
}
	#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  eT_GetSteamData
////////////////////////////////////////////////////////////////////////////////////////////////////
EDrxLobbyError CDrxSteamReward::StartTaskGetSteamData()
{
	DrxSteamRewardTaskId id;
	DrxLobbyTaskID lobbyTaskId = m_pLobby->CreateTask();
	if (lobbyTaskId != DrxLobbyInvalidTaskID)
		return StartTask(eT_GetSteamData, lobbyTaskId, id);

	return eCLE_InternalError;
}

void CDrxSteamReward::StartGetSteamData(DrxSteamRewardTaskId task)
{
	m_CallbackUserStatsReceived.m_taskID = task;
	RequestStats();
}

void CDrxSteamReward::EndGetSteamData(DrxSteamRewardTaskId task, EDrxLobbyError error)
{
	//Data is up to date, ready for next pending award
	m_status = eCRS_Idle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  eT_InsertAward
////////////////////////////////////////////////////////////////////////////////////////////////////
void CDrxSteamReward::StartAddAwardToPending(DrxSteamRewardTaskId taskID)
{
	m_tasks[taskID].achievementID = m_pendingAchievements[0].achievementID;
	m_tasks[taskID].pCb = m_pendingAchievements[0].pCb;
	m_tasks[taskID].pCbArg = m_pendingAchievements[0].pCbArg;

	if (!InternalAward(m_tasks[taskID].achievementID))
	{
		m_tasks[taskID].error = eCLE_InternalError;
	}

	m_CallbackAchievementStored.m_taskID = taskID;
}

void CDrxSteamReward::EndAddAwardToPending(DrxSteamRewardTaskId task, EDrxLobbyError error)
{
	for (uint32 i = 0; i < m_iNumAchievements; i++)
	{
		if (m_pendingAchievements[i].lTaskID == m_tasks[m_CallbackAchievementStored.m_taskID].lTaskID)
		{
			memmove(&m_pendingAchievements[i], &m_pendingAchievements[i + 1], sizeof(SPendingAchievement) * (m_iNumAchievements - 1));
			m_count--;
			break;
		}
	}

	StartTaskGetSteamData();
}

#endif // USE_STEAM
