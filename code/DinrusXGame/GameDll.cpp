// Разработка 2018-2023 DinrusPro / Dinrus Group. ���� ������.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: Game DLL entry point.

	-------------------------------------------------------------------------
	History:
	- 2:8:2004   10:38 : Created by M�rcio Martins

*************************************************************************/
#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/Game.h>
#include <DinrusX/DinrusXGame/GameStartup.h>
#include <DinrusX/DinrusXGame/EditorGame.h>

#include <DinrusX/CoreX/Platform/DrxLibrary.h>

extern "C"
{
	GAME_API IGame *CreateGame(IGameFramework* pGameFramework)
	{
		ModuleInitISystem(pGameFramework->GetISystem(),"DrxGame");

		static char pGameBuffer[sizeof(CGame)];
		return new ((void*)pGameBuffer) CGame();
	}

	GAME_API IGameStartup *CreateGameStartup()
	{
		return CGameStartup::Create();
	}
	GAME_API IEditorGame *CreateEditorGame()
	{
		return new CEditorGame();
	}
}


/*
 * this section makes sure that the framework dll is loaded and cleaned up
 * at the appropriate time
 */

#if !defined(_LIB)

static HMODULE s_frameworkDLL;


static void CleanupFrameworkDLL()
{
	assert( s_frameworkDLL );
	DrxFreeLibrary( s_frameworkDLL );
	s_frameworkDLL = 0;
}

HMODULE GetFrameworkDLL(const char* binariesDir)
{
	MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_Other, 0, "Load %s",GAME_FRAMEWORK_FILENAME );
	if (!s_frameworkDLL)
	{
		if (binariesDir && binariesDir[0])
		{
			string dllName = PathUtil::Make(binariesDir, GAME_FRAMEWORK_FILENAME);
			s_frameworkDLL = DrxLoadLibrary(dllName.c_str());		
		}
		else
		{
			s_frameworkDLL = DrxLoadLibrary(GAME_FRAMEWORK_FILENAME);
		}
		atexit( CleanupFrameworkDLL );
	}
	return s_frameworkDLL;
}

#endif
