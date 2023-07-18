// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXFRIENDSMANAGEMENT_H__
#define __DRXFRIENDSMANAGEMENT_H__

#pragma once

<DinrusX/CoreX/Lobby/IDrxFriendsManagement.h>
#include "DrxLobby.h"
#include "DrxMatchMaking.h"

#define MAX_FRIENDS_MANAGEMENT_PARAMS 3
#define MAX_FRIENDS_MANAGEMENT_TASKS  4

typedef uint32 DrxFriendsManagementTaskID;
const DrxFriendsManagementTaskID DrxFriendsManagementInvalidTaskID = 0xffffffff;

class CDrxFriendsManagement : public CMultiThreadRefCount, public IDrxFriendsManagement
{
public:
	CDrxFriendsManagement(CDrxLobby* pLobby, CDrxLobbyService* pService);

	EDrxLobbyError Initialise();
	EDrxLobbyError Terminate();

	virtual bool   IsDead() const { return false; }

	virtual void   CancelTask(DrxLobbyTaskID lTaskID);

protected:

	struct  STask
	{
		DrxLobbyTaskID lTaskID;
		EDrxLobbyError error;
		uint32         startedTask;
		uint32         subTask;
		void*          pCb;
		void*          pCbArg;
		TMemHdl        paramsMem[MAX_FRIENDS_MANAGEMENT_PARAMS];
		uint32         paramsNum[MAX_FRIENDS_MANAGEMENT_PARAMS];
		bool           used;
		bool           running;
		bool           canceled;
	};

	EDrxLobbyError StartTask(uint32 eTask, bool startRunning, DrxFriendsManagementTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg);
	virtual void   FreeTask(DrxFriendsManagementTaskID fTaskID);
	EDrxLobbyError CreateTaskParamMem(DrxFriendsManagementTaskID fTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	void           UpdateTaskError(DrxFriendsManagementTaskID fTaskID, EDrxLobbyError error);

	STask*            m_pTask[MAX_FRIENDS_MANAGEMENT_TASKS];
	CDrxLobby*        m_pLobby;
	CDrxLobbyService* m_pService;
};

#endif // __DRXFRIENDSMANAGEMENT_H__
