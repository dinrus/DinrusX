// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/IAuditionMap.h>

namespace Perception
{

inline void ReflectType(Schematyc::CTypeDesc<EStimulusObstructionHandling>& desc)
{
	desc.SetGUID("B741A80F-FBB3-40B2-BCAA-6757DA3B4434"_drx_guid);

	desc.AddConstant(EStimulusObstructionHandling::IgnoreAllObstructions, "IgnoreAllObstructions", "Ignore All Obstructions");
	desc.AddConstant(EStimulusObstructionHandling::RayCastWithLinearFallOff, "RayCastWithLinearFallOff", "Raycast with Linear Falloff");
}
}

namespace PerceptionSystemSchematyc
{
	void Register(Schematyc::IEnvRegistrar& registrar, Schematyc::CEnvRegistrationScope& parentScope);
}