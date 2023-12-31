// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN
		#if USE_DRX_STATS

			<DinrusX/libsysmodule.h>
			#include "DrxPSN2Stats.h"
			#include "../DrxSharedLobbyPacket.h"

			#define PSN_TUS_SLOT_PROFILE (1)            //-- PSN TUS binary data slot 1 is reserved for the user profile

			#define SCORE_GAMEINFO_SIZE  SCE_NP_SCORE_GAMEINFO_MAXSIZE

EDrxLobbyError CDrxPSNStats::CreateScoreTransactionHandle(int* pHandle)
{
	if (pHandle && (*pHandle == 0))
	{
		int ret;
		ret = sceNpScoreCreateRequest(m_pPSNSupport->GetScoringContextId());
		if (ret > 0)
		{
			*pHandle = ret;
			return eCLE_Success;
		}
		else
		{
			m_pPSNSupport->HandlePSNError(ret);
		}
	}

	return eCLE_InternalError;
}

void CDrxPSNStats::AbortScoreTransactionHandle(int* pHandle)
{
	if (pHandle && (*pHandle > 0))
	{
		int ret;
		ret = sceNpScoreAbortRequest(*pHandle);
		FreeScoreTransactionHandle(pHandle);
		m_pPSNSupport->HandlePSNError(ret);
	}
}

void CDrxPSNStats::FreeScoreTransactionHandle(int* pHandle)
{
	if (pHandle && (*pHandle > 0))
	{
		int ret;
		ret = sceNpScoreDeleteRequest(*pHandle);
		*pHandle = 0;
		m_pPSNSupport->HandlePSNError(ret);
	}
}

EDrxLobbyError CDrxPSNStats::CreateTusTransactionHandle(int* pHandle)
{
	if (pHandle)
	{
		int ret;
		ret = sceNpTusCreateRequest(m_pPSNSupport->GetTUSContextId());
		if (ret > 0)
		{
			*pHandle = ret;
			return eCLE_Success;
		}
		else
		{
			m_pPSNSupport->HandlePSNError(ret);
		}
	}

	return eCLE_InternalError;
}

void CDrxPSNStats::AbortTusTransactionHandle(int* pHandle)
{
	if (pHandle && (*pHandle > 0))
	{
		int ret;
		ret = sceNpTusAbortRequest(*pHandle);
		FreeTusTransactionHandle(pHandle);
		m_pPSNSupport->HandlePSNError(ret);
	}
}

void CDrxPSNStats::FreeTusTransactionHandle(int* pHandle)
{
	if (pHandle && (*pHandle > 0))
	{
		int ret;
		ret = sceNpTusDeleteRequest(*pHandle);
		*pHandle = 0;
		m_pPSNSupport->HandlePSNError(ret);
	}
}

void CDrxPSNStats::ProcessErrorEvent(SDrxPSNSupportCallbackEventData& data)
{
	for (uint32 i = 0; i < MAX_STATS_TASKS; i++)
	{
		STask* pTask = &m_task[i];

		if (pTask->used && pTask->running)
		{
			AbortTusTransactionHandle(&pTask->transaction);

			UpdateTaskError(i, MapSupportErrorToLobbyError(data.m_errorEvent.m_error));
			StopTaskRunning(i);
		}
	}
}

void CDrxPSNStats::SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg)
{
	CDrxPSNStats* _this = static_cast<CDrxPSNStats*>(pArg);

	switch (ecb)
	{
	case eCE_ErrorEvent:
		{
			_this->ProcessErrorEvent(data);
		}
		break;
	}
}

CDrxPSNStats::CDrxPSNStats(CDrxLobby* pLobby, CDrxLobbyService* pService, CDrxPSNSupport* pSupport)
	: CDrxStats(pLobby, pService)
{
	m_pPSNSupport = pSupport;

	// Make the CDrxStats base pointers point to our data so we can use the common code in CDrxStats
	for (uint32 i = 0; i < MAX_STATS_TASKS; i++)
	{
		CDrxStats::m_pTask[i] = &m_task[i];
	}

	m_numLeaderBoards = 0;
	m_numUserData = 0;
	m_nCumulativeUserDataSize = 0;
	ZeroMemory(&m_task, sizeof(m_task));
}

			#define STATS_PARAM_WRITE_BOARD                   0
			#define STATS_PARAM_WRITE_BOARD_COUNT             0

			#define STATS_PARAM_READ_LEADERBOARD_ENTRY        0
			#define STATS_PARAM_READ_LEADERBOARD_RANKS        1
			#define STATS_PARAM_READ_LEADERBOARD_COMMENTS     2
			#define STATS_PARAM_READ_LEADERBOARD_DATA         3
			#define STATS_PARAM_READ_LEADERBOARD_GAME_ROWS    4
			#define STATS_PARAM_READ_LEADERBOARD_GAME_COLUMNS 5
			#define STATS_PARAM_READ_LEADERBOARD_NPID_LIST    6

EDrxLobbyError CDrxPSNStats::Initialise()
{
	EDrxLobbyError error = CDrxStats::Initialise();

	if (error == eCLE_Success)
	{
		int ret;
		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_SCORE_RANKING);
		if (ret != PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_SCORE_RANKING) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}

		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TUS);
		if (ret != PSN_OK)
		{
			NetLog("sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TUS) failed. ret = 0x%x\n", ret);
			return eCLE_InternalError;
		}
	}

	for (uint32 taskIndex = 0; taskIndex < MAX_STATS_TASKS; taskIndex++)
	{
		STask* pTask = &m_task[taskIndex];
		pTask->transaction = 0;
		pTask->paramsMem[STATS_PARAM_WRITE_BOARD] = TMemInvalidHdl;
	}

	return error;
}

EDrxLobbyError CDrxPSNStats::Terminate()
{
	for (uint32 taskIndex = 0; taskIndex < MAX_STATS_TASKS; taskIndex++)
	{
		STask* pTask = &m_task[taskIndex];

		switch (pTask->subTask)
		{
		case eT_StatsWriteLeaderBoards:
		case eST_StatsWriteLeaderBoardsAsyncPolling:
			{
				if (pTask->paramsMem[STATS_PARAM_WRITE_BOARD] != TMemInvalidHdl)
				{
					SDrxPSNStatsBoardWriteInfo* pBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);
					if (pBoard != NULL)
					{
						for (uint32 x = 0; x < pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT]; x++)
						{
							AbortScoreTransactionHandle(&pBoard->transaction);
							pBoard++;
						}
					}
				}
			}
			break;

		case eT_StatsReadLeaderBoardByRankForRange:
		case eT_StatsReadLeaderBoardByRankForUser:
		case eST_StatsReadLeaderBoardByRankForUserAsyncPolling:
		case eT_StatsReadLeaderBoardByUserID:
		case eST_StatsReadLeaderBoardAsyncPolling:
			{
				if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
				{
					SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
					AbortScoreTransactionHandle(&pReadInfo->transaction);
				}
			}
			break;
		}

		AbortTusTransactionHandle(&pTask->transaction);

		if (pTask->running)
		{
			StopTaskRunning(taskIndex);
		}
	}

	int ret;
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_TUS);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_TUS) failed. ret = 0x%x\n", ret);
	}
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_SCORE_RANKING);
	if (ret < PSN_OK)
	{
		NetLog("sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_SCORE_RANKING) failed. ret = 0x%x\n", ret);
	}

	return eCLE_Success;
}

