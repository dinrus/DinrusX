// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ParticleMemory.h
//  Version:     v1.00
//  Created:     18/03/2010 by Corey Spink
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: All the particle system's specific memory needs
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __particlememory_h__
#define __particlememory_h__
#pragma once

#include <DinrusX/DinrusX3dEng/ParticleFixedSizeElementPool.h>

class ParticleObjectPool;
ParticleObjectPool& ParticleObjectAllocator();

#endif // __particlememory_h__
