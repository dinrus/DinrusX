// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Reward.h"

		<DinrusX/libsysmodule.h>

		#define TROPHY_ALREADY_UNLOCKED SCE_NP_TROPHY_ERROR_TROPHY_ALREADY_UNLOCKED

		#define USE_TROPHY_API

EDrxLobbyError CDrxPSNReward::STrophyUser::Initialise(uint32 user)
{
	EDrxLobbyError rc = eCLE_InternalError;

	m_user = user;
	m_trophyContext = SCE_NP_TROPHY_INVALID_CONTEXT;
	m_trophyHandle = SCE_NP_TROPHY_INVALID_HANDLE;

	SceNpServiceLabel label = 0;
	int ret = -1;
		#if defined(USE_TROPHY_API)
	// cppcheck-suppress redundantAssignment
	ret = sceNpTrophyCreateContext(&m_trophyContext, (SceUserServiceUserId)user, label, 0);
		#endif
	if (ret == PSN_OK)
	{
		rc = eCLE_Success;
	}
	else
	{
		NetLog("DrxPSNReward: sceNpTrophyCreateContext failed to create trophy context: 0x%08x", ret);
	}

	ClearRegisterTrophiesWorkArea();
	ClearAwardTrophyWorkArea();

	return rc;
}

EDrxLobbyError CDrxPSNReward::STrophyUser::Reset()
{
		#if defined(USE_TROPHY_API)
	if (m_trophyHandle != SCE_NP_TROPHY_INVALID_HANDLE)
	{
		sceNpTrophyDestroyHandle(m_trophyHandle);
		m_trophyHandle = SCE_NP_TROPHY_INVALID_HANDLE;
	}
	if (m_trophyContext != SCE_NP_TROPHY_INVALID_CONTEXT)
	{
		sceNpTrophyDestroyContext(m_trophyContext);
		m_trophyContext = SCE_NP_TROPHY_INVALID_CONTEXT;
	}
		#endif

	ClearRegisterTrophiesWorkArea();
	ClearAwardTrophyWorkArea();

	return eCLE_Success;
}

void CDrxPSNReward::STrophyUser::ClearRegisterTrophiesWorkArea()
{
	m_RegisterTrophiesWorkArea.error = PSN_OK;
	m_pRegisteredTrophiesResult = NULL;
}

void CDrxPSNReward::STrophyUser::ClearAwardTrophyWorkArea()
{
	m_AwardTrophyWorkArea.awardedTrophyId = SCE_NP_TROPHY_INVALID_TROPHY_ID;
	m_AwardTrophyWorkArea.platinumTrophyId = SCE_NP_TROPHY_INVALID_TROPHY_ID;
	m_AwardTrophyWorkArea.error = PSN_OK;
	m_pAwardedTrophyResult = NULL;
}

EDrxLobbyError CDrxPSNReward::STrophyUser::CreateHandle()
{
	assert(m_trophyContext != SCE_NP_TROPHY_INVALID_CONTEXT);

	if (m_trophyHandle != SCE_NP_TROPHY_INVALID_HANDLE)
	{
		return eCLE_Success;
	}

	// Create handle
	int ret = -1;
		#if defined(USE_TROPHY_API)
	// cppcheck-suppress redundantAssignment
	ret = sceNpTrophyCreateHandle(&m_trophyHandle);
		#endif
	if (ret == PSN_OK)
	{
		return eCLE_Success;
	}
	else
	{
		NetLog("DrxPSNReward: sceNpTrophyCreateHandle() failed to create trophy handle: 0x%08x", ret);
		return eCLE_InternalError;
	}
}

void CDrxPSNReward::STrophyUser::TrophyThreadAwardTrophyFunc(CDrxPSNReward::STrophyUser* _this, uint64 data1)
{
	//-- this is running on COrbisTrophyThread thread.
	//-- These are atomic read-only, nothing _should_ be modifying these values while the task is running.
	assert(_this->m_trophyContext != SCE_NP_TROPHY_INVALID_CONTEXT);
	assert(_this->m_trophyHandle != SCE_NP_TROPHY_INVALID_HANDLE);

		#if defined(USE_TROPHY_API)
	_this->m_AwardTrophyWorkArea.error = sceNpTrophyUnlockTrophy(_this->m_trophyContext, _this->m_trophyHandle, (SceNpTrophyId)data1, &_this->m_AwardTrophyWorkArea.platinumTrophyId);
		#endif
	if (_this->m_AwardTrophyWorkArea.error == PSN_OK)
	{
		_this->m_AwardTrophyWorkArea.awardedTrophyId = (SceNpTrophyId)data1;
	}
	else
	{
		NetLog("DrxPSNReward: failed to unlock trophy %" PRIu64 ": 0x%08x", data1, _this->m_AwardTrophyWorkArea.error);

		//-- Need to fill in awardedTrophyId anyway, so it can be matched against the pending list to remove the failed request
		_this->m_AwardTrophyWorkArea.awardedTrophyId = (SceNpTrophyId)data1;
	}

	//-- Our thread task is finished so we can set the pointer to the data and use the ptr to read the data as necessary.
	_this->m_pAwardedTrophyResult = &_this->m_AwardTrophyWorkArea;
}

