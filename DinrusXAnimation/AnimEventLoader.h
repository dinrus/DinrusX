// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

class CDefaultSkeleton;

namespace AnimEventLoader
{
// loads the data from the animation event database file (.animevent) - this is usually
// specified in the CHRPARAMS file.
bool LoadAnimationEventDatabase(CDefaultSkeleton* pDefaultSkeleton, const char* pFileName);

// Toggles preloading of particle effects
void SetPreLoadParticleEffects(bool bPreLoadParticleEffects);
}
