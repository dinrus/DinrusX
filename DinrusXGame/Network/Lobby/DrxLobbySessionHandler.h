// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: DrxLobby session handler implementation.

-------------------------------------------------------------------------
History:
- 08:12:2009 : Created By Ben Johnson

*************************************************************************/
#ifndef __DRXLOBBY_SESSION_HANDLER_H__
#define __DRXLOBBY_SESSION_HANDLER_H__

#include <IGameSessionHandler.h>
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>

class CDrxLobbySessionHandler : public IGameSessionHandler
{
public:
	CDrxLobbySessionHandler();
	virtual ~CDrxLobbySessionHandler();

	// IGameSessionHandler
	virtual bool ShouldCallMapCommand(const char *pLevelName, const char *pGameRules);
	virtual void JoinSessionFromConsole(DrxSessionID session);
	virtual void LeaveSession();
	
	virtual int StartSession();
	virtual int EndSession();

	virtual void OnUserQuit();
	virtual void OnGameShutdown();

	virtual DrxSessionHandle GetGameSessionHandle() const;
	virtual bool ShouldMigrateNub() const;

	virtual bool IsMultiplayer() const;
	virtual int GetNumberOfExpectedClients();

	virtual bool IsGameSessionMigrating() const;
	virtual bool IsMidGameLeaving() const;
	virtual bool IsGameSessionMigratable() const;
	// ~IGameSessionHandler

protected:
	bool m_userQuit;
};

#endif //__DRXLOBBY_SESSION_HANDLER_H__
