// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 04:05:2012   16:00 : Created by Jan Neugebauer

*************************************************************************/
#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/ThrowIndicator.h>

#include <DinrusX/DinrusXGame/Throw.h>
#include <DinrusX/DinrusXGame/LTagSingle.h>

#include <DinrusX/DinrusXGame/UI/HUD/HUDEventDispatcher.h>

DRX_IMPLEMENT_GTI(CThrowIndicator, CIronSight);

//------------------------------------------------------------------------
CThrowIndicator::CThrowIndicator()
	: m_indicatorShowing(false)
{
}

//------------------------------------------------------------------------
CThrowIndicator::~CThrowIndicator()
{
}

//------------------------------------------------------------------------
bool CThrowIndicator::StartZoom(bool stayZoomed, bool fullZoomout, int zoomStep)
{
	if (m_pWeapon->IsOwnerClient())
	{
		CThrow* pThrow = crygti_cast<CThrow*>(m_pWeapon->GetCFireMode(m_pWeapon->GetCurrentFireMode()));
		if((pThrow != NULL) && !pThrow->OutOfAmmo())
		{
			const float lifetime = pThrow->GetProjectileLifeTime();

			SHUDEvent trajectoryEvent(eHUDEvent_DisplayTrajectory);
			trajectoryEvent.AddData(SHUDEventData(true));
			trajectoryEvent.AddData(SHUDEventData(lifetime));
			CHUDEventDispatcher::CallEvent(trajectoryEvent);
			m_indicatorShowing = true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
void CThrowIndicator::StopZoom()
{
	if (m_pWeapon->IsOwnerClient())
	{
		SHUDEvent trajectoryEvent(eHUDEvent_DisplayTrajectory);
		trajectoryEvent.AddData(SHUDEventData(false));
		trajectoryEvent.AddData(SHUDEventData(0.0f));
		CHUDEventDispatcher::CallEvent(trajectoryEvent);
		m_indicatorShowing = false;
	}
}

//------------------------------------------------------------------------
void CThrowIndicator::ExitZoom(bool force)
{
	StopZoom();
}