//-- this is running on COrbisTrophyThread thread.
void CDrxPSNReward::STrophyUser::TrophyThreadRegisterTrophiesFunc(CDrxPSNReward::STrophyUser* _this, uint64 data1)
{
	//-- These are atomic read-only, nothing _should_ be modifying these values while the task is running.
	assert(_this->m_trophyContext != SCE_NP_TROPHY_INVALID_CONTEXT);
	assert(_this->m_trophyHandle != SCE_NP_TROPHY_INVALID_HANDLE);

		#if defined(USE_TROPHY_API)
	_this->m_RegisterTrophiesWorkArea.error = sceNpTrophyRegisterContext(_this->m_trophyContext, _this->m_trophyHandle, 0);
		#endif
	if (_this->m_RegisterTrophiesWorkArea.error != PSN_OK)
	{
		NetLog("DrxPSNReward: failed to register trophy context: 0x%08x", _this->m_RegisterTrophiesWorkArea.error);
	}

	//-- Our thread task is finished so we can set the pointer to the data and use the ptr to read the data as necessary.
	_this->m_pRegisteredTrophiesResult = &_this->m_RegisterTrophiesWorkArea;
}

void CDrxPSNReward::COrbisTrophyThread::ThreadEntry()
{
	while (true)
	{
		m_mtx.Lock();

		while (m_bAlive && m_jobQueue.empty())
		{
			m_cond.Wait(m_mtx);
		}

		if (!m_bAlive)
		{
			break;
		}

		SOrbisTrophyThreadJobInfo info(NULL, NULL, 0);
		info = m_jobQueue.front();
		m_jobQueue.pop_front();

		if ((info.m_pTrophyUser != 0) && info.m_pJobFunc)
		{
			info.m_pJobFunc(info.m_pTrophyUser, info.m_data1);
		}

		m_mtx.Unlock();
	}

	// thread is cancelled, clear remaining queue jobs
	{
		std::deque<SOrbisTrophyThreadJobInfo> tempQueue;
		m_jobQueue.swap(tempQueue);
	}

	m_mtx.Unlock();
}

void CDrxPSNReward::COrbisTrophyThread::SignalStopWork()
{
	DrxAutoLock<DrxMutex> lk(m_mtx);
	m_bAlive = false;
	m_cond.Notify();
}

void CDrxPSNReward::COrbisTrophyThread::Push(TOrbisTrophyThreadJobFunc pFunc, CDrxPSNReward::STrophyUser* pTrophyUser, uint64 data1)
{
	DrxAutoLock<DrxMutex> lk(m_mtx);
	m_jobQueue.push_back(SOrbisTrophyThreadJobInfo(pFunc, pTrophyUser, data1));
	m_cond.Notify();
}

void CDrxPSNReward::COrbisTrophyThread::Flush(CDrxPSNReward::STrophyUser* pTrophyUser)
{
	DrxAutoLock<DrxMutex> lk(m_mtx);

	std::deque<SOrbisTrophyThreadJobInfo>::iterator it = m_jobQueue.begin();
	while (it != m_jobQueue.end())
	{
		STrophyUser* pTestUser = (STrophyUser*)it->m_pTrophyUser;

		if (pTestUser->m_user == pTrophyUser->m_user)
		{
			it = m_jobQueue.erase(it);
		}
		else
		{
			++it;
		}
	}
	m_cond.Notify();
}

CDrxPSNReward::CDrxPSNReward(CDrxLobby* pLobby)
	: CDrxReward(pLobby)
	, m_pOrbisTrophyJobThread(nullptr)
{
	ZeroMemory(&m_User, sizeof(m_User));
	ZeroMemory(&m_pendingTrophies, sizeof(m_pendingTrophies));
}

