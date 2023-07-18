// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXFRIENDS_H__
#define __DRXFRIENDS_H__

#pragma once

<DinrusX/CoreX/Lobby/IDrxFriends.h>
#include "DrxLobby.h"
#include "DrxMatchMaking.h"

#if USE_DRX_FRIENDS

	#define MAX_FRIENDS_PARAMS 2
	#define MAX_FRIENDS_TASKS  2

typedef uint32 DrxFriendsTaskID;
const DrxFriendsTaskID DrxFriendsInvalidTaskID = 0xffffffff;

class CDrxFriends : public CMultiThreadRefCount, public IDrxFriends
{
public:
	CDrxFriends(CDrxLobby* pLobby, CDrxLobbyService* pService);

	virtual EDrxLobbyError Initialise();
	virtual EDrxLobbyError Terminate();

	virtual bool           IsDead() const { return false; }

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
		TMemHdl               paramsMem[MAX_FRIENDS_PARAMS];
		uint32                paramsNum[MAX_FRIENDS_PARAMS];
		bool                  used;
		bool                  running;
		bool                  canceled;
	};

	EDrxLobbyError StartTask(uint32 eTask, bool startRunning, DrxFriendsTaskID* pFTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	virtual void   FreeTask(DrxFriendsTaskID fTaskID);
	void           CancelTask(DrxLobbyTaskID lTaskID);
	EDrxLobbyError CreateTaskParamMem(DrxFriendsTaskID fTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	void           UpdateTaskError(DrxFriendsTaskID fTaskID, EDrxLobbyError error);

	STask*            m_pTask[MAX_FRIENDS_TASKS];
	CDrxLobby*        m_pLobby;
	CDrxLobbyService* m_pService;
};

#endif // USE_DRX_FRIENDS
#endif // __DRXFRIENDS_H__
