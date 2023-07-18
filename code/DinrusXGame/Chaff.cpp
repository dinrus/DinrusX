// Copyright 2018-2023 DinrusPro / Dinrus Group. ÐÍÖÏ Äèíðóñ.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 5:5:2006   15:26 : Created by Márcio Martins

*************************************************************************/
#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/Game.h>
#include <DinrusX/DinrusXGame/Chaff.h>
#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/CoreX/Game/IGameTokens.h>

#include <DinrusX/DinrusXGame/WeaponSystem.h>

VectorSet<CChaff*> CChaff::s_chaffs;

//------------------------------------------------------------------------
CChaff::CChaff()
{
	s_chaffs.insert(this);
	if(s_chaffs.size()>MAX_SPAWNED_CHAFFS)
	{
		if(s_chaffs[0]!=this)
			s_chaffs[0]->Destroy();
		else
			s_chaffs[1]->Destroy(); //Just in case...??
	}
}

//------------------------------------------------------------------------
CChaff::~CChaff()
{
	s_chaffs.erase(this);
}

//------------------------------------------------------------------------

void CChaff::HandleEvent(const SGameObjectEvent &event)
{
	CProjectile::HandleEvent(event);
}

Vec3 CChaff::GetPosition(void)
{
	return m_last;
}