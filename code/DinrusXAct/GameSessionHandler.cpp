// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Дефолтная имплементация обработчика сессии.

   -------------------------------------------------------------------------
   History:
   - 08:12:2009 : Created By Ben Johnson

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/CoreX/Platform/IPlatformOS.h>
#include <DinrusX/DinrusXAct/GameSessionHandler.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>

//-------------------------------------------------------------------------
CGameSessionHandler::CGameSessionHandler()
{
}

//-------------------------------------------------------------------------
CGameSessionHandler::~CGameSessionHandler()
{
}

//-------------------------------------------------------------------------
void CGameSessionHandler::JoinSessionFromConsole(DrxSessionID sessionID)
{
}

//-------------------------------------------------------------------------
int CGameSessionHandler::EndSession()
{
	return 1;
}

//-------------------------------------------------------------------------
int CGameSessionHandler::StartSession()
{
	return 1;
}

//-------------------------------------------------------------------------
void CGameSessionHandler::LeaveSession()
{
}

//-------------------------------------------------------------------------
void CGameSessionHandler::OnUserQuit()
{
}

//-------------------------------------------------------------------------
void CGameSessionHandler::OnGameShutdown()
{
}

//-------------------------------------------------------------------------
DrxSessionHandle CGameSessionHandler::GetGameSessionHandle() const
{
	return DrxSessionInvalidHandle;
}

//-------------------------------------------------------------------------
bool CGameSessionHandler::ShouldMigrateNub() const
{
	return true;
}

//-------------------------------------------------------------------------
bool CGameSessionHandler::IsMultiplayer() const
{
	return false;
}

//-------------------------------------------------------------------------
int CGameSessionHandler::GetNumberOfExpectedClients()
{
	return 0;
}

//-------------------------------------------------------------------------
bool CGameSessionHandler::IsGameSessionMigrating() const
{
	return false;
}

//-------------------------------------------------------------------------
bool CGameSessionHandler::IsMidGameLeaving() const
{
	return false;
}

bool CGameSessionHandler::ShouldCallMapCommand(const char* pLevelName, const char* pGameRules)
{
	IPlatformOS* pPlatform = gEnv->pSystem->GetPlatformOS();

	SStreamingInstallProgress progress;
	pPlatform->QueryStreamingInstallProgressForLevel(pLevelName, &progress);
	const bool bLevelReady = SStreamingInstallProgress::eState_Completed == progress.m_state;
	if (!bLevelReady)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LEVEL_NOT_READY, (UINT_PTR)pLevelName, 0);
	}
	return bLevelReady;
}
