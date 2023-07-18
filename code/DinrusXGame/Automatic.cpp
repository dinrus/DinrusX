// Copyright 2018-2023 DinrusPro / Dinrus Group. ÐÍÖÏ Äèíðóñ.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 11:9:2005   15:00 : Created by Márcio Martins

*************************************************************************/
#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/Automatic.h>
#include <DinrusX/DinrusXGame/Actor.h>
#include <DinrusX/DinrusXGame/ScreenEffects.h>

#include <DinrusX/DinrusXGame/WeaponSharedParams.h>
#include <DinrusX/DinrusXGame/Game.h>


DRX_IMPLEMENT_GTI(CAutomatic, CSingle);


//------------------------------------------------------------------------
CAutomatic::CAutomatic()
{
}

//------------------------------------------------------------------------
CAutomatic::~CAutomatic()
{
}

//------------------------------------------------------------------------
void CAutomatic::StartReload(int zoomed)
{
	m_firing = false;
	BaseClass::StartReload(zoomed);
}

//------------------------------------------------------------------------
void CAutomatic::StartFire()
{
	BaseClass::StartFire();

	if (m_firing)
		m_pWeapon->PlayAction(GetFragmentIds().spin_down_tail);

	m_pWeapon->RequestStartFire();
}
//------------------------------------------------------------------------
void CAutomatic::Update(float frameTime, uint32 frameId)
{
	BaseClass::Update(frameTime, frameId);

	if (m_firing && CanFire(false))
	{
		m_firing = Shoot(true, m_fireParams->fireparams.autoReload);

		if(!m_firing)
		{
			StopFire();
		}
	}
}

//------------------------------------------------------------------------
void CAutomatic::StopFire()
{
	if(m_firing)
		SmokeEffect();

	m_firing = false;

	m_pWeapon->RequestStopFire();

	BaseClass::StopFire();
}

//---------------------------------------------------
void CAutomatic::GetMemoryUsage(IDrxSizer * s) const
{
	s->AddObject(this, sizeof(*this));	
	GetInternalMemoryUsage(s); 
}

void CAutomatic::GetInternalMemoryUsage(IDrxSizer * s) const
{
	CSingle::GetInternalMemoryUsage(s);		// collect memory of parent class
}
