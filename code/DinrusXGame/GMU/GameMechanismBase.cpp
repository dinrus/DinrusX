// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/GameMechanismBase.h>
#include <DinrusX/DinrusXGame/GameMechanismUpr.h>

CGameMechanismBase::CGameMechanismBase(const char * className)
{
	memset (& m_linkedListPointers, 0, sizeof(m_linkedListPointers));

#if INCLUDE_NAME_IN_GAME_MECHANISMS
	m_className = className;
#endif

	CGameMechanismUpr * manager = CGameMechanismUpr::GetInstance();
	manager->RegisterMechanism(this);
}

CGameMechanismBase::~CGameMechanismBase()
{
	CGameMechanismUpr * manager = CGameMechanismUpr::GetInstance();
	manager->UnregisterMechanism(this);
}