void CDrxPSNStats::Tick(CTimeValue tv)
{
	for (uint32 i = 0; i < MAX_STATS_TASKS; i++)
	{
		STask* pTask = &m_task[i];

			#if ENABLE_DRXLOBBY_DEBUG_TESTS
		if (pTask->used)
		{
			if (m_pLobby->DebugTickCallStartTaskRunning(pTask->lTaskID))
			{
				StartTaskRunning(i);
				continue;
			}
			if (!m_pLobby->DebugOKToTickTask(pTask->lTaskID, pTask->running))
			{
				continue;
			}
		}
			#endif

		if (pTask->used && pTask->running)
		{
			switch (pTask->subTask)
			{
			case eT_StatsWriteLeaderBoards:
				TickStatsWriteLeaderBoards(i);
				break;
			case eST_StatsWriteLeaderBoardsAsyncPolling:
				TickStatsWriteLeaderBoardsAsyncPolling(i);
				break;

			case eT_StatsReadLeaderBoardByRankForRange:
				TickStatsReadLeaderBoardByRankForRange(i);
				break;
			case eT_StatsReadLeaderBoardByRankForUser:
				TickStatsReadLeaderBoardByRankForUser(i);
				break;
			case eST_StatsReadLeaderBoardByRankForUserAsyncPolling:
				TickStatsReadLeaderBoardByRankForUserPolling(i);
				break;
			case eT_StatsReadLeaderBoardByUserID:
				TickStatsReadLeaderBoardByUserID(i);
				break;
			case eST_StatsReadLeaderBoardAsyncPolling:
				TickStatsReadLeaderBoardAsyncPolling(i);
				break;

			case eT_StatsWriteUserData:
				TickStatsWriteUserData(i);
				break;
			case eST_StatsWriteUserDataAsyncPolling:
				TickStatsWriteUserDataAsyncPolling(i);
				break;

			case eT_StatsReadUserData:
				TickStatsReadUserData(i);
				break;
			case eST_StatsReadUserDataAsyncPolling:
				TickStatsReadUserDataAsyncPolling(i);
				break;

			case eT_StatsVerifyString:
				TickStatsVerifyString(i);
				break;
			case eST_StatsVerifyStringAsyncPolling:
				TickStatsVerifyStringAsyncPolling(i);
				break;
			}
		}
	}
}

EDrxLobbyError CDrxPSNStats::StartTask(ETask etask, bool startRunning, uint32 user, DrxStatsTaskID* pSTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg)
{
	DrxStatsTaskID tmpSTaskID;
	DrxStatsTaskID* pUseSTaskID = pSTaskID ? pSTaskID : &tmpSTaskID;
	EDrxLobbyError error = CDrxStats::StartTask(etask, startRunning, pUseSTaskID, pLTaskID, h, pCb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[*pUseSTaskID];
		memset(&pTask->npId, 0, sizeof(SceNpId));
	}

	return error;
}

void CDrxPSNStats::StartSubTask(ETask etask, DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	pTask->subTask = etask;
}

void CDrxPSNStats::StartTaskRunning(DrxStatsTaskID sTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[sTaskID];

	if (pTask->used)
	{
			#if ENABLE_DRXLOBBY_DEBUG_TESTS
		EDrxLobbyError error;

		if (!m_pLobby->DebugOKToStartTaskRunning(pTask->lTaskID))
		{
			return;
		}

		if (m_pLobby->DebugGenerateError(pTask->lTaskID, error))
		{
			UpdateTaskError(sTaskID, error);
			StopTaskRunning(sTaskID);
			return;
		}
			#endif

		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_StatsRegisterLeaderBoards:
		case eT_StatsRegisterUserData:
			StopTaskRunning(sTaskID);
			break;

		case eT_StatsReadLeaderBoardByRankForRange:
			StartStatsReadLeaderBoardByRankForRange(sTaskID);
			break;

		case eT_StatsReadLeaderBoardByRankForUser:
			StartStatsReadLeaderBoardByRankForUser(sTaskID);
			break;

		case eT_StatsReadLeaderBoardByUserID:
			StartStatsReadLeaderBoardByUserID(sTaskID);
			break;

		case eT_StatsReadUserData:
			StartStatsReadUserData(sTaskID);
			break;
		}
	}
}

void CDrxPSNStats::EndTask(DrxStatsTaskID sTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[sTaskID];

	if (pTask->used)
	{
		if (pTask->pCb)
		{
			switch (pTask->startedTask)
			{
			case eT_StatsRegisterLeaderBoards:
			case eT_StatsWriteLeaderBoards:
			case eT_StatsRegisterUserData:
			case eT_StatsWriteUserData:
			case eT_StatsVerifyString:
				((DrxStatsCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
				break;

			case eT_StatsReadLeaderBoardByRankForRange:
			case eT_StatsReadLeaderBoardByRankForUser:
				EndStatsReadLeaderBoard(sTaskID);
				break;

			case eT_StatsReadLeaderBoardByUserID:
				EndStatsReadLeaderBoardByUserID(sTaskID);
				break;

			case eT_StatsReadUserData:
				EndStatsReadUserData(sTaskID);
				break;
			}
		}

		NetLog("[Lobby] Stats EndTask %d (%d) Result %d", pTask->startedTask, pTask->subTask, pTask->error);

		FreeTask(sTaskID);
	}
}

void CDrxPSNStats::StopTaskRunning(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxPSNStats::EndTask, this, sTaskID);
	}
}

EDrxLobbyError CDrxPSNStats::StatsRegisterLeaderBoards(SDrxStatsLeaderBoardWrite* pBoards, uint32 numBoards, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	DrxStatsTaskID sTaskID;
	EDrxLobbyError error = StartTask(eT_StatsRegisterLeaderBoards, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		if ((m_numLeaderBoards + numBoards) > m_leaderBoards.size())
		{
			m_leaderBoards.resize(m_leaderBoards.size() + numBoards);
		}

		for (uint32 i = 0; i < numBoards; i++)
		{
			SRegisterLeaderBoardData* pRegisterData = &m_leaderBoards[m_numLeaderBoards];
			SDrxStatsLeaderBoardWrite* pBoardData = &pBoards[i];

			pRegisterData->boardID = pBoardData->id;
			pRegisterData->scoreID = pBoardData->data.score.id;
			pRegisterData->numColumns = pBoardData->data.numColumns;

			if (pRegisterData->numColumns > 0)
			{
				pRegisterData->pColumns = new SRegisterLeaderBoardData::SColumn[pRegisterData->numColumns];

				for (uint32 j = 0; j < pRegisterData->numColumns; j++)
				{
					SRegisterLeaderBoardData::SColumn* pRegisterColumn = &pRegisterData->pColumns[j];
					SDrxStatsLeaderBoardUserColumn* pBoardColumn = &pBoardData->data.pColumns[j];

					pRegisterColumn->columnID = pBoardColumn->columnID;
					pRegisterColumn->dataID = pBoardColumn->data.m_id;
					pRegisterColumn->dataType = pBoardColumn->data.m_type;
				}
			}
			else
			{
				pRegisterData->pColumns = NULL;
			}

			m_numLeaderBoards++;
		}

		FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
	}

	NetLog("[Lobby] Start StatsRegisterLeaderBoards return %d", error);

	return error;
}

EDrxLobbyError CDrxPSNStats::StatsWriteLeaderBoards(DrxSessionHandle session, uint32 user, SDrxStatsLeaderBoardWrite* pBoards, uint32 numBoards, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	const SceNpId* const pMyNpId = m_pPSNSupport->GetLocalNpId();
	if (pMyNpId)
	{
		SDrxStatsLeaderBoardWrite* pLeaderBoards[1];
		uint32 numLeaderBoards[1];

		pLeaderBoards[0] = pBoards;
		numLeaderBoards[0] = numBoards;

		error = StatsWriteLeaderBoards(pMyNpId, pLeaderBoards, numLeaderBoards, 1, pTaskID, cb, pCbArg);
	}
	else
	{
		error = eCLE_UserNotSignedIn;
	}

	return error;
}

EDrxLobbyError CDrxPSNStats::StatsWriteLeaderBoards(DrxSessionHandle session, DrxUserID* pUserIDs, SDrxStatsLeaderBoardWrite** ppBoards, uint32* pNumBoards, uint32 numUsers, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	const SceNpId* const pMyNpId = m_pPSNSupport->GetLocalNpId();
	if (pMyNpId)
	{
		error = StatsWriteLeaderBoards(pMyNpId, ppBoards, pNumBoards, numUsers, pTaskID, cb, pCbArg);
	}
	else
	{
		error = eCLE_UserNotSignedIn;
	}

	return error;
}

