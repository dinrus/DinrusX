// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// Various class and types that are shared between the various hazard module classes.

#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/HazardShared.h>


using namespace HazardSystem;


// ============================================================================
// ============================================================================
// ============================================================================
//
// -- HazardProjectileID -- HazardProjectileID -- HazardProjectileID --
//
// ============================================================================
// ============================================================================
// ============================================================================


void HazardProjectileID::Serialize(TSerialize ser)
{
	ser.BeginGroup("HazardProjectileID");

	ser.Value("ID", m_ID);

	ser.EndGroup();
}


// ============================================================================
// ============================================================================
// ============================================================================
//
// -- HazardSphereID -- HazardSphereID -- HazardSphereID -- HazardSphereID --
//
// ============================================================================
// ============================================================================
// ============================================================================


void HazardSphereID::Serialize(TSerialize ser)
{
	ser.BeginGroup("HazardSphereID");

	ser.Value("ID", m_ID);

	ser.EndGroup();
}
