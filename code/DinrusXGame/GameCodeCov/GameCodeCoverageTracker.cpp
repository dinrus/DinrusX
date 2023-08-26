// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   GameCodeCoverageTracker.cpp
//  Created:     18/06/2008 by Matthew
//  Description: Defines code coverage check points
//               and a central class to track their registration
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/GameCodeCoverage/GameCodeCoverageTracker.h>
#include <DinrusX/DinrusXGame/GameCodeCoverage/GameCodeCoverageUpr.h>

#if ENABLE_GAME_CODE_COVERAGE

CGameCodeCoverageCheckPoint::CGameCodeCoverageCheckPoint( const char * label ) : m_nCount(0), m_psLabel(label)
{
	assert(label);
	CGameCodeCoverageUpr::GetInstance()->Register(this);
}

void CGameCodeCoverageCheckPoint::Touch()
{
	++ m_nCount;
	CGameCodeCoverageUpr::GetInstance()->Hit(this);
}

#endif