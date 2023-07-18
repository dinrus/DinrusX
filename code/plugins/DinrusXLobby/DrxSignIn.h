// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxSignIn.h
//  Version:     v1.00
//  Created:     09/11/2011 by Paul Mikell.
//  Описание: CDrxSignIn class definition
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSIGNIN_H__

#define __DRXSIGNIN_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxLobby.h"
<DinrusX/CoreX/Lobby/IDrxSignIn.h>

#define MAX_SIGNIN_PARAMS 10
#define MAX_SIGNIN_TASKS  4

class CDrxSignIn : public CMultiThreadRefCount, public IDrxSignIn
{
public:

	virtual ~CDrxSignIn();

	// IDrxSignIn
	virtual void CancelTask(DrxLobbyTaskID lTaskID);
	// ~IDrxSignIn

	// CWorkQueue::CClassJob*< ..., class U >::U
	virtual bool IsDead() const { return false; }
	// ~CWorkQueue::CClassJob*< ..., class U >::U

	virtual EDrxLobbyError Initialise();
	virtual EDrxLobbyError Terminate();
	virtual void           Tick(CTimeValue tv);

protected:

	enum ETask
	{
		eT_Invalid = -1,
		eT_SignInUser,
		eT_SignOutUser,
		eT_CreateUser,
		eT_CreateAndSignInUser,
		eT_GetUserCredentials
	};

	struct  STask
	{
		DrxLobbyTaskID lTaskID;
		EDrxLobbyError error;
		uint32         startedTask;
		uint32         subTask;
		void*          pCb;
		void*          pCbArg;
		TMemHdl        paramsMem[MAX_SIGNIN_PARAMS];
		uint32         paramsNum[MAX_SIGNIN_PARAMS];
		bool           used;
		bool           running;
		bool           cancelled;
	};

	struct SDrxSignInTaskID {};
	typedef DrxLobbyID<SDrxSignInTaskID, MAX_SIGNIN_TASKS>             DrxSignInTaskID;
	typedef DrxLobbyIDArray<STask*, DrxSignInTaskID, MAX_SIGNIN_TASKS> DrxSignInTaskPtrArray;

	CDrxSignIn(CDrxLobby* pLobby, CDrxLobbyService* pService);

	virtual void   StartTaskRunning(DrxSignInTaskID siTaskID);
	virtual void   StopTaskRunning(DrxSignInTaskID siTaskID);
	virtual void   EndTask(DrxSignInTaskID siTaskID);

	EDrxLobbyError StartTask(uint32 eTask, DrxSignInTaskID* pSITaskID, DrxLobbyTaskID* pLTaskID, void* pCb, void* pCbArg);

	virtual void   FreeTask(DrxSignInTaskID siTaskID);
	EDrxLobbyError CreateTaskParamMem(DrxSignInTaskID siTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	void           UpdateTaskError(DrxSignInTaskID siTaskID, EDrxLobbyError error);
	virtual void   CreateTask(DrxSignInTaskID siTaskID, uint32 eTask, DrxLobbyTaskID lTaskID, void* pCb, void* pCbArg);

	DrxSignInTaskPtrArray m_pTask;
	CDrxLobby*            m_pLobby;
	CDrxLobbyService*     m_pService;

private:

	virtual void ClearTask(DrxSignInTaskID siTaskID);
};

#endif //__DRXSIGNIN_H__