EDrxLobbyError CDrxPSNStats::StatsWriteLeaderBoards(const SceNpId* const pMyNpId, SDrxStatsLeaderBoardWrite** ppBoards, uint32* pNumBoards, uint32 numUsers, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	//-- Only ever one user on PSN, cannot write leaderboards of other people
	assert(numUsers == 1);

	DrxStatsTaskID sTaskID;

	EDrxLobbyError error = StartTask(eT_StatsWriteLeaderBoards, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[sTaskID];

		memcpy(&pTask->npId, pMyNpId, sizeof(SceNpId));

		pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT] = pNumBoards[0];
		error = CreateTaskParamMem(sTaskID, STATS_PARAM_WRITE_BOARD, NULL, sizeof(SDrxPSNStatsBoardWriteInfo) * pNumBoards[0]);
		if (error == eCLE_Success)
		{
			SDrxStatsLeaderBoardWrite* pInBoard = &ppBoards[0][0];
			SDrxPSNStatsBoardWriteInfo* pOutBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);
			memset(pOutBoard, 0, sizeof(SDrxPSNStatsBoardWriteInfo));

			for (uint32 i = 0; i < pNumBoards[0]; i++)
			{
				pOutBoard->transaction = 0;
				pOutBoard->boardId = (SceNpScoreBoardId)pInBoard->id;
				pOutBoard->score = (SceNpScoreValue)pInBoard->data.score.score.m_int64;

				//-- For possible future clan name support
				memset(&pOutBoard->comment, 0, sizeof(SceNpScoreComment));
				pOutBoard->comment.utf8Comment[0] = 'N';
				pOutBoard->comment.utf8Comment[1] = 'E';
				pOutBoard->comment.utf8Comment[2] = 'V';
				pOutBoard->comment.utf8Comment[3] = '\0';

				//-- For the column storage
				memset(&pOutBoard->data, 0, sizeof(SceNpScoreGameInfo));
				pOutBoard->nCumulativeDataSize = 0;

				for (uint32 x = 0; (x < pInBoard->data.numColumns) && (error == eCLE_Success); x++)
				{
					uint32 nItemSize = 0;

					switch (pInBoard->data.pColumns[x].data.m_type)
					{
					case eCLUDT_Int64:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_int64);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_int64, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					case eCLUDT_Int32:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_int32);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_int32, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					case eCLUDT_Int16:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_int16);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_int16, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					case eCLUDT_Int8:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_int8);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_int8, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					case eCLUDT_Float64:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_f64);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_f64, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					case eCLUDT_Float32:
						{
							nItemSize = sizeof(pInBoard->data.pColumns[x].data.m_f32);
							if ((pOutBoard->nCumulativeDataSize + nItemSize) <= SCORE_GAMEINFO_SIZE)
							{
								memcpy(&pOutBoard->data.data[pOutBoard->nCumulativeDataSize], &pInBoard->data.pColumns[x].data.m_f32, nItemSize);
								pOutBoard->data.infoSize += nItemSize;
								pOutBoard->nCumulativeDataSize += nItemSize;
							}
							else
							{
								error = eCLE_OutOfMemory;
							}
						}
						break;
					}
				}

				pOutBoard++;
				pInBoard++;
			}

			if (error == eCLE_Success)
			{
				m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

				FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
			}
			else
			{
				FreeTask(sTaskID);
			}
		}
		else
		{
			FreeTask(sTaskID);
		}
	}

	NetLog("[Lobby] Start StatsWriteLeaderBoards return %d", error);

	return error;
}

void CDrxPSNStats::TickStatsWriteLeaderBoards(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_WRITE_BOARD] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardWriteInfo* pBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);

			StartSubTask(eST_StatsWriteLeaderBoardsAsyncPolling, sTaskID);

			for (uint32 x = 0; (x < pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT]) && (pTask->error == eCLE_Success); x++)
			{
				UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pBoard->transaction));
				if (pTask->error == eCLE_Success)
				{
					int ret = sceNpScoreRecordScoreAsync(pBoard->transaction, pBoard->boardId, pBoard->score, &pBoard->comment, &pBoard->data, &pBoard->tempRank, 0, NULL);
					if (ret == PSN_OK)
					{
						//-- started successfully
					}
					else
					{
						DrxLogAlways("sceNpScoreRecordScoreAsync : error %08X", ret);
						if (!m_pPSNSupport->HandlePSNError(ret))
						{
							UpdateTaskError(sTaskID, eCLE_InternalError);
						}
					}
				}

				pBoard++;
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_OutOfMemory);
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		SDrxPSNStatsBoardWriteInfo* pBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);
		for (uint32 x = 0; x < pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT]; x++)
		{
			AbortScoreTransactionHandle(&pBoard->transaction);
			pBoard++;
		}

		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsWriteLeaderBoardsAsyncPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		int result = 0;

		SDrxPSNStatsBoardWriteInfo* pBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);

		uint32 nTransactionEnded = 0;

		for (uint32 x = 0; (x < pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT]) && (pTask->error == eCLE_Success); x++)
		{
			if (pBoard->transaction > 0)
			{
				int ret = sceNpScorePollAsync(pBoard->transaction, &result);
				if (ret == PSN_OK)
				{
					//-- success!
					FreeScoreTransactionHandle(&pBoard->transaction);
					if (result >= PSN_OK)
					{
						//-- result is good.
					}
					else
					{
						if (!m_pPSNSupport->HandlePSNError(result))
						{
							UpdateTaskError(sTaskID, eCLE_InternalError);
						}
					}
				}
				else if (ret < PSN_OK)
				{
					DrxLogAlways("sceNpScorePollAsync() : Error %08X", ret);
					if (!m_pPSNSupport->HandlePSNError(ret))
					{
						UpdateTaskError(sTaskID, eCLE_InternalError);
					}
				}
				else
				{
					//-- still running the async task
				}
			}

			if (pBoard->transaction == 0)
			{
				nTransactionEnded++;
			}

			pBoard++;
		}

		if (pTask->error == eCLE_Success)
		{
			if (nTransactionEnded == pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT])
			{
				UpdateTaskError(sTaskID, eCLE_Success);
				StopTaskRunning(sTaskID);
			}
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		SDrxPSNStatsBoardWriteInfo* pBoard = (SDrxPSNStatsBoardWriteInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_BOARD]);
		for (uint32 x = 0; x < pTask->paramsNum[STATS_PARAM_WRITE_BOARD_COUNT]; x++)
		{
			AbortScoreTransactionHandle(&pBoard->transaction);
			pBoard++;
		}

		StopTaskRunning(sTaskID);
	}
}

EDrxLobbyError CDrxPSNStats::StatsReadLeaderBoardByRankForRange(DrxStatsLeaderBoardID board, uint32 startRank, uint32 num, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxStatsTaskID sTaskID;

	EDrxLobbyError error = StartTask(eT_StatsReadLeaderBoardByRankForRange, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[sTaskID];

		error = CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_ENTRY, NULL, sizeof(SDrxPSNStatsBoardReadInfo));
		if (error == eCLE_Success)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			memset(pReadInfo, 0, sizeof(SDrxPSNStatsBoardReadInfo));

			pReadInfo->transaction = 0;
			pReadInfo->boardId = board;
			pReadInfo->start = max(startRank, 1U);
			pReadInfo->count = num;

			m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

			FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
		}
		else
		{
			FreeTask(sTaskID);
		}
	}

	NetLog("[Lobby] Start StatsReadLeaderBoardByRankForRange return %d", error);

	return error;
}

void CDrxPSNStats::StartStatsReadLeaderBoardByRankForRange(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		uint32 nRankBufferSize = sizeof(SceNpScoreRankData) * pReadInfo->count;
		uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
		uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

		UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_RANKS, NULL, nRankBufferSize));
		if (pTask->error == eCLE_Success)
		{
			UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_COMMENTS, NULL, nCommentBufferSize));
			if (pTask->error == eCLE_Success)
			{
				UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_DATA, NULL, nDataBufferSize));
				if (pTask->error == eCLE_Success)
				{
					m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
				}
			}
		}
	}
	else
	{
		UpdateTaskError(sTaskID, eCLE_InternalError);
	}

	NetLog("[Lobby] StartStatsReadLeaderBoardByRankForRange result %d", pTask->error);

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsReadLeaderBoardByRankForRange(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			uint32 nRankBufferSize = sizeof(SceNpScoreRankData) * pReadInfo->count;
			uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
			uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

			SceNpScoreRankData* pRankBuffer = (SceNpScoreRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
			SceNpScoreComment* pCommentBuffer = (SceNpScoreComment*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_COMMENTS]);
			SceNpScoreGameInfo* pDataBuffer = (SceNpScoreGameInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_DATA]);

			UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pReadInfo->transaction));
			if (pTask->error == eCLE_Success)
			{
				StartSubTask(eST_StatsReadLeaderBoardAsyncPolling, sTaskID);

				int ret = sceNpScoreGetRankingByRangeAsync(pReadInfo->transaction, pReadInfo->boardId, pReadInfo->start,
				                                           pRankBuffer, nRankBufferSize,
				                                           pCommentBuffer, nCommentBufferSize,
				                                           pDataBuffer, nDataBufferSize,
				                                           pReadInfo->count, &pReadInfo->lastSortDate, &pReadInfo->totalSize,
				                                           NULL);
				if (ret == PSN_OK)
				{
					//-- PSN task started successfully
				}
				else
				{
					DrxLogAlways("sceNpScoreGetRankingByRangeAsync : error %08X", ret);
					if (!m_pPSNSupport->HandlePSNError(ret))
					{
						UpdateTaskError(sTaskID, eCLE_InternalError);
					}
				}
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_OutOfMemory);
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			AbortScoreTransactionHandle(&pReadInfo->transaction);
			StopTaskRunning(sTaskID);
		}
	}
}

