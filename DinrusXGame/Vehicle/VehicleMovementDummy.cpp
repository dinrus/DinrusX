// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a dummy vehicle movement, for prop vehicles

-------------------------------------------------------------------------
History:
- 05:03:2010: Created by Steve Humphreys

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/VehicleMovementDummy.h>

#include <DinrusX/DinrusXGame/GameCVars.h>

bool CVehicleMovementDummy::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;
	return true;
}

void CVehicleMovementDummy::Release()
{
	delete this;
}

void CVehicleMovementDummy::Physicalize()
{
	SEntityPhysicalizeParams physicsParams(m_pVehicle->GetPhysicsParams());	

	physicsParams.type = PE_RIGID;	  

	m_pVehicle->GetEntity()->Physicalize(physicsParams);
}