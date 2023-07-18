// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxSteamReward.h
//  Version:     v1.00
//  Created:     21/01/2013 by MichielM.
//  Compilers:   Visual Studio.NET 2010
//  Описание: Handles reward/achievements for the Steam API
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#if !defined(__DRXSTEAMREWARD_H__)
#define __DRXSTEAMREWARD_H__

#if USE_STEAM

	#include "DrxReward.h"
	#include "DrxSteamMatchMaking.h"

	#define MAXIMUM_PENDING_ACHIEVEMENTS 8
	#define NUMBER_OF_ACHIEVEMENTS       5
	#define ACHIEVEMENT_NAME_LENGTH      128
	#define ACHIEVEMENT_DESC_LENGTH      256
	#define MAX_REWARD_TASKS             8
	#define REWARD_TASK_TIMEOUT          5.0f

	#define _ACH_ID(id, name) { "", name, id, # id, 0, 0 }

/*
   There can only be 1 player logged into steam on a single PC
   Therefore we don't have to worry about different users in achievements (in the same instance of launcher)
   So keeping 1 struct synced for the achievements is enough
 */

typedef uint32 DrxSteamRewardTaskId;

struct DrxSteamReward
{
	char        m_sDescription[ACHIEVEMENT_DESC_LENGTH];
	char        m_sName[ACHIEVEMENT_NAME_LENGTH];
	int         m_eAchievementID;
	const char* m_pchAchievementID;
	int         m_iIconImage;
	bool        m_bAchieved;
};

enum ETask
{
	eT_GetSteamData,
	eT_InsertAward,
	eT_ClearAwards,
};

class CDrxSteamReward : public CDrxReward
{
public:
	CDrxSteamReward(CDrxLobby* pLobby);
	virtual ~CDrxSteamReward(void);

	virtual EDrxRewardError Award(uint32 playerID, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg);

	virtual void            Tick(CTimeValue tv);

	EDrxLobbyError          Initialise();
	EDrxLobbyError          Terminate() { return eCLE_Success; }

protected:

	DRXSTEAM_CALLBACK(CDrxSteamReward, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	DRXSTEAM_CALLBACK(CDrxSteamReward, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	DRXSTEAM_CALLBACK(CDrxSteamReward, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

	void StartAward(uint32 awardID, DrxLobbyTaskID lTaskID, DrxRewardCallback cb, void* pCbArg);

	bool RequestStats();
	bool InternalAward(int achievementID);

	#if !defined(_RELEASE)
	bool ClearAwards(DrxSteamRewardTaskId taskId);
	#endif

	EDrxLobbyError StartTask(ETask task, DrxLobbyTaskID id, DrxSteamRewardTaskId& rewardTaskID);
	void           EndTask(DrxSteamRewardTaskId taskID);
	void           TickTasks(CTimeValue tv);

	//eT_GetSteamData
	EDrxLobbyError StartTaskGetSteamData();
	void           StartGetSteamData(DrxSteamRewardTaskId task);            // ask steam for it
	void           EndGetSteamData(DrxSteamRewardTaskId task, EDrxLobbyError error);

	//eT_InsertAward
	void StartAddAwardToPending(DrxSteamRewardTaskId task);
	void EndAddAwardToPending(DrxSteamRewardTaskId task, EDrxLobbyError error);

	#if !defined(_RELEASE)
	//eT_ClearAwards
	void StartClearAwards(DrxSteamRewardTaskId task);
	void EndClearAwards(DrxSteamRewardTaskId task, EDrxLobbyError error);
	#endif

protected:
	struct SPendingAchievement
	{
		DrxLobbyTaskID    lTaskID;
		DrxRewardCallback pCb;
		void*             pCbArg;
		uint32            achievementID;
	} m_pendingAchievements[MAXIMUM_PENDING_ACHIEVEMENTS];

	struct STask
	{
		CTimeValue        m_StartTime;
		DrxLobbyTaskID    lTaskID;
		ETask             m_eStartedTask;
		DrxRewardCallback pCb;
		void*             pCbArg;
		uint32            achievementID;
		EDrxLobbyError    error;
		bool              m_bFinished;
		bool              m_bUsed;
		bool              m_bRunning;
	};

	DrxSteamReward* m_achievements;
	int             m_iNumAchievements;
	bool            m_bInitialized;
	int             m_count;

	STask           m_tasks[MAX_REWARD_TASKS];

	uint32          m_taskQueue[MAX_REWARD_TASKS];           //store indices from the m_tasks queue (sorted version of m_tasks)

protected:
	void TriggerCallbackOnGameThread(STask task);

};

#endif // USE_STEAM
#endif // End [!defined(__DRXSTEAMREWARD_H__)]
