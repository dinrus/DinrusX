// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/GpuParticleFeatureFactory.h>

#include <DinrusX/DinrusXRend/GpuParticleFeatureColor.h>
#include <DinrusX/DinrusXRend/GpuParticleFeatureCollision.h>
#include <DinrusX/DinrusXRend/GpuParticleFeatureField.h>
#include <DinrusX/DinrusXRend/GpuParticleFeatureMotion.h>
#include <DinrusX/DinrusXRend/GpuParticleFeatureFluidDynamics.h>

namespace gpu_pfx2
{

// this dummy is used when a feature is supported on GPU but does not
// need any special treatment (e.g. all the appearance features
struct CFeatureDummy : public CFeature
{
	static const EGpuFeatureType type = eGpuFeatureType_Dummy;
};

CGpuInterfaceFactory::CGpuInterfaceFactory()
{
	memset(m_functions, 0, sizeof(m_functions));
#define X(name) RegisterClass<CFeature ## name>();
	LIST_OF_FEATURE_TYPES
#undef X
}

}