CDrxPSNReward::~CDrxPSNReward(void)
{
	DRX_ASSERT_MESSAGE(m_pendingTrophyCount == 0, "DrxPSNReward: still have pending awards at destruction");

	if (m_status != eCRS_Uninitialised)
	{
		Terminate();
	}
}

EDrxLobbyError CDrxPSNReward::Initialise()
{
	m_status = eCRS_Uninitialised;
	EDrxLobbyError rc = eCLE_NotInitialised;

	m_pendingTrophyCount = 0;

	m_pOrbisTrophyJobThread = NULL;

	// Initialise utility
	int ret = -1;
		#if defined(USE_TROPHY_API)
	// cppcheck-suppress redundantAssignment
	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TROPHY);
	if (ret == PSN_OK)
	{
		SceUserServiceInitializeParams initParams;
		memset(&initParams, 0, sizeof(initParams));
		initParams.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;

		SceUserServiceUserId user;
		ret = sceUserServiceInitialize(&initParams);
		DRX_ASSERT(ret == SCE_OK || ret == SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED);
		ret = sceUserServiceGetInitialUser(&user);

		m_pOrbisTrophyJobThread = new COrbisTrophyThread();

		if (!gEnv->pThreadUpr->SpawnThread(m_pOrbisTrophyJobThread, "CDrxPSNReward_COrbisTrophyThread"))
		{
			DRX_ASSERT_MESSAGE(false, "Error spawning \"CDrxPSNReward_COrbisTrophyThread\" thread.");
			__debugbreak();
			delete m_pOrbisTrophyJobThread;
			m_pOrbisTrophyJobThread = NULL;
			rc = eCLE_InternalError;
			return rc;
		}

		uint32 nSlot = 0;
		STrophyUser* pUser = &m_User[0];
		rc = pUser->Initialise((uint32)user);

		if (rc == eCLE_Success)
		{
			m_status = eCRS_Idle;
			RegisterTrophies(user, this);
		}
	}
	else
	{
		NetLog("DrxPSNReward: failed to initialise trophy utility: 0x%08x", ret);
		rc = eCLE_InternalError;
	}
		#endif //USE_TROPHY_API

	return rc;
}

EDrxLobbyError CDrxPSNReward::Terminate()
{
	if (m_pOrbisTrophyJobThread)
	{
		m_pOrbisTrophyJobThread->SignalStopWork();
		gEnv->pThreadUpr->JoinThread(m_pOrbisTrophyJobThread, eJM_Join);

		for (uint32 i = 0; i < NUM_TROPHY_USERS; i++)
		{
			m_pOrbisTrophyJobThread->Flush(&m_User[i]);
		}

		SAFE_DELETE(m_pOrbisTrophyJobThread);
	}

	for (uint32 i = 0; i < NUM_TROPHY_USERS; i++)
	{
		m_User[i].Reset();
	}

		#if defined(USE_TROPHY_API)
	int ret;
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_TROPHY);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_TROPHY) failed. ret = 0x%x\n", ret);
	}
		#endif

	m_pendingTrophyCount = 0;

	m_status = eCRS_Uninitialised;

	return eCLE_Success;
}

void CDrxPSNReward::Tick(CTimeValue tv)
{
	if (m_pendingTrophyCount)
	{
		STrophyUser* pUser = GetTrophyUserFromUser(m_pendingTrophies[0].user);
		if (pUser && pUser->m_pRegisteredTrophiesResult && (pUser->m_pRegisteredTrophiesResult->error == PSN_OK))
		{
			switch (m_status)
			{
			case eCRS_Idle:
				{
					//-- create handle (if a valid one doesn't already exist)
					if (pUser->CreateHandle() == eCLE_Success)
					{
						pUser->ClearAwardTrophyWorkArea();

						if (m_pOrbisTrophyJobThread)
						{
							m_pOrbisTrophyJobThread->Push(CDrxPSNReward::STrophyUser::TrophyThreadAwardTrophyFunc, pUser, (uint64)m_pendingTrophies[0].trophyID);
						}

						m_status = eCRS_Working;
					}
				}
				break;

			case eCRS_Working:
				{
					DRX_ASSERT_MESSAGE(pUser->m_pAwardedTrophyResult && (pUser->m_pAwardedTrophyResult->awardedTrophyId == m_pendingTrophies[0].trophyID), "DrxPSNReward wrong trophy!");
					if (pUser->m_pAwardedTrophyResult && (pUser->m_pAwardedTrophyResult->awardedTrophyId == m_pendingTrophies[0].trophyID))
					{
						EDrxLobbyError err = eCLE_InternalError;

						if (pUser->m_pAwardedTrophyResult->error == PSN_OK)
						{
							err = eCLE_Success;
						}
						else if (pUser->m_pAwardedTrophyResult->error == TROPHY_ALREADY_UNLOCKED)
						{
							err = eCLE_SuccessContinue;
						}
						TO_GAME_FROM_LOBBY(&CDrxPSNReward::TriggerCallbackOnGameThread, this, m_pendingTrophies[0], err);

						//-- release lobby task handle
						m_pLobby->ReleaseTask(m_pendingTrophies[0].lTaskID);

						//-- move the queue. Not pretty, but it's not going to be called often enough for performance to be a concern.
						memmove(m_pendingTrophies, &m_pendingTrophies[1], sizeof(SPendingTrophy) * (MAXIMUM_PENDING_TROPHIES - 1));
						m_pendingTrophyCount--;

						if (pUser->m_pAwardedTrophyResult->platinumTrophyId != SCE_NP_TROPHY_INVALID_TROPHY_ID)
						{
							//-- Hey, we've just been automatically awarded the platinum trophy! (typically awarded when all other trophies are unlocked)
							//-- If we ever need to do any processing in response to that, put it here.
						}

						m_status = eCRS_Idle;
					}
				}
				break;
			}
		}
	}
}

