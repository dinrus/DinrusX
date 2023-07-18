// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  Created:     23/10/2015 by Benjamin Block
//  Описание:
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DinrusX/DinrusXRend/GpuParticleFeatureBase.h>
#include <DinrusX/DinrusXRend/GpuComputeBackend.h>

namespace gpu_pfx2
{
struct CFeatureCollision : public CFeature
{
	static const EGpuFeatureType type = eGpuFeatureType_Collision;

	virtual void Initialize() override;
	virtual void Update(const gpu_pfx2::SUpdateContext& context, CDeviceCommandListRef RESTRICT_REFERENCE commandList) override;
	virtual void InternalSetParameters(const EParameterType type, const SFeatureParametersBase& p) override;

private:
	gpu::CTypedConstantBuffer<SFeatureParametersCollision> m_parameters;
};
}
