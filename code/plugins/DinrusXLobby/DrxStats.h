// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXSTATS_H__
#define __DRXSTATS_H__

#pragma once

<DinrusX/CoreX/Lobby/IDrxStats.h>
#include "DrxLobby.h"
#include "DrxMatchMaking.h"

#if USE_DRX_STATS

	#define MAX_STATS_PARAMS 7
	#define MAX_STATS_TASKS  5

typedef uint32 DrxStatsTaskID;
const DrxStatsTaskID DrxStatsInvalidTaskID = 0xffffffff;

class CDrxStats : public CMultiThreadRefCount, public IDrxStats
{
public:
	CDrxStats(CDrxLobby* pLobby, CDrxLobbyService* pService);

	EDrxLobbyError                   Initialise();
	EDrxLobbyError                   Terminate();

	virtual bool                     IsDead() const                                                                                                                                                       { return false; }

	virtual EDrxLobbyError           StatsWriteUserData(DrxUserID* pUserIDs, SDrxLobbyUserData** ppData, uint32* pNumData, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg) { return eCLE_InvalidRequest; };
	virtual void                     CancelTask(DrxLobbyTaskID lTaskID);
	virtual void                     SetLeaderboardType(EDrxLobbyLeaderboardType leaderboardType)                                                                                                         {}
	virtual void                     RegisterLeaderboardNameIdPair(string name, uint32 id)                                                                                                                {}
	virtual EDrxLobbyLeaderboardType GetLeaderboardType()                                                                                                                                                 { return eCLLT_P2P; }

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
		TMemHdl               paramsMem[MAX_STATS_PARAMS];
		uint32                paramsNum[MAX_STATS_PARAMS];
		bool                  used;
		bool                  running;
		bool                  canceled;
	};

	EDrxLobbyError StartTask(uint32 eTask, bool startRunning, DrxStatsTaskID* pSTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	virtual void   FreeTask(DrxStatsTaskID sTaskID);
	EDrxLobbyError CreateTaskParamMem(DrxStatsTaskID sTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	void           UpdateTaskError(DrxStatsTaskID sTaskID, EDrxLobbyError error);

	EDrxLobbyError EncryptUserData(uint32 nBufferSize, const uint8* pUnencrypted, uint8* pEncrypted);
	EDrxLobbyError DecryptUserData(uint32 nBufferSize, const uint8* pEncrypted, uint8* pDecrypted);

	STask*            m_pTask[MAX_STATS_TASKS];
	CDrxLobby*        m_pLobby;
	CDrxLobbyService* m_pService;
};

#endif // USE_DRX_STATS
#endif // __DRXSTATS_H__