EDrxLobbyError CDrxPSNReward::RegisterTrophies(uint32 user, void* pArg)
{
	CDrxPSNReward* _this = static_cast<CDrxPSNReward*>(pArg);

	EDrxLobbyError err = eCLE_InternalError;

	if (_this->m_status >= eCRS_Idle)
	{
		STrophyUser* pUser = _this->GetTrophyUserFromUser(user);
		if (pUser)
		{
			//-- create handle (if a valid one doesn't already exist)
			err = pUser->CreateHandle();
			if (err == eCLE_Success)
			{
				if (_this->m_pOrbisTrophyJobThread)
				{
					_this->m_pOrbisTrophyJobThread->Push(CDrxPSNReward::STrophyUser::TrophyThreadRegisterTrophiesFunc, pUser, 0);
				}
			}
		}
	}

	return err;
}

EDrxRewardError CDrxPSNReward::Award(uint32 user, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	if (m_pendingTrophyCount < (MAXIMUM_PENDING_TROPHIES - 1))
	{
		STrophyUser* pUser = GetTrophyUserFromUser(user);
		if (pUser && pUser->m_pRegisteredTrophiesResult && (pUser->m_pRegisteredTrophiesResult->error == PSN_OK))
		{
			// Add any new awards after any that are currently being processed (if any).
			DrxLobbyTaskID lobbyTaskID = m_pLobby->CreateTask();
			if (lobbyTaskID != DrxLobbyInvalidTaskID)
			{
				m_pendingTrophies[m_pendingTrophyCount].lTaskID = lobbyTaskID;
				m_pendingTrophies[m_pendingTrophyCount].pCb = cb;
				m_pendingTrophies[m_pendingTrophyCount].pCbArg = pCbArg;
				m_pendingTrophies[m_pendingTrophyCount].trophyID = (SceNpTrophyId)awardID;
				m_pendingTrophies[m_pendingTrophyCount].user = user;
				m_pendingTrophyCount++;

				if (pTaskID)
				{
					*pTaskID = lobbyTaskID;
				}

				return eCRE_Queued;
			}
		}

		return eCRE_Failed;
	}
	else
	{
		// Pending queue full... please try again later.
		return eCRE_Busy;
	}
}

// Runs on game thread to call the callback for the awarded reward.
void CDrxPSNReward::TriggerCallbackOnGameThread(CDrxPSNReward::SPendingTrophy pending, EDrxLobbyError error)
{
	if (pending.pCb)
	{
		if (error == eCLE_SuccessContinue)
		{
			pending.pCb(pending.lTaskID, 0, pending.trophyID, eCLE_Success, true, pending.pCbArg);
		}
		else
		{
			pending.pCb(pending.lTaskID, 0, pending.trophyID, error, false, pending.pCbArg);
		}
	}
}

CDrxPSNReward::STrophyUser* CDrxPSNReward::GetTrophyUserFromUser(uint32 user)
{
	for (uint32 i = 0; i < NUM_TROPHY_USERS; i++)
	{
		if (m_User[i].m_user == user)
		{
			return &m_User[i];
		}
	}
	return &m_User[0];
}

	#endif // End [USE_PSN]
#endif   // DRX_PLATFORM_ORBIS
