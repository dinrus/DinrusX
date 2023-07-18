// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Default session handler implementation.

   -------------------------------------------------------------------------
   History:
   - 08:12:2009 : Created By Ben Johnson

*************************************************************************/
#ifndef __GAME_SESSION_HANDLER_H__
#define __GAME_SESSION_HANDLER_H__

#include <DinrusX/DinrusXAct/IGameSessionHandler.h>

class CGameSessionHandler : public IGameSessionHandler
{
public:
	CGameSessionHandler();
	virtual ~CGameSessionHandler();

	// IGameSessionHandler
	virtual bool             ShouldCallMapCommand(const char* pLevelName, const char* pGameRules);
	virtual void             JoinSessionFromConsole(DrxSessionID sessionID);
	virtual int              EndSession();

	virtual int              StartSession();
	virtual void             LeaveSession();

	virtual void             OnUserQuit();
	virtual void             OnGameShutdown();

	virtual DrxSessionHandle GetGameSessionHandle() const;
	virtual bool             ShouldMigrateNub() const;

	virtual bool             IsMultiplayer() const;
	virtual int              GetNumberOfExpectedClients();

	virtual bool             IsGameSessionMigrating() const;
	virtual bool             IsMidGameLeaving() const;
	// ~IGameSessionHandler
};

#endif //__GAME_SESSION_HANDLER_H__