EDrxLobbyError CDrxPSNStats::StatsReadLeaderBoardByRankForUser(DrxStatsLeaderBoardID board, uint32 user, uint32 num, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxStatsTaskID sTaskID;

	EDrxLobbyError error = StartTask(eT_StatsReadLeaderBoardByRankForUser, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[sTaskID];

		error = CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_ENTRY, NULL, sizeof(SDrxPSNStatsBoardReadInfo));
		if (error == eCLE_Success)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			memset(pReadInfo, 0, sizeof(SDrxPSNStatsBoardReadInfo));

			pReadInfo->boardId = board;
			pReadInfo->start = 1;
			pReadInfo->count = num;

			m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

			FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
		}
		else
		{
			FreeTask(sTaskID);
		}
	}

	NetLog("[Lobby] Start StatsReadLeaderBoardByRankForUser return %d", error);

	return error;
}

void CDrxPSNStats::StartStatsReadLeaderBoardByRankForUser(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		uint32 nRankBufferSize = max(sizeof(SceNpScoreRankData) * pReadInfo->count, sizeof(SceNpScorePlayerRankData) + sizeof(SceNpId));
		uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
		uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

		UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_RANKS, NULL, nRankBufferSize));
		if (pTask->error == eCLE_Success)
		{
			UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_COMMENTS, NULL, nCommentBufferSize));
			if (pTask->error == eCLE_Success)
			{
				UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_DATA, NULL, nDataBufferSize));
				if (pTask->error == eCLE_Success)
				{
					m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
				}
			}
		}
	}
	else
	{
		UpdateTaskError(sTaskID, eCLE_InternalError);
	}

	NetLog("[Lobby] StartStatsReadLeaderBoardByRankForUser result %d", pTask->error);

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsReadLeaderBoardByRankForUser(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			SceNpScorePlayerRankData* pMyRank = (SceNpScorePlayerRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
			uint32 nRankBufferSize = sizeof(SceNpScorePlayerRankData);

			UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pReadInfo->transaction));
			if (pTask->error == eCLE_Success)
			{
				const SceNpId* const pMyNpId = m_pPSNSupport->GetLocalNpId();
				if (pMyNpId)
				{
					StartSubTask(eST_StatsReadLeaderBoardByRankForUserAsyncPolling, sTaskID);

					int ret = sceNpScoreGetRankingByNpIdAsync(pReadInfo->transaction, pReadInfo->boardId, pMyNpId, 1 * sizeof(SceNpId),
					                                          &pMyRank[0], nRankBufferSize,
					                                          NULL, 0,
					                                          NULL, 0,
					                                          1, &pReadInfo->lastSortDate, &pReadInfo->totalSize,
					                                          NULL);
					if (ret == PSN_OK)
					{
						//-- PSN task started successfully
					}
					else
					{
						DrxLogAlways("sceNpScoreGetRankingByNpIdAsync : error %08X", ret);
						if (!m_pPSNSupport->HandlePSNError(ret))
						{
							UpdateTaskError(sTaskID, eCLE_Success);
						}
					}
				}
				else
				{
					DrxLogAlways("No NpId");
					UpdateTaskError(sTaskID, eCLE_InternalError);
				}
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_InternalError);
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			AbortScoreTransactionHandle(&pReadInfo->transaction);
			StopTaskRunning(sTaskID);
		}
	}
}

void CDrxPSNStats::TickStatsReadLeaderBoardByRankForUserPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);

			int result = 0;
			int ret = sceNpScorePollAsync(pReadInfo->transaction, &result);
			if (ret == PSN_OK)
			{
				//-- success!
				FreeScoreTransactionHandle(&pReadInfo->transaction);

				if ((result >= 0) || (result == SCE_NP_COMMUNITY_SERVER_ERROR_GAME_RANKING_NOT_FOUND))
				{
					SceNpScorePlayerRankData* pMyRank = (SceNpScorePlayerRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
					SceNpScoreRankData* pRankBuffer = (SceNpScoreRankData*)pMyRank; //-- reusing this buffer for a different purpose
					SceNpScoreComment* pCommentBuffer = (SceNpScoreComment*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_COMMENTS]);
					SceNpScoreGameInfo* pDataBuffer = (SceNpScoreGameInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_DATA]);

					if (pMyRank->hasData && (result >= 0))
					{
						//-- I now know my rank. So we'll use that to set the correct pReadInfo->start position for a range search
						uint32 nPageSize = pReadInfo->count - 1;
						uint32 nHalfSize = nPageSize / 2;
						int32 nTotalMinusPage = pReadInfo->totalSize - nPageSize;
						int32 nSerialRankMid = pMyRank->rankData.serialRank - nHalfSize;
						int32 nMinOffset = min(nTotalMinusPage, nSerialRankMid);
						pReadInfo->start = max(nMinOffset, 1);
					}
					else
					{
						//-- I'm not on the board, so just get some results from the top.
						pReadInfo->start = 1;
					}

					//-- start a range search.
					uint32 nRankBufferSize = sizeof(SceNpScoreRankData) * pReadInfo->count;
					uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
					uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

					UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pReadInfo->transaction));
					if (pTask->error == eCLE_Success)
					{
						StartSubTask(eST_StatsReadLeaderBoardAsyncPolling, sTaskID);

						int ret = sceNpScoreGetRankingByRangeAsync(pReadInfo->transaction, pReadInfo->boardId, pReadInfo->start,
						                                           pRankBuffer, nRankBufferSize,
						                                           pCommentBuffer, nCommentBufferSize,
						                                           pDataBuffer, nDataBufferSize,
						                                           pReadInfo->count, &pReadInfo->lastSortDate, &pReadInfo->totalSize,
						                                           NULL);
						if (ret == PSN_OK)
						{
							//-- PSN task started successfully
						}
						else
						{
							DrxLogAlways("sceNpScoreGetRankingByRangeAsync : error %08X", ret);
							if (!m_pPSNSupport->HandlePSNError(ret))
							{
								UpdateTaskError(sTaskID, eCLE_InternalError);
							}
						}
					}
				}
				else
				{
					if (!m_pPSNSupport->HandlePSNError(result))
					{
						UpdateTaskError(sTaskID, eCLE_InternalError);
					}
				}
			}
			else if (ret < PSN_OK)
			{
				DrxLogAlways("sceNpScorePollAsync() : Error %08X", ret);
				if (!m_pPSNSupport->HandlePSNError(ret))
				{
					UpdateTaskError(sTaskID, eCLE_InternalError);
				}
			}
			else
			{
				//-- still running the async task
			}
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		AbortScoreTransactionHandle(&pReadInfo->transaction);
		StopTaskRunning(sTaskID);
	}
}

EDrxLobbyError CDrxPSNStats::StatsReadLeaderBoardByUserID(DrxStatsLeaderBoardID board, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxStatsReadLeaderBoardCallback cb, void* pCbArg)
{
	DrxStatsTaskID sTaskID;

	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = StartTask(eT_StatsReadLeaderBoardByUserID, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[sTaskID];

		error = CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_ENTRY, NULL, sizeof(SDrxPSNStatsBoardReadInfo));
		if (error == eCLE_Success)
		{
			error = CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_NPID_LIST, NULL, sizeof(SceNpId) * numUserIDs);
			if (error == eCLE_Success)
			{
				SceNpId* pNpIdList = (SceNpId*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_NPID_LIST]);
				SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
				memset(pReadInfo, 0, sizeof(SDrxPSNStatsBoardReadInfo));

				pReadInfo->boardId = board;
				pReadInfo->start = 1;
				pReadInfo->count = numUserIDs;

				for (uint32 i = 0; i < numUserIDs; i++)
				{
					if (pUserIDs[i] == DrxUserInvalidID)
					{
						error = eCLE_InvalidUser;
						break;
					}

					pNpIdList[i] = ((SDrxPSNUserID*)pUserIDs[i].get())->npId;
				}

				if (error == eCLE_Success)
				{
					m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
					FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
				}
			}
		}

		if (error != eCLE_Success)
		{
			FreeTask(sTaskID);
		}
	}

	NetLog("[Lobby] Start StatsReadLeaderBoardByUserID return %d", error);

	return error;
}

void CDrxPSNStats::StartStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		uint32 nRankBufferSize = sizeof(SceNpScorePlayerRankData) * pReadInfo->count;
		uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
		uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

		UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_RANKS, NULL, nRankBufferSize));
		if (pTask->error == eCLE_Success)
		{
			UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_COMMENTS, NULL, nCommentBufferSize));
			if (pTask->error == eCLE_Success)
			{
				UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_DATA, NULL, nDataBufferSize));
				if (pTask->error == eCLE_Success)
				{
					m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
				}
			}
		}
	}
	else
	{
		UpdateTaskError(sTaskID, eCLE_InternalError);
	}

	NetLog("[Lobby] StartStatsReadLeaderBoardByUserID result %d", pTask->error);

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if ((pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl) &&
		    (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_NPID_LIST] != TMemInvalidHdl))
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			SceNpId* pNpIdList = (SceNpId*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_NPID_LIST]);

			uint32 nRankBufferSize = sizeof(SceNpScorePlayerRankData) * pReadInfo->count;
			uint32 nCommentBufferSize = sizeof(SceNpScoreComment) * pReadInfo->count;
			uint32 nDataBufferSize = sizeof(SceNpScoreGameInfo) * pReadInfo->count;

			SceNpScorePlayerRankData* pRankBuffer = (SceNpScorePlayerRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
			SceNpScoreComment* pCommentBuffer = (SceNpScoreComment*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_COMMENTS]);
			SceNpScoreGameInfo* pDataBuffer = (SceNpScoreGameInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_DATA]);

			UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pReadInfo->transaction));
			if (pTask->error == eCLE_Success)
			{
				StartSubTask(eST_StatsReadLeaderBoardAsyncPolling, sTaskID);

				int ret = sceNpScoreGetRankingByNpIdAsync(pReadInfo->transaction, pReadInfo->boardId, pNpIdList, sizeof(SceNpId) * pReadInfo->count,
				                                          pRankBuffer, nRankBufferSize,
				                                          pCommentBuffer, nCommentBufferSize,
				                                          pDataBuffer, nDataBufferSize,
				                                          pReadInfo->count, &pReadInfo->lastSortDate, &pReadInfo->totalSize,
				                                          NULL);
				if (ret == PSN_OK)
				{
					//-- PSN task started successfully
				}
				else
				{
					DrxLogAlways("sceNpScoreGetRankingByNpIdAsync : error %08X", ret);
					if (!m_pPSNSupport->HandlePSNError(ret))
					{
						UpdateTaskError(sTaskID, eCLE_Success);
					}
				}
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_InternalError);
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		if (pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY] != TMemInvalidHdl)
		{
			SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
			AbortScoreTransactionHandle(&pReadInfo->transaction);
			StopTaskRunning(sTaskID);
		}
	}
}

void CDrxPSNStats::TickStatsReadLeaderBoardAsyncPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);

		int result = 0;
		int ret = sceNpScorePollAsync(pReadInfo->transaction, &result);
		if (ret == PSN_OK)
		{
			//-- success!
			//-- The result should be the actual number of rows returned.
			if (result >= 0)
			{
				if (pTask->startedTask != eT_StatsReadLeaderBoardByUserID)
				{
					pReadInfo->count = (uint32)result;
				}

				FreeScoreTransactionHandle(&pReadInfo->transaction);
				UpdateTaskError(sTaskID, eCLE_Success);
				StopTaskRunning(sTaskID);
			}
			else
			{
				if (result == SCE_NP_COMMUNITY_SERVER_ERROR_GAME_RANKING_NOT_FOUND)
				{
					pReadInfo->count = 0;
					FreeScoreTransactionHandle(&pReadInfo->transaction);
					UpdateTaskError(sTaskID, eCLE_Success);
					StopTaskRunning(sTaskID);
				}
				else
				{
					if (!m_pPSNSupport->HandlePSNError(result))
					{
						UpdateTaskError(sTaskID, eCLE_InternalError);
					}
				}
			}
		}
		else if (ret < PSN_OK)
		{
			DrxLogAlways("sceNpScorePollAsync() : Error %08X", ret);
			if (!m_pPSNSupport->HandlePSNError(ret))
			{
				UpdateTaskError(sTaskID, eCLE_InternalError);
			}
		}
		else
		{
			//-- still running the async task
		}
	}

	if (pTask->canceled || pTask->error != eCLE_Success)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		AbortScoreTransactionHandle(&pReadInfo->transaction);
		StopTaskRunning(sTaskID);
	}
}

EDrxLobbyError CDrxPSNStats::FillColumnFromPSNLeaderboardData(SDrxStatsLeaderBoardUserColumn* pColumn,
                                                              SRegisterLeaderBoardData::SColumn* pRegisteredColumn,
                                                              uint8* pData,
                                                              uint32* pOffset)
{
	pColumn->columnID = pRegisteredColumn->columnID;
	pColumn->data.m_id = pRegisteredColumn->dataID;
	pColumn->data.m_type = (EDrxLobbyUserDataType)pRegisteredColumn->dataType;

	uint32 nItemSize = 0;
	uint32 updatedOffset = 0;

	switch (pColumn->data.m_type)
	{
	case eCLUDT_Int64:
		{
			nItemSize = sizeof(pColumn->data.m_int64);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_int64, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	case eCLUDT_Int32:
		{
			nItemSize = sizeof(pColumn->data.m_int32);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_int32, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	case eCLUDT_Int16:
		{
			nItemSize = sizeof(pColumn->data.m_int16);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_int16, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	case eCLUDT_Int8:
		{
			nItemSize = sizeof(pColumn->data.m_int8);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_int8, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	case eCLUDT_Float64:
		{
			nItemSize = sizeof(pColumn->data.m_f64);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_f64, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	case eCLUDT_Float32:
		{
			nItemSize = sizeof(pColumn->data.m_f32);
			updatedOffset = *pOffset + nItemSize;
			if (updatedOffset <= SCORE_GAMEINFO_SIZE)
			{
				memcpy(&pColumn->data.m_f32, pData, nItemSize);
				*pOffset = updatedOffset;
			}
			else
			{
				return eCLE_OutOfUserData;
			}
		}
		break;
	}

	return eCLE_Success;
}

void CDrxPSNStats::EndStatsReadLeaderBoard(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->error == eCLE_Success)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		SceNpScoreRankData* pRankBuffer = (SceNpScoreRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
		SceNpScoreComment* pCommentBuffer = (SceNpScoreComment*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_COMMENTS]);
		SceNpScoreGameInfo* pDataBuffer = (SceNpScoreGameInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_DATA]);

		for (uint32 i = 0; i < m_numLeaderBoards; i++)
		{
			SRegisterLeaderBoardData* pRegisteredLeaderBoard = &m_leaderBoards[i];
			if (pRegisteredLeaderBoard->boardID == pReadInfo->boardId)
			{
				SDrxStatsLeaderBoardReadResult result;

				result.id = pReadInfo->boardId;
				result.numRows = pReadInfo->count;
				result.totalNumBoardRows = pReadInfo->totalSize;

				UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_GAME_ROWS, NULL, result.numRows * sizeof(SDrxStatsLeaderBoardReadRow)));

				if (pTask->error == eCLE_Success)
				{
					uint32 numColumns = pReadInfo->count * pRegisteredLeaderBoard->numColumns;
					SDrxStatsLeaderBoardUserColumn* pColumns = NULL;

					result.pRows = (SDrxStatsLeaderBoardReadRow*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_GAME_ROWS]);

					if (numColumns > 0)
					{
						UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_GAME_COLUMNS, NULL, numColumns * sizeof(SDrxStatsLeaderBoardUserColumn)));

						if (pTask->error == eCLE_Success)
						{
							pColumns = (SDrxStatsLeaderBoardUserColumn*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_GAME_COLUMNS]);
						}
					}

					if (pTask->error == eCLE_Success)
					{
						// Our userID pointers are currently invalid so initialise them correctly.
						for (uint32 j = 0; j < result.numRows; j++)
						{
							result.pRows[j].userID.userID.ReleaseOwnership();
						}

						uint32 columnIndex = 0;

						for (uint32 j = 0; (j < result.numRows) && (pTask->error == eCLE_Success); j++)
						{
							SDrxStatsLeaderBoardReadRow* pRow = &result.pRows[j];

							SDrxPSNUserID* pID = new SDrxPSNUserID;
							if (pID)
							{
								pID->npId = pRankBuffer[j].npId;
								pRow->userID = pID;

								pRow->rank = pRankBuffer[j].serialRank;

								static_assert(sizeof(pRow->name) >= sizeof(pRankBuffer[j].npId.handle.data), "Invalid array size!");
								memset(pRow->name, 0, sizeof(pRow->name));
								strncpy(pRow->name, &pRankBuffer[j].npId.handle.data[0], sizeof(pRankBuffer[j].npId.handle.data));

								pRow->data.score.id = pRegisteredLeaderBoard->scoreID;
								pRow->data.score.score.m_int64 = pRankBuffer[j].scoreValue;
								pRow->data.score.score.m_type = eCLUDT_Int64;
								pRow->data.numColumns = pRegisteredLeaderBoard->numColumns;

								if (pColumns)
								{
									pRow->data.pColumns = &pColumns[columnIndex];
									columnIndex += pRow->data.numColumns;

									uint32 nCumulativeSize = 0;

									for (uint32 k = 0; (k < pRegisteredLeaderBoard->numColumns) && (pTask->error == eCLE_Success); k++)
									{
										SDrxStatsLeaderBoardUserColumn* pColumn = &pRow->data.pColumns[k];
										SRegisterLeaderBoardData::SColumn* pRegisteredColumn = &pRegisteredLeaderBoard->pColumns[k];

										uint8* pData = &pDataBuffer[j].data[nCumulativeSize];

										UpdateTaskError(sTaskID, FillColumnFromPSNLeaderboardData(pColumn, pRegisteredColumn, pData, &nCumulativeSize));
									}
								}
								else
								{
									pRow->data.pColumns = NULL;
								}
							}
						}

						((DrxStatsReadLeaderBoardCallback)pTask->pCb)(pTask->lTaskID, pTask->error, &result, pTask->pCbArg);

						// Remove our reference to the userIDs
						for (uint32 j = 0; j < result.numRows; j++)
						{
							result.pRows[j].userID = NULL;
						}
					}
				}

				break;
			}
		}
	}

	if (pTask->error != eCLE_Success)
	{
		((DrxStatsReadLeaderBoardCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, pTask->pCbArg);
	}
}

