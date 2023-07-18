// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxSignIn.cpp
//  Version:     v1.00
//  Created:     14/11/2011 by Paul Mikell.
//  Описание: CDrxSignIn member definitions
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DrxSignIn.h"

CDrxSignIn::CDrxSignIn(CDrxLobby* pLobby, CDrxLobbyService* pService)
	: m_pLobby(pLobby)
	, m_pService(pService)
{
}

CDrxSignIn::~CDrxSignIn()
{
}

void CDrxSignIn::CancelTask(DrxLobbyTaskID lTaskID)
{
	LOBBY_AUTO_LOCK;

	if (lTaskID != DrxLobbyInvalidTaskID)
	{
		for (DrxSignInTaskID siTaskID = 0; siTaskID.IsValid(); ++siTaskID)
		{
			STask* pTask = m_pTask[siTaskID];

			DRX_ASSERT_MESSAGE(pTask, "CDrxSignIn: Task base pointers not setup");

			if (pTask && pTask->used && (pTask->lTaskID == lTaskID))
			{
				NetLog("[Lobby] SignIn task %u cancelled", lTaskID);
				pTask->pCb = NULL;
				pTask->cancelled = true;
				break;
			}
		}
	}
}

EDrxLobbyError CDrxSignIn::Initialise()
{
	for (DrxSignInTaskID siTaskID = 0; siTaskID.IsValid(); ++siTaskID)
	{
		ClearTask(siTaskID);
	}

	return eCLE_Success;
}

EDrxLobbyError CDrxSignIn::Terminate()
{
	for (DrxSignInTaskID siTaskID = 0; siTaskID.IsValid(); ++siTaskID)
	{
		FreeTask(siTaskID);
	}

	return eCLE_Success;
}

void CDrxSignIn::Tick(CTimeValue tv)
{
}

void CDrxSignIn::StartTaskRunning(DrxSignInTaskID siTaskID)
{
	STask* pTask = m_pTask[siTaskID];

	if (pTask->used)
	{
		pTask->running = true;
	}
}

void CDrxSignIn::StopTaskRunning(DrxSignInTaskID siTaskID)
{
	STask* pTask = m_pTask[siTaskID];

	if (pTask->used)
	{
		pTask->running = false;

		TO_GAME_FROM_LOBBY(&CDrxSignIn::EndTask, this, siTaskID);
	}
}

void CDrxSignIn::EndTask(DrxSignInTaskID siTaskID)
{
	LOBBY_AUTO_LOCK;

	FreeTask(siTaskID);
}

EDrxLobbyError CDrxSignIn::StartTask(uint32 eTask, DrxSignInTaskID* pSITaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg)
{
	EDrxLobbyError result = eCLE_TooManyTasks;
	DrxLobbyTaskID lTaskID = m_pLobby->CreateTask();

	if (lTaskID != DrxLobbyInvalidTaskID)
	{
		for (DrxSignInTaskID siTaskID = 0; siTaskID < MAX_SIGNIN_TASKS; ++siTaskID)
		{
			STask* pTask = m_pTask[siTaskID];

			DRX_ASSERT_MESSAGE(pTask, "CDrxSignIn: Task base pointers not setup");

			if (!pTask->used)
			{
				CreateTask(siTaskID, lTaskID, eTask, pCb, pCbArg);

				if (pSITaskID)
				{
					*pSITaskID = siTaskID;
				}

				if (pLTaskID)
				{
					*pLTaskID = lTaskID;
				}

				result = eCLE_Success;
			}
		}

		if (result != eCLE_Success)
		{
			m_pLobby->ReleaseTask(lTaskID);
		}
	}

	return result;
}

void CDrxSignIn::FreeTask(DrxSignInTaskID siTaskID)
{
	STask* pTask = m_pTask[siTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxSignIn: Task base pointers not setup");

	if (pTask && pTask->used)
	{
		for (uint32 i = 0; i < MAX_SIGNIN_PARAMS; ++i)
		{
			if (pTask->paramsMem[i] != TMemInvalidHdl)
			{
				m_pLobby->MemFree(pTask->paramsMem[i]);
				pTask->paramsMem[i] = TMemInvalidHdl;
			}

			pTask->paramsNum[i] = 0;
		}

		m_pLobby->ReleaseTask(pTask->lTaskID);
		pTask->used = false;
	}
}

EDrxLobbyError CDrxSignIn::CreateTaskParamMem(DrxSignInTaskID siTaskID, uint32 param, const void* pParamData, size_t paramDataSize)
{
	EDrxLobbyError result;
	STask* pTask = m_pTask[siTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxSignIn: Task base pointers not setup");

	if (paramDataSize > 0)
	{
		pTask->paramsMem[param] = m_pLobby->MemAlloc(paramDataSize);

		if (pTask->paramsMem[param] != TMemInvalidHdl)
		{
			if (pParamData)
			{
				memcpy(m_pLobby->MemGetPtr(pTask->paramsMem[param]), pParamData, paramDataSize);
			}

			result = eCLE_Success;
		}
		else
		{
			result = eCLE_OutOfMemory;
		}
	}
	else
	{
		result = eCLE_InvalidParam;
	}

	return result;
}

void CDrxSignIn::UpdateTaskError(DrxSignInTaskID siTaskID, EDrxLobbyError error)
{
	STask* pTask = m_pTask[siTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxSignIn: Task base pointers not setup");

	if (pTask->error == eCLE_Success)
	{
		pTask->error = error;
	}
}

void CDrxSignIn::CreateTask(DrxSignInTaskID siTaskID, DrxLobbyTaskID lTaskID, uint32 eTask, void* pCb, void* pCbArg)
{
	STask* pTask = m_pTask[siTaskID];

	pTask->lTaskID = lTaskID;
	pTask->error = eCLE_Success;
	pTask->startedTask = eTask;
	pTask->subTask = eTask;
	pTask->pCb = pCb;
	pTask->pCbArg = pCbArg;
	pTask->used = true;
	pTask->running = false;
	pTask->cancelled = false;

	for (uint32 j = 0; j < MAX_SIGNIN_PARAMS; ++j)
	{
		pTask->paramsMem[j] = TMemInvalidHdl;
		pTask->paramsNum[j] = 0;
	}
}

void CDrxSignIn::ClearTask(DrxSignInTaskID siTaskID)
{
	STask* pTask = m_pTask[siTaskID];

	pTask->lTaskID = DrxLobbyInvalidTaskID;
	pTask->error = eCLE_Success;
	pTask->startedTask = eT_Invalid;
	pTask->subTask = eT_Invalid;
	pTask->pCb = NULL;
	pTask->pCbArg = NULL;
	pTask->used = false;
	pTask->running = false;
	pTask->cancelled = false;

	for (uint32 j = 0; j < MAX_SIGNIN_PARAMS; ++j)
	{
		pTask->paramsMem[j] = TMemInvalidHdl;
		pTask->paramsNum[j] = 0;
	}
}
