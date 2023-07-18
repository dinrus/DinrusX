// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusX3dEng/ParticleCommon.h>
#include <DinrusX/DinrusX3dEng/ParticleAttributes.h>
#include <DinrusX/DinrusX3dEng/ParticleEmitter.h>
#include <DinrusX/DinrusX3dEng/ParticleComponentRuntime.h>
#include <DinrusX/DinrusX3dEng/ParamMod.h>

namespace pfx2
{

SERIALIZATION_ENUM_DECLARE(ETargetSource, ,
                           Self,
                           Parent,
                           Target
                           )

// PFX2_TODO : optimize : GetTarget is very inefficient. Make static dispatch and then vectorize it.

class CTargetSource
{
public:
	CTargetSource(ETargetSource source = ETargetSource::Target);

	void Serialize(Serialization::IArchive& ar);
	Vec3 GetTarget(const CParticleComponentRuntime& runtime, TParticleId particleId, bool isParentId = false);

private:
	Vec3          m_offset;
	ETargetSource m_source;
};

}

#include "TargetImpl.h"
