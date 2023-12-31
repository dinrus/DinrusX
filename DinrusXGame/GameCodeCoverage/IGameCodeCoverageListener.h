// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//
//  File name:   IGameCodeCoverageListener.h
//  Created:     18/06/2008 by Tim Furnish
//  Description: Interface class for anything which wants to be informed of
//               code coverage checkpoints being hit
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __I_GAME_CODE_COVERAGE_LISTENER_H_
#define __I_GAME_CODE_COVERAGE_LISTENER_H_

#include <DinrusX/DinrusXGame/GameCodeCoverage/GameCodeCoverageEnabled.h>

#if ENABLE_GAME_CODE_COVERAGE

class CGameCodeCoverageCheckPoint;

class IGameCodeCoverageListener
{
	public:
	virtual void InformCodeCoverageCheckpointHit(CGameCodeCoverageCheckPoint * cp) = 0;
};

#endif // ENABLE_GAME_CODE_COVERAGE

#endif // __I_GAME_CODE_COVERAGE_LISTENER_H_