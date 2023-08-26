// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamStats.h
//  Created:     11/10/2012 by Andrew Catlender
//  Modified:    05/04/2014 by Michiel Meesters
//  Описание: Integration of Steamworks API into DrxStats
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSTEAMSTATS_H__
#define __DRXSTEAMSTATS_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxStats.h"

#if USE_STEAM && USE_DRX_STATS

	#include "steam/steam_api.h"
	#include "DrxSteamMatchMaking.h"

////////////////////////////////////////////////////////////////////////////////

class CDrxSteamStats : public CDrxStats
{
public:
	CDrxSteamStats(CDrxLobby* pLobby, CDrxLobbyService* pService);

	EDrxLobbyError         Initialise();
	void                   Tick(CTimeValue tv);
	void                   EnterInteractiveGameSession();
	void                   LeaveInteractiveGameSession();

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

	virtual void           RegisterLeaderboardNameIdPair(string name, uint32 id);

private:
	enum ETask
	{
		eT_StatsRegisterLeaderBoards,
		eT_StatsWriteLeaderBoards,
		eT_StatsReadLeaderBoardByRankForRange,
		eT_StatsReadLeaderBoardByRankForUser,
		eT_StatsReadLeaderBoardByUserID,
		eT_StatsRegisterUserData,
		eT_StatsWriteUserData,
		eT_StatsReadUserData
	};

	struct SRegisterLeaderBoardData
	{
		DrxStatsLeaderBoardID boardID;
		DrxLobbyUserDataID    scoreID;
		uint32                numColumns;

		struct SColumn
		{
			DrxLobbyUserDataID dataID;
			DrxLobbyUserDataID columnID;
			uint16             dataType;
		};

		SColumn* pColumns;
	};

	struct SRegisterUserData
	{
		DrxLobbyUserDataID id;
		uint16             type;
	};

	struct  STask : public CDrxStats::STask
	{
	};

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxStatsTaskID* pSTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartSubTask(ETask etask, DrxStatsTaskID sTaskID);
	void           StartTaskRunning(DrxStatsTaskID sTaskID);
	void           EndTask(DrxStatsTaskID sTaskID);
	void           StopTaskRunning(DrxStatsTaskID sTaskID);
	EDrxLobbyError TickTask(DrxStatsTaskID sTaskID);

	void           StartEnumerate(DrxStatsTaskID sTaskID, uint32 bufferSize);
	EDrxLobbyError GetLeaderBoardSpec(DrxStatsLeaderBoardID board /*, XUSER_STATS_SPEC* pSpec*/);

	EDrxLobbyError StatsWriteLeaderBoards(DrxSessionHandle session /*, XUID* pXUIDs*/, SDrxStatsLeaderBoardWrite** ppBoards, uint32* pNumBoards, uint32 numUsers, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg);
	void           StartStatsWriteLeaderBoards(DrxStatsTaskID sTaskID);
	void           TickStatsWriteLeaderBoards(DrxStatsTaskID sTaskID);

	void           StartStatsReadLeaderBoardByRankForRange(DrxStatsTaskID sTaskID);
	void           TickStatsReadLeaderBoard(DrxStatsTaskID sTaskID);
	void           EndStatsReadLeaderBoard(DrxStatsTaskID sTaskID);
	void           StartStatsReadLeaderBoardByRankForUser(DrxStatsTaskID sTaskID);

	void           StartStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID);

	void           StartStatsWriteUserData(DrxStatsTaskID sTaskID);
	void           TickStatsWriteUserData(DrxStatsTaskID sTaskID);

	void           StartStatsReadUserData(DrxStatsTaskID sTaskID);
	void           TickStatsReadUserData(DrxStatsTaskID sTaskID);
	void           EndStatsReadUserData(DrxStatsTaskID sTaskID);

	DRXSTEAM_CALLRESULT(CDrxSteamStats, OnFindLeaderboard, LeaderboardFindResult_t, m_callResultFindLeaderboard);
	DRXSTEAM_CALLRESULT(CDrxSteamStats, OnDownloadedLeaderboard, LeaderboardScoresDownloaded_t, m_callResultDownloadLeaderboard);
	DRXSTEAM_CALLRESULT(CDrxSteamStats, OnUploadedScore, LeaderboardScoreUploaded_t, m_callResultWriteLeaderboard);

	DRXSTEAM_CALLBACK(CDrxSteamStats, OnUserStatsReceived, UserStatsReceived_t, m_callUserStatsReceived);
	DRXSTEAM_CALLBACK(CDrxSteamStats, OnUserStatsStored, UserStatsStored_t, m_callUserStatsStored);

	STask                              m_task[MAX_STATS_TASKS];
	DynArray<SRegisterLeaderBoardData> m_leaderBoards;
	DynArray<SRegisterUserData>        m_userData;
	uint32                             m_numLeaderBoards;
	uint32                             m_numUserData;
	uint32                             m_nCumulativeUserDataSize;

	typedef std::map<uint32, string> TIdToNameMap;
	TIdToNameMap m_IdToNameMap;
};

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_STATS
#endif // __DRXSTEAMSTATS_H__