void CDrxPSNStats::EndStatsReadLeaderBoardByUserID(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->error == eCLE_Success)
	{
		SDrxPSNStatsBoardReadInfo* pReadInfo = (SDrxPSNStatsBoardReadInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_ENTRY]);
		SceNpScorePlayerRankData* pRankBuffer = (SceNpScorePlayerRankData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_RANKS]);
		SceNpScoreComment* pCommentBuffer = (SceNpScoreComment*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_COMMENTS]);
		SceNpScoreGameInfo* pDataBuffer = (SceNpScoreGameInfo*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_DATA]);

		for (uint32 i = 0; i < m_numLeaderBoards; i++)
		{
			SRegisterLeaderBoardData* pRegisteredLeaderBoard = &m_leaderBoards[i];
			if (pRegisteredLeaderBoard->boardID == pReadInfo->boardId)
			{
				SDrxStatsLeaderBoardReadResult result;

				result.id = pReadInfo->boardId;
				result.numRows = 0;
				result.totalNumBoardRows = pReadInfo->totalSize;

				for (uint32 nRow = 0; nRow < pReadInfo->count; nRow++)
				{
					if (pRankBuffer[nRow].hasData)
					{
						result.numRows++;
					}
				}

				UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_GAME_ROWS, NULL, result.numRows * sizeof(SDrxStatsLeaderBoardReadRow)));

				if (pTask->error == eCLE_Success)
				{
					uint32 numColumns = pRegisteredLeaderBoard->numColumns * result.numRows;
					SDrxStatsLeaderBoardUserColumn* pColumns = NULL;

					result.pRows = (SDrxStatsLeaderBoardReadRow*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_GAME_ROWS]);

					if (numColumns > 0)
					{
						UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_LEADERBOARD_GAME_COLUMNS, NULL, numColumns * sizeof(SDrxStatsLeaderBoardUserColumn)));

						if (pTask->error == eCLE_Success)
						{
							pColumns = (SDrxStatsLeaderBoardUserColumn*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_LEADERBOARD_GAME_COLUMNS]);
						}
					}

					if (pTask->error == eCLE_Success)
					{
						// Our userID pointers are currently invalid so initialise them correctly.
						for (uint32 j = 0; j < result.numRows; j++)
						{
							result.pRows[j].userID.userID.ReleaseOwnership();
						}

						uint32 columnIndex = 0;
						uint32 nRow = 0;

						for (uint32 j = 0; (j < pReadInfo->count) && (pTask->error == eCLE_Success); j++)
						{
							if (pRankBuffer[j].hasData)
							{
								SDrxStatsLeaderBoardReadRow* pRow = &result.pRows[nRow];

								SDrxPSNUserID* pID = new SDrxPSNUserID;
								if (pID)
								{
									pID->npId = pRankBuffer[j].rankData.npId;
									pRow->userID = pID;

									pRow->rank = pRankBuffer[j].rankData.serialRank;

									static_assert(sizeof(pRow->name) >= sizeof(pRankBuffer[j].rankData.npId.handle.data), "Invalid array size!");
									memset(pRow->name, 0, sizeof(pRow->name));
									strncpy(pRow->name, &pRankBuffer[j].rankData.npId.handle.data[0], sizeof(pRankBuffer[j].rankData.npId.handle.data));

									pRow->data.score.id = pRegisteredLeaderBoard->scoreID;
									pRow->data.score.score.m_int64 = pRankBuffer[j].rankData.scoreValue;
									pRow->data.score.score.m_type = eCLUDT_Int64;
									pRow->data.numColumns = pRegisteredLeaderBoard->numColumns;

									if (pColumns)
									{
										pRow->data.pColumns = &pColumns[columnIndex];
										columnIndex += pRow->data.numColumns;

										uint32 nCumulativeSize = 0;

										for (uint32 k = 0; (k < pRegisteredLeaderBoard->numColumns) && (pTask->error == eCLE_Success); k++)
										{
											SDrxStatsLeaderBoardUserColumn* pColumn = &pRow->data.pColumns[k];
											SRegisterLeaderBoardData::SColumn* pRegisteredColumn = &pRegisteredLeaderBoard->pColumns[k];

											uint8* pData = &pDataBuffer[j].data[nCumulativeSize];

											UpdateTaskError(sTaskID, FillColumnFromPSNLeaderboardData(pColumn, pRegisteredColumn, pData, &nCumulativeSize));
										}
									}
									else
									{
										pRow->data.pColumns = NULL;
									}
								}

								nRow++;
							}
						}

						((DrxStatsReadLeaderBoardCallback)pTask->pCb)(pTask->lTaskID, pTask->error, &result, pTask->pCbArg);

						// Remove our reference to the userIDs
						for (uint32 j = 0; j < result.numRows; j++)
						{
							result.pRows[j].userID = NULL;
						}
					}
				}
				break;
			}
		}
	}

	if (pTask->error != eCLE_Success)
	{
		((DrxStatsReadLeaderBoardCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, pTask->pCbArg);
	}
}

// ONLINE PERSISTENT USER DATA FUNCTIONS

