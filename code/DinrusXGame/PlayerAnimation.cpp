// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
Description: Helper interface for player animation control

-------------------------------------------------------------------------
History:
- 08.9.11: Created by Tom Berry

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/PlayerAnimation.h>

SMannequinPlayerParams PlayerMannequin;

void InitPlayerMannequin( IActionController* pActionController )
{
	DRX_ASSERT( g_pGame );
	IGameFramework* pGameFramework = g_pGame->GetIGameFramework();
	DRX_ASSERT( pGameFramework );
	CMannequinUserParamsUpr& mannequinUserParams = pGameFramework->GetMannequinInterface().GetMannequinUserParamsUpr();

	mannequinUserParams.RegisterParams< SMannequinPlayerParams >( pActionController, &PlayerMannequin );
}

