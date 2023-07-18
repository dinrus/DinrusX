// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: CDrxLobbyUI implementation for Durango

   -------------------------------------------------------------------------
   History:
   - 13:06:2013 : Created by Yeonwoon JUNG

*************************************************************************/

#ifndef __DRXDURANGOLIVELOBBYUI_H__
#define __DRXDURANGOLIVELOBBYUI_H__

#pragma once

#include "DrxLobbyUI.h"

#if USE_DURANGOLIVE

class CDrxDurangoLiveLobbyUI : public CDrxLobbyUI
{
public:
	CDrxDurangoLiveLobbyUI(CDrxLobby* pLobby, CDrxLobbyService* pService);
	void                   Tick(CTimeValue tv);

	virtual EDrxLobbyError ShowGamerCard(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                            { return eCLE_InvalidRequest; }
	virtual EDrxLobbyError ShowGameInvite(uint32 user, DrxSessionHandle h, DrxUserID* pUserIDs, uint32 numUserIDs, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg) { return eCLE_InvalidRequest; }
	virtual EDrxLobbyError ShowFriends(uint32 user, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                                                { return eCLE_InvalidRequest; }
	virtual EDrxLobbyError ShowFriendRequest(uint32 user, DrxUserID userID, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)                                        { return eCLE_InvalidRequest; }
	virtual EDrxLobbyError SetRichPresence(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg);

protected:
	enum ETask
	{
		eT_SetRichPresence,
	};

	struct  STask : public CDrxLobbyUI::STask
	{
		uint32 user;
	};

	EDrxLobbyError StartTask(ETask etask, bool startRunning, uint32 user, DrxLobbyUITaskID* pUITaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg);
	void           StartTaskRunning(DrxLobbyUITaskID uiTaskID);
	void           EndTask(DrxLobbyUITaskID uiTaskID);
	void           StopTaskRunning(DrxLobbyUITaskID uiTaskID);

	void           StartSetRichPresence(DrxLobbyUITaskID uiTaskID);

	STask m_task[MAX_LOBBYUI_TASKS];

private:
	typedef std::map<int, wstring> TRichPresence;
	TRichPresence m_richPresence;
};

#endif//USE_DURANGOLIVE

#endif // __DRXDURANGOLIVELOBBYUI_H__