EDrxLobbyError CDrxPSNStats::StatsRegisterUserData(SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxStatsTaskID sTaskID;

	m_nCumulativeUserDataSize = 0;
	m_numUserData = 0;

	EDrxLobbyError error = StartTask(eT_StatsRegisterUserData, false, 0, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

	if (error == eCLE_Success)
	{
		if ((m_numUserData + numData) > m_userData.size())
		{
			m_userData.resize(m_userData.size() + numData);
		}

		for (uint32 i = 0; (i < numData) && (error == eCLE_Success); i++)
		{
			uint32 itemSize;

			switch (pData[i].m_type)
			{
			case eCLUDT_Int64:
				itemSize = sizeof(pData[i].m_int64);
				break;

			case eCLUDT_Int32:
				itemSize = sizeof(pData[i].m_int32);
				break;

			case eCLUDT_Int16:
				itemSize = sizeof(pData[i].m_int16);
				break;

			case eCLUDT_Int8:
				itemSize = sizeof(pData[i].m_int8);
				break;

			case eCLUDT_Float64:
				itemSize = sizeof(pData[i].m_f64);
				break;

			case eCLUDT_Float32:
				itemSize = sizeof(pData[i].m_f32);
				break;

			case eCLUDT_Int64NoEndianSwap:
				itemSize = sizeof(pData[i].m_int64);
				break;

			default:
				itemSize = 0;
				DRX_ASSERT_MESSAGE(0, "CDrxPSNStats::StatsRegisterUserData: Undefined data type");
				break;
			}

			if ((m_nCumulativeUserDataSize + itemSize) <= MAX_PSN_PROFILE_SETTING_BUFFER_SIZE)
			{
				m_userData[m_numUserData].id = pData[i].m_id;
				m_userData[m_numUserData].type = pData[i].m_type;
				m_numUserData++;
				m_nCumulativeUserDataSize += itemSize;
			}
			else
			{
				DRX_ASSERT_MESSAGE(0, "CDrxPSNStats::StatsRegisterUserData: To much data to fit into the allocated TUS size.");
				error = eCLE_OutOfUserData;
			}
		}

		FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
	}

	NetLog("[Lobby] Start StatsRegisterUserData return %d", error);

	return error;
}

			#define STATS_PARAM_WRITE_USER_DATA_SETTINGS 0

EDrxLobbyError CDrxPSNStats::StatsWriteUserData(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxStatsCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	if (m_numUserData > 0)
	{
		DrxStatsTaskID sTaskID;

		error = StartTask(eT_StatsWriteUserData, false, user, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

		if (error == eCLE_Success)
		{
			error = CreateTaskParamMem(sTaskID, STATS_PARAM_WRITE_USER_DATA_SETTINGS, NULL, sizeof(SProfileDataBufferGroup));

			if (error == eCLE_Success)
			{
				SProfileDataBufferGroup* pBufferGroup = (SProfileDataBufferGroup*)m_pLobby->MemGetPtr(m_task[sTaskID].paramsMem[STATS_PARAM_WRITE_USER_DATA_SETTINGS]);

				CDrxSharedLobbyPacket packet;
				packet.SetWriteBuffer(pBufferGroup->unencryptedBuffer, MAX_PSN_PROFILE_SETTING_BUFFER_SIZE);
				packet.SetWriteBufferPos(0);

				for (uint32 j = 0; j < m_numUserData; j++)
				{
					if (pData[j].m_id == m_userData[j].id)
					{
						packet.WriteDrxLobbyUserData(&pData[j]);
					}
					else
					{
						error = eCLE_UserDataMissMatch;
					}
				}

				if (error == eCLE_Success)
				{
					m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

					FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
				}
			}

			if (error != eCLE_Success)
			{
				FreeTask(sTaskID);
			}
		}
	}
	else
	{
		error = eCLE_NoUserDataRegistered;
	}

	NetLog("[Lobby] Stats StatsWriteUserData return %d", error);

	return error;
}

void CDrxPSNStats::TickStatsWriteUserData(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_WRITE_USER_DATA_SETTINGS] != TMemInvalidHdl)
		{
			SProfileDataBufferGroup* pBufferGroup = (SProfileDataBufferGroup*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_WRITE_USER_DATA_SETTINGS]);

			memset(pBufferGroup->encryptedBuffer, 0, MAX_PSN_TUS_PROFILE_SIZE);

			UpdateTaskError(sTaskID, EncryptUserData(MAX_PSN_PROFILE_SETTING_BUFFER_SIZE, pBufferGroup->unencryptedBuffer, pBufferGroup->encryptedBuffer));

			if (pTask->error == eCLE_Success)
			{
				const SceNpId* const pMyNpId = m_pPSNSupport->GetLocalNpId();
				if (pMyNpId)
				{
					UpdateTaskError(sTaskID, CreateTusTransactionHandle(&pTask->transaction));

					if (pTask->error == eCLE_Success)
					{
						StartSubTask(eST_StatsWriteUserDataAsyncPolling, sTaskID);

						int ret = sceNpTusSetDataAsync(pTask->transaction, pMyNpId, PSN_TUS_SLOT_PROFILE, MAX_PSN_TUS_PROFILE_SIZE, MAX_PSN_TUS_PROFILE_SIZE, pBufferGroup->encryptedBuffer, NULL, 0, NULL, NULL, NULL);
						if (ret == PSN_OK)
						{
							//-- PSN task started successfully, wait for async to complete
						}
						else
						{
							DrxLogAlways("sceNpTusSetDataAsync : error %08X", ret);
							if (!m_pPSNSupport->HandlePSNError(ret))
							{
								UpdateTaskError(sTaskID, eCLE_InternalError);
							}
						}
					}
				}
				else
				{
					DrxLogAlways("No NpId");
					UpdateTaskError(sTaskID, eCLE_InvalidUser);
				}
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_OutOfMemory);
		}
	}

	if (pTask->canceled || (pTask->error != eCLE_Success))
	{
		AbortTusTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsWriteUserDataAsyncPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		int result = 0;
		int ret = sceNpTusPollAsync(pTask->transaction, &result);
		if (ret == PSN_OK)
		{
			//-- success!
			if (result >= PSN_OK)
			{
				FreeTusTransactionHandle(&pTask->transaction);
				UpdateTaskError(sTaskID, eCLE_Success);
				StopTaskRunning(sTaskID);
			}
			else
			{
				if (!m_pPSNSupport->HandlePSNError(result))
				{
					UpdateTaskError(sTaskID, eCLE_InternalError);
				}
			}
		}
		else if (ret < PSN_OK)
		{
			DrxLogAlways("sceNpTusPollAsync() : Error %08X", ret);
			if (!m_pPSNSupport->HandlePSNError(ret))
			{
				UpdateTaskError(sTaskID, eCLE_InternalError);
			}
		}
		else
		{
			//-- still running the async task
		}
	}

	if (pTask->error != eCLE_Success)
	{
		AbortTusTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

EDrxLobbyError CDrxPSNStats::StatsReadUserData(uint32 user, DrxLobbyTaskID* pTaskID, DrxStatsReadUserDataCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	if (m_numUserData > 0)
	{
		DrxStatsTaskID sTaskID;

		error = StartTask(eT_StatsReadUserData, false, user, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);
		if (error == eCLE_Success)
		{
			const SceNpId* const pMyNpId = m_pPSNSupport->GetLocalNpId();
			if (pMyNpId)
			{
				m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

				m_task[sTaskID].npId = *pMyNpId;
				FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
			}
			else
			{
				error = eCLE_InvalidUser;
				FreeTask(sTaskID);
			}
		}
	}
	else
	{
		error = eCLE_NoUserDataRegistered;
	}

	NetLog("[Lobby] Stats StatsReadUserData return %d", error);

	return error;
}

EDrxLobbyError CDrxPSNStats::StatsReadUserData(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxStatsReadUserDataCallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	if (m_numUserData > 0)
	{
		DrxStatsTaskID sTaskID;

		error = StartTask(eT_StatsReadUserData, false, user, &sTaskID, pTaskID, DrxLobbyInvalidSessionHandle, (void*)cb, pCbArg);

		if (error == eCLE_Success)
		{
			m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);

			m_task[sTaskID].npId = ((SDrxPSNUserID*)userID.get())->npId;
			FROM_GAME_TO_LOBBY(&CDrxPSNStats::StartTaskRunning, this, sTaskID);
		}
	}
	else
	{
		error = eCLE_NoUserDataRegistered;
	}

	NetLog("[Lobby] Stats StatsReadUserData return %d", error);

	return error;
}

			#define STATS_PARAM_READ_USER_DATA_RESULTS      0
			#define STATS_PARAM_READ_USER_DATA_STATUS       1
			#define STATS_PARAM_READ_USER_DATA_GAME_RESULTS 2

