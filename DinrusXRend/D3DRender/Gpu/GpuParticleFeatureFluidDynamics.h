// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  Created:     21/10/2015 by Benjamin Block
//  Описание:
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DinrusX/DinrusXRend/GpuParticleFeatureBase.h>

namespace gpu_pfx2
{

struct CFeatureMotionFluidDynamics : public CFeatureWithParameterStruct<SFeatureParametersMotionFluidDynamics>
{
	static const EGpuFeatureType type = eGpuFeatureType_MotionFluidDynamics;

	virtual void Update(const gpu_pfx2::SUpdateContext& context, CDeviceCommandListRef RESTRICT_REFERENCE commandList) override;
	virtual void Initialize() override;
private:
};
}
