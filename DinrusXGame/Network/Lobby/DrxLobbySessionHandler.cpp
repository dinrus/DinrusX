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
#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/DrxLobbySessionHandler.h>
#include <DinrusX/DinrusXGame/GameLobby.h>
#include <DinrusX/DinrusXGame/GameLobbyUpr.h>
#include <DinrusX/DinrusXGame/PlayerProgression.h>
#include <DinrusX/DinrusXGame/UI/ProfileOptions.h>

//-------------------------------------------------------------------------
CDrxLobbySessionHandler::CDrxLobbySessionHandler()
{
	g_pGame->GetIGameFramework()->SetGameSessionHandler(this);
	m_userQuit = false;
}

//-------------------------------------------------------------------------
CDrxLobbySessionHandler::~CDrxLobbySessionHandler()
{
	g_pGame->ClearGameSessionHandler(); // Must clear pointer in game if cry action deletes the handler.
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::ShouldCallMapCommand( const char *pLevelName, const char *pGameRules )
{
	bool result = false;

	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		result = pGameLobby->ShouldCallMapCommand(pLevelName, pGameRules);
		if(result)
		{
			IPlatformOS* pPlatform = gEnv->pSystem->GetPlatformOS();

			SStreamingInstallProgress progress;
			pPlatform->QueryStreamingInstallProgressForLevel(pLevelName, &progress);
			const bool bLevelReady = SStreamingInstallProgress::eState_Completed == progress.m_state;
			if(!bLevelReady)
			{
				// we can jump directly into MP from system game invitation avoiding frontend, so update streaming install priorities here
				pPlatform->SwitchStreamingInstallPriorityToLevel(pLevelName);
				gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent( ESYSTEM_EVENT_LEVEL_NOT_READY, (UINT_PTR)pLevelName, 0 );
			}
			return bLevelReady;
		}
	}

	return result;
}
//-------------------------------------------------------------------------
void CDrxLobbySessionHandler::JoinSessionFromConsole(DrxSessionID session)
{
	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		pGameLobby->JoinServer(session, "JoinSessionFromConsole", DrxMatchMakingInvalidConnectionUID, true);
	}
}

//-------------------------------------------------------------------------
void CDrxLobbySessionHandler::LeaveSession()
{
	CGameLobbyUpr* pGameLobbyUpr = g_pGame->GetGameLobbyUpr();
	if (pGameLobbyUpr)
	{
		pGameLobbyUpr->LeaveGameSession(CGameLobbyUpr::eLSR_Menu);
	}
}

//-------------------------------------------------------------------------
int CDrxLobbySessionHandler::StartSession()
{
	m_userQuit = false;
	return (int) eCLE_Success;
}

//-------------------------------------------------------------------------
int CDrxLobbySessionHandler::EndSession()
{
	if (IPlayerProfileUpr *pPlayerProfileUpr = g_pGame->GetIGameFramework()->GetIPlayerProfileUpr())
	{
		CPlayerProgression *pPlayerProgression = CPlayerProgression::GetInstance();
		if (pPlayerProgression)
		{
			pPlayerProgression->OnEndSession();
		}

		CGameLobbyUpr *pLobbyUpr = g_pGame->GetGameLobbyUpr();
		if (pLobbyUpr)
		{
			const unsigned int controllerIndex = pPlayerProfileUpr->GetExclusiveControllerDeviceIndex();
			if (pLobbyUpr->GetOnlineState(controllerIndex) == eOS_SignedIn)
			{
				DrxLog("CDrxLobbySessionHandler::EndSession() saving profile");
				//Quitting the session from in game
				g_pGame->GetProfileOptions()->SaveProfile(ePR_All);
			}
			else
			{
				DrxLog("CDrxLobbySessionHandler::EndSession() not saving as we're signed out");
			}
		}
	}

	return (int) eCLE_Success;
}

//-------------------------------------------------------------------------
void CDrxLobbySessionHandler::OnUserQuit()
{
	m_userQuit = true;

	if (g_pGame->GetIGameFramework()->StartedGameContext() == false)
	{
		g_pGame->GetGameLobbyUpr()->LeaveGameSession(CGameLobbyUpr::eLSR_Menu);
	}
}

//-------------------------------------------------------------------------
void CDrxLobbySessionHandler::OnGameShutdown()
{
	const CGame::EHostMigrationState  migrationState = (g_pGame ? g_pGame->GetHostMigrationState() : CGame::eHMS_NotMigrating);

	DrxLog("CDrxLobbySessionHandler::OnGameShutdown(), m_userQuit=%s, migrationState=%d", (m_userQuit ? "true" : "false"), migrationState);
#if 0 // old frontend
	CMPMenuHub* pMPMenu = NULL;
	CFlashFrontEnd *pFlashFrontEnd = g_pGame ? g_pGame->GetFlashMenu() : NULL;
	if (pFlashFrontEnd)
	{
		pMPMenu = pFlashFrontEnd->GetMPMenu();
	}
#endif
	if(m_userQuit)
	{
		LeaveSession();
	}
#if 0 // old frontend
	if (pMPMenu)
	{
		// If we're still on the loading screen, clear it
		pMPMenu->ClearLoadingScreen();
	}
#endif
}

//-------------------------------------------------------------------------
DrxSessionHandle CDrxLobbySessionHandler::GetGameSessionHandle() const
{
	DrxSessionHandle result = DrxSessionInvalidHandle;

	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		result = pGameLobby->GetCurrentSessionHandle();
	}

	return result;
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::ShouldMigrateNub() const
{
	bool bResult = true;

	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		bResult = pGameLobby->ShouldMigrateNub();
	}

	return bResult;
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::IsMultiplayer() const
{
	bool result = false;

	CGameLobbyUpr *pLobbyUpr = g_pGame->GetGameLobbyUpr();
	if (pLobbyUpr)
	{
		result = pLobbyUpr->IsMultiplayer();
	}

	return result;
}

//-------------------------------------------------------------------------
int CDrxLobbySessionHandler::GetNumberOfExpectedClients()
{
	int result = 0;

	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		result = pGameLobby->GetNumberOfExpectedClients();
	}

	return result;
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::IsGameSessionMigrating() const
{
	return g_pGame->IsGameSessionHostMigrating();
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::IsMidGameLeaving() const
{
	bool result = false;

	const CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		result = pGameLobby->IsMidGameLeaving();
	}

	return result;
}

//-------------------------------------------------------------------------
bool CDrxLobbySessionHandler::IsGameSessionMigratable() const
{
	bool result = false;

	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		result = pGameLobby->IsSessionMigratable();
	}

	return result;
}
