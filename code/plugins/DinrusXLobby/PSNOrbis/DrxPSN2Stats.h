// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_STATS_H__
#define __DRXPSN2_STATS_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		#include "DrxStats.h"
		#include "DrxPSN2Support.h"

		#if USE_DRX_STATS

			#define MAX_PSN_TUS_PROFILE_SIZE (3000)                 //-- 3000 bytes (not 3K!) to match the Xbox 360 limitation.
//-- We have to limit the buffer to a multiple of 16 bytes for rijndael to work, so this works out as 2992 bytes
//-- for the PSN data, not 3000
			#define MAX_PSN_PROFILE_SETTING_BUFFER_SIZE (MAX_PSN_TUS_PROFILE_SIZE & ~(15))

			#define MAX_PARALLEL_BOARD_WRITES           (6) //-- same as max number of leaderboards that can be written at the same time on xbox

class CDrxPSNStats : public CDrxStats
{
public:
	CDrxPSNStats(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport);

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();

	void                   Tick(CTimeValue tv);

	virtual EDrxLobbyError StatsRegisterLeaderBoards(SDrxStatsLeaderBoardWrite* pBoards, uint32 numBoards, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsWriteLeaderBoards(DrxSessionHandle session, uint32 user, SDrxStatsLeaderBoardWrite* pBoards, uint32 numBoards, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsWriteLeaderBoards(DrxSessionHandle session, DrxUserID* pUserIDs, SDrxStatsLeaderBoardWrite** ppBoards, uint32* pNumBoards, uint32 numUsers, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsReadLeaderBoardByRankForRange(DrxStatsLeaderBoardID board, uint32 startRank, uint32 num, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsReadLeaderBoardByRankForUser(DrxStatsLeaderBoardID board, uint32 user, uint32 num, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsReadLeaderBoardByUserID(DrxStatsLeaderBoardID board, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg);

	virtual EDrxLobbyError StatsRegisterUserData(SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsWriteUserData(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsReadUserData(uint32 user, DrxLobbyTaskID* pTaskID, DrxStatsReadUserDataCallback cb, void* pCbArg);
	virtual EDrxLobbyError StatsReadUserData(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxStatsReadUserDataCallback cb, void* pCbArg);

	static void            SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg);

private:
	enum ETask
	{
		eT_StatsRegisterLeaderBoards,

		eT_StatsWriteLeaderBoards,
		eST_StatsWriteLeaderBoardsAsyncPolling,

		eT_StatsReadLeaderBoardByRankForRange,
		eT_StatsReadLeaderBoardByRankForUser,
		eT_StatsReadLeaderBoardByUserID,
		eST_StatsReadLeaderBoardByRankForUserAsyncPolling,
		eST_StatsReadLeaderBoardAsyncPolling,

		eT_StatsRegisterUserData,

		eT_StatsWriteUserData,
		eST_StatsWriteUserDataAsyncPolling,

		eT_StatsReadUserData,
		eST_StatsReadUserDataAsyncPolling,

		eT_StatsVerifyString,
		eST_StatsVerifyStringAsyncPolling,
	};

	struct SRegisterLeaderBoardData
	{
		DrxStatsLeaderBoardID boardID;
		DrxLobbyUserDataID    scoreID;
		uint32                numColumns;

		struct SColumn
		{
			DrxLobbyUserDataID dataID;
			uint16             columnID;
			uint16             dataType;
		};

		SColumn* pColumns;
	};

	struct SRegisterUserData
	{
		uint16 id;
		uint16 type;
	};

	struct STask : public CDrxStats::STask
	{
		SceNpId npId;
		int     transaction;
	} m_task[MAX_STATS_TASKS];

	struct SDrxPSNStatsBoardWriteInfo
	{
		SceNpScoreBoardId    boardId;
		SceNpScoreValue      score;
		SceNpScoreComment    comment;
		SceNpScoreGameInfo   data;
		SceNpScoreRankNumber tempRank;
		uint32               nCumulativeDataSize;
		int                  transaction;
	};

	struct SDrxPSNStatsBoardReadInfo
	{
		int                  transaction;
		SceNpScoreBoardId    boardId;
		SceNpScoreRankNumber start;
		uint32               count;
		SceRtcTick           lastSortDate;
		SceNpScoreRankNumber totalSize;
	};

	struct SProfileDataBufferGroup
	{
		uint8 unencryptedBuffer[MAX_PSN_PROFILE_SETTING_BUFFER_SIZE];
		uint8 encryptedBuffer[MAX_PSN_TUS_PROFILE_SIZE];
	};

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxStatsTaskID* pSTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartSubTask(ETask etask, DrxStatsTaskID sTaskID);
	void           StartTaskRunning(DrxStatsTaskID sTaskID);
	void           EndTask(DrxStatsTaskID sTaskID);
	void           StopTaskRunning(DrxStatsTaskID sTaskID);

	EDrxLobbyError StatsWriteLeaderBoards(const SceNpId* const pMyNpId, SDrxStatsLeaderBoardWrite** ppBoards, uint32* pNumBoards, uint32 numUsers, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	void           TickStatsWriteLeaderBoards(DrxStatsTaskID sTaskID);
	void           TickStatsWriteLeaderBoardsAsyncPolling(DrxStatsTaskID sTaskID);

	void           StartStatsReadLeaderBoardByRankForRange(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoardByRankForRange(DrxStatsTaskID sTaskID);
	void           StartStatsReadLeaderBoardByRankForUser(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoardByRankForUser(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoardByRankForUserPolling(DrxStatsTaskID sTaskID);
	void           StartStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoardAsyncPolling(DrxStatsTaskID sTaskID);

	EDrxLobbyError FillColumnFromPSNLeaderboardData(SDrxStatsLeaderBoardUserColumn*, SRegisterLeaderBoardData::SColumn*, uint8* pData, uint32* pOffset);
	void           EndStatsReadLeaderBoard(DrxStatsTaskID sTaskID);
	void           EndStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID);

	void           TickStatsWriteUserData(DrxStatsTaskID sTaskID);
	void           TickStatsWriteUserDataAsyncPolling(DrxStatsTaskID sTaskID);

	void           StartStatsReadUserData(DrxStatsTaskID sTaskID);
	void           TickStatsReadUserData(DrxStatsTaskID sTaskID);
	void           TickStatsReadUserDataAsyncPolling(DrxStatsTaskID sTaskID);
	void           EndStatsReadUserData(DrxStatsTaskID sTaskID);

	void           TickStatsVerifyString(DrxStatsTaskID sTaskID);
	void           TickStatsVerifyStringAsyncPolling(DrxStatsTaskID sTaskID);

	void           ProcessErrorEvent(SDrxPSNSupportCallbackEventData& data);

	EDrxLobbyError CreateScoreTransactionHandle(int* pHandle);
	void           AbortScoreTransactionHandle(int* pHandle);
	void           FreeScoreTransactionHandle(int* pHandle);
	EDrxLobbyError CreateTusTransactionHandle(int* pHandle);
	void           AbortTusTransactionHandle(int* pHandle);
	void           FreeTusTransactionHandle(int* pHandle);

	DynArray<SRegisterLeaderBoardData> m_leaderBoards;
	DynArray<SRegisterUserData>        m_userData;
	uint32                             m_numLeaderBoards;
	uint32                             m_numUserData;
	uint32                             m_nCumulativeUserDataSize;

	CDrxPSNSupport*                    m_pPSNSupport;
};

		#endif // USE_DRX_STATS
	#endif   // USE_PSN
#endif     // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_STATS_H__
