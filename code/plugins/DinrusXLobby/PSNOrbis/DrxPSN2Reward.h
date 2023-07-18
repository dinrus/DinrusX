// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_REWARD_H__
#define __DRXPSN2_REWARD_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		<DinrusX/CoreX/Platform/IPlatformOS.h>
		#include "DrxReward.h"
		<DinrusX/np.h>

		#define MAXIMUM_PENDING_TROPHIES (8)
		#define NUM_TROPHY_USERS         1

class CDrxPSNReward : public CDrxReward
{

public:
	CDrxPSNReward(CDrxLobby* pLobby);
	virtual ~CDrxPSNReward(void);

	virtual void            Tick(CTimeValue tv);

	EDrxLobbyError          Initialise();
	EDrxLobbyError          Terminate();

	virtual EDrxRewardError Award(uint32 user, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg);

	static EDrxLobbyError   RegisterTrophies(uint32 user, void* pArg);

protected:

	struct SRegisterTrophiesData
	{
		int error;
	};

	struct SAwardTrophyData
	{
		SceNpTrophyId awardedTrophyId;
		SceNpTrophyId platinumTrophyId;
		int           error;
	};

	struct STrophyUser
	{
		SRegisterTrophiesData  m_RegisterTrophiesWorkArea;
		SRegisterTrophiesData* m_pRegisteredTrophiesResult;
		SAwardTrophyData       m_AwardTrophyWorkArea;
		SAwardTrophyData*      m_pAwardedTrophyResult;
		SceNpTrophyContext     m_trophyContext;
		SceNpTrophyHandle      m_trophyHandle;
		uint32                 m_user;

		EDrxLobbyError Initialise(uint32 user);
		EDrxLobbyError Reset();
		EDrxLobbyError CreateHandle();
		void           ClearRegisterTrophiesWorkArea();
		void           ClearAwardTrophyWorkArea();
		static void    TrophyThreadRegisterTrophiesFunc(CDrxPSNReward::STrophyUser* pTrophyUser, uint64 data1);
		static void    TrophyThreadAwardTrophyFunc(CDrxPSNReward::STrophyUser* pTrophyUser, uint64 data1);
	};

	struct SPendingTrophy
	{
		DrxRewardCallback pCb;
		void*             pCbArg;
		DrxLobbyTaskID    lTaskID;
		SceNpTrophyId     trophyID;
		uint32            user;
	};

	typedef void (* TOrbisTrophyThreadJobFunc)(STrophyUser*, uint64);

	struct SOrbisTrophyThreadJobInfo
	{
		TOrbisTrophyThreadJobFunc   m_pJobFunc;
		CDrxPSNReward::STrophyUser* m_pTrophyUser;
		uint64                      m_data1;

		SOrbisTrophyThreadJobInfo(TOrbisTrophyThreadJobFunc pFunc, CDrxPSNReward::STrophyUser* pTrophyUser, uint64 data1)
		{
			m_pJobFunc = pFunc;
			m_pTrophyUser = pTrophyUser;
			m_data1 = data1;
		}
	};

	class COrbisTrophyThread
		: public IThread
	{
	public:
		COrbisTrophyThread() : m_bAlive(true) {};

		// Start accepting work on thread
		virtual void ThreadEntry();           // IThread
		void         SignalStopWork();

		void         Push(TOrbisTrophyThreadJobFunc pFunc, CDrxPSNReward::STrophyUser* pTrophyUser, uint64 data1);
		void         Flush(CDrxPSNReward::STrophyUser* pTrophyUser);

	private:
		DrxMutex                              m_mtx;
		DrxConditionVariable                  m_cond;
		std::deque<SOrbisTrophyThreadJobInfo> m_jobQueue;
		bool m_bAlive;
	};

	COrbisTrophyThread* m_pOrbisTrophyJobThread;

	void         TriggerCallbackOnGameThread(SPendingTrophy pending, EDrxLobbyError error);
	STrophyUser* GetTrophyUserFromUser(uint32 user);

	STrophyUser    m_User[NUM_TROPHY_USERS];

	SPendingTrophy m_pendingTrophies[MAXIMUM_PENDING_TROPHIES];
	uint32         m_pendingTrophyCount;
};

	#endif // End [USE_PSN]
#endif   // DRX_PLATFORM_ORBIS

#endif // End [!defined(__DRXPSN2_REWARD_H__)]
// [EOF]