void CDrxPSNStats::StartStatsReadUserData(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_USER_DATA_RESULTS, NULL, sizeof(SProfileDataBufferGroup)));
	if (pTask->error == eCLE_Success)
	{
		UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_USER_DATA_STATUS, NULL, sizeof(SceNpTusDataStatus)));
		if (pTask->error == eCLE_Success)
		{
			m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
		}
	}

	NetLog("[Lobby] Stats StartStatsReadUserData result %d", pTask->error);
	if (pTask->error != eCLE_Success)
	{
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsReadUserData(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_READ_USER_DATA_RESULTS] != TMemInvalidHdl)
		{
			SProfileDataBufferGroup* pBufferGroup = (SProfileDataBufferGroup*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_USER_DATA_RESULTS]);

			if (pTask->paramsMem[STATS_PARAM_READ_USER_DATA_STATUS] != TMemInvalidHdl)
			{
				SceNpTusDataStatus* pDataStatus = (SceNpTusDataStatus*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_USER_DATA_STATUS]);

				UpdateTaskError(sTaskID, CreateTusTransactionHandle(&pTask->transaction));
				if (pTask->error == eCLE_Success)
				{
					StartSubTask(eST_StatsReadUserDataAsyncPolling, sTaskID);

					int ret = sceNpTusGetDataAsync(pTask->transaction, &pTask->npId, PSN_TUS_SLOT_PROFILE, pDataStatus, sizeof(SceNpTusDataStatus), pBufferGroup->encryptedBuffer, MAX_PSN_TUS_PROFILE_SIZE, NULL);
					if (ret == PSN_OK)
					{
						//-- PSN task started successfully, wait for async to complete
					}
					else
					{
						DrxLogAlways("sceNpTusGetDataAsync : error %08X", ret);

						if (ret == SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND)
						{
							UpdateTaskError(sTaskID, eCLE_ReadDataNotWritten);
						}
						else
						{
							if (!m_pPSNSupport->HandlePSNError(ret))
							{
								UpdateTaskError(sTaskID, eCLE_InternalError);
							}
						}
					}
				}
			}
			else
			{
				UpdateTaskError(sTaskID, eCLE_OutOfMemory);
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_OutOfMemory);
		}
	}

	if (pTask->error != eCLE_Success)
	{
		AbortTusTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsReadUserDataAsyncPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		int result = 0;
		int ret = sceNpTusPollAsync(pTask->transaction, &result);
		if (ret == PSN_OK)
		{
			//-- success!
			if (result >= MAX_PSN_PROFILE_SETTING_BUFFER_SIZE)
			{
				SProfileDataBufferGroup* pBufferGroup = (SProfileDataBufferGroup*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_USER_DATA_RESULTS]);

				memset(pBufferGroup->unencryptedBuffer, 0, MAX_PSN_PROFILE_SETTING_BUFFER_SIZE);

				UpdateTaskError(sTaskID, DecryptUserData(MAX_PSN_PROFILE_SETTING_BUFFER_SIZE, pBufferGroup->encryptedBuffer, pBufferGroup->unencryptedBuffer));

				if (pTask->error == eCLE_Success)
				{
					FreeTusTransactionHandle(&pTask->transaction);
					UpdateTaskError(sTaskID, eCLE_Success);
					StopTaskRunning(sTaskID);
				}
			}
			else
			{
				if ((result == 0) || (result == SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND))
				{
					UpdateTaskError(sTaskID, eCLE_ReadDataNotWritten);
				}
				else
				{
					if (result > 0)
					{
						UpdateTaskError(sTaskID, eCLE_ReadDataCorrupt);
					}
					else
					{
						if (!m_pPSNSupport->HandlePSNError(result))
						{
							UpdateTaskError(sTaskID, eCLE_InternalError);
						}
					}
				}
			}
		}
		else if (ret < PSN_OK)
		{
			DrxLogAlways("sceNpTusPollAsync() : Error %08X", ret);

			if (ret == SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND)
			{
				UpdateTaskError(sTaskID, eCLE_ReadDataNotWritten);
			}
			else
			{
				if (!m_pPSNSupport->HandlePSNError(ret))
				{
					UpdateTaskError(sTaskID, eCLE_InternalError);
				}
			}
		}
		else
		{
			//-- still running the async task
		}
	}

	if (pTask->error != eCLE_Success)
	{
		AbortTusTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::EndStatsReadUserData(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	if (pTask->error == eCLE_Success)
	{
		UpdateTaskError(sTaskID, CreateTaskParamMem(sTaskID, STATS_PARAM_READ_USER_DATA_GAME_RESULTS, NULL, m_numUserData * sizeof(SDrxLobbyUserData)));

		if (pTask->error == eCLE_Success)
		{
			SDrxLobbyUserData* pResults = (SDrxLobbyUserData*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_USER_DATA_GAME_RESULTS]);
			SProfileDataBufferGroup* pBufferGroup = (SProfileDataBufferGroup*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_READ_USER_DATA_RESULTS]);

			CDrxSharedLobbyPacket packet;

			packet.SetReadBuffer(pBufferGroup->unencryptedBuffer, MAX_PSN_PROFILE_SETTING_BUFFER_SIZE);
			packet.SetReadBufferPos(0);

			for (uint32 j = 0; j < m_numUserData; j++)
			{
				SDrxLobbyUserData* pResult = &pResults[j];
				SRegisterUserData* pRegistered = &m_userData[j];

				pResult->m_id = pRegistered->id;
				pResult->m_type = (EDrxLobbyUserDataType)pRegistered->type;
				packet.ReadDrxLobbyUserData(pResult);
			}

			if (pTask->error == eCLE_Success)
			{
				((DrxStatsReadUserDataCallback)pTask->pCb)(pTask->lTaskID, pTask->error, pResults, m_numUserData, pTask->pCbArg);
			}
		}
	}

	if (pTask->error != eCLE_Success)
	{
		((DrxStatsReadUserDataCallback)pTask->pCb)(pTask->lTaskID, pTask->error, NULL, 0, pTask->pCbArg);
	}
}

			#define STATS_PARAM_VERIFY_STRING 0

void CDrxPSNStats::TickStatsVerifyString(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		if (pTask->paramsMem[STATS_PARAM_VERIFY_STRING] != TMemInvalidHdl)
		{
			uint8* pBuffer = (uint8*)m_pLobby->MemGetPtr(pTask->paramsMem[STATS_PARAM_VERIFY_STRING]);

			UpdateTaskError(sTaskID, CreateScoreTransactionHandle(&pTask->transaction));
			if (pTask->error == eCLE_Success)
			{
				StartSubTask(eST_StatsVerifyStringAsyncPolling, sTaskID);

				int ret = sceNpScoreCensorCommentAsync(pTask->transaction, (const char*)pBuffer, NULL);
				if (ret == PSN_OK)
				{
					//-- PSN task started successfully, wait for async to complete
				}
				else
				{
					DrxLogAlways("sceNpScoreCensorCommentAsync : error %08X", ret);
					UpdateTaskError(sTaskID, eCLE_InternalError);
				}
			}
		}
		else
		{
			UpdateTaskError(sTaskID, eCLE_OutOfMemory);
		}
	}

	if (pTask->error != eCLE_Success)
	{
		AbortScoreTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

void CDrxPSNStats::TickStatsVerifyStringAsyncPolling(DrxStatsTaskID sTaskID)
{
	STask* pTask = &m_task[sTaskID];

	m_pPSNSupport->ResumeTransitioning(ePSNOS_Online);
	if (m_pPSNSupport->HasTransitioningReachedState(ePSNOS_Online))
	{
		int result = 0;
		int ret = sceNpScorePollAsync(pTask->transaction, &result);
		if (ret == PSN_OK)
		{
			if (result == SCE_NP_COMMUNITY_SERVER_ERROR_CENSORED)
			{
				UpdateTaskError(sTaskID, eCLE_InvalidParam);
			}
			else
			{
				UpdateTaskError(sTaskID, eCLE_Success);
			}
			//-- success!
			FreeScoreTransactionHandle(&pTask->transaction);

			StopTaskRunning(sTaskID);
		}
		else if (ret < PSN_OK)
		{
			DrxLogAlways("sceNpScorePollAsync() : Error %08X", ret);
			UpdateTaskError(sTaskID, eCLE_InternalError);
		}
		else
		{
			//-- still running the async task
		}
	}

	if (pTask->error != eCLE_Success)
	{
		AbortScoreTransactionHandle(&pTask->transaction);
		StopTaskRunning(sTaskID);
	}
}

		#endif // USE_DRX_STATS
	#endif   //USE_PSN
#endif     // DRX_PLATFORM_ORBIS
