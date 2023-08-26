// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxFriendsManagement.h"

CDrxFriendsManagement::CDrxFriendsManagement(CDrxLobby* pLobby, CDrxLobbyService* pService)
{
	DRX_ASSERT_MESSAGE(pLobby, "CDrxFriendsManagement::CDrxFriendsManagement: Lobby not specified");
	DRX_ASSERT_MESSAGE(pService, "CDrxFriendsManagement::CDrxFriendsManagement: Service not specified");

	m_pLobby = pLobby;
	m_pService = pService;

	for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
	{
		m_pTask[i] = NULL;
	}
}

EDrxLobbyError CDrxFriendsManagement::Initialise()
{
	for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
	{
		DRX_ASSERT_MESSAGE(m_pTask[i], "CDrxFriendsManagement: Task base pointers not setup");
		m_pTask[i]->used = false;
	}

	return eCLE_Success;
}

EDrxLobbyError CDrxFriendsManagement::Terminate()
{
	for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
	{
		DRX_ASSERT_MESSAGE(m_pTask[i], "CDrxFriendsManagement: Task base pointers not setup");

		if (m_pTask[i]->used)
		{
			FreeTask(i);
		}
	}

	return eCLE_Success;
}

EDrxLobbyError CDrxFriendsManagement::StartTask(uint32 eTask, bool startRunning, DrxFriendsManagementTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg)
{
	DrxLobbyTaskID lobbyTaskID = m_pLobby->CreateTask();

	if (lobbyTaskID != DrxLobbyInvalidTaskID)
	{
		for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
		{
			STask* pTask = m_pTask[i];

			DRX_ASSERT_MESSAGE(pTask, "CDrxFriendsManagement: Task base pointers not setup");

			if (!pTask->used)
			{
				pTask->lTaskID = lobbyTaskID;
				pTask->error = eCLE_Success;
				pTask->startedTask = eTask;
				pTask->subTask = eTask;
				pTask->pCb = pCb;
				pTask->pCbArg = pCbArg;
				pTask->used = true;
				pTask->running = startRunning;
				pTask->canceled = false;

				for (uint32 j = 0; j < MAX_FRIENDS_MANAGEMENT_PARAMS; j++)
				{
					pTask->paramsMem[j] = TMemInvalidHdl;
					pTask->paramsNum[j] = 0;
				}

				if (pFTaskID)
				{
					*pFTaskID = i;
				}

				if (pLTaskID)
				{
					*pLTaskID = lobbyTaskID;
				}

				return eCLE_Success;
			}
		}

		m_pLobby->ReleaseTask(lobbyTaskID);
	}

	return eCLE_TooManyTasks;
}

void CDrxFriendsManagement::FreeTask(DrxFriendsManagementTaskID fTaskID)
{
	STask* pTask = m_pTask[fTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxFriendsManagement: Task base pointers not setup");

	for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_PARAMS; i++)
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

void CDrxFriendsManagement::CancelTask(DrxLobbyTaskID lTaskID)
{
	LOBBY_AUTO_LOCK;

	NetLog("[Lobby]Try cancel task %u", lTaskID);

	if (lTaskID != DrxLobbyInvalidTaskID)
	{
		for (uint32 i = 0; i < MAX_FRIENDS_MANAGEMENT_TASKS; i++)
		{
			STask* pTask = m_pTask[i];

			DRX_ASSERT_MESSAGE(pTask, "CDrxFriendsManagement: Task base pointers not setup");

			if (pTask->used && (pTask->lTaskID == lTaskID))
			{
				NetLog("[Lobby] Task %u canceled", lTaskID);
				pTask->pCb = NULL;
				pTask->canceled = true;

				break;
			}
		}
	}
}

EDrxLobbyError CDrxFriendsManagement::CreateTaskParamMem(DrxFriendsManagementTaskID fTaskID, uint32 param, const void* pParamData, size_t paramDataSize)
{
	STask* pTask = m_pTask[fTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxFriendsManagement: Task base pointers not setup");

	if (paramDataSize > 0)
	{
		pTask->paramsMem[param] = m_pLobby->MemAlloc(paramDataSize);
		void* p = m_pLobby->MemGetPtr(pTask->paramsMem[param]);

		if (p)
		{
			if (pParamData)
			{
				memcpy(p, pParamData, paramDataSize);
			}
		}
		else
		{
			return eCLE_OutOfMemory;
		}
	}

	return eCLE_Success;
}

void CDrxFriendsManagement::UpdateTaskError(DrxFriendsManagementTaskID fTaskID, EDrxLobbyError error)
{
	STask* pTask = m_pTask[fTaskID];

	DRX_ASSERT_MESSAGE(pTask, "CDrxStats: Task base pointers not setup");

	if (pTask->error == eCLE_Success)
	{
		pTask->error = error;
	}
}
