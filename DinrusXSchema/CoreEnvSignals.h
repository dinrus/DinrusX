// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IEnvRegistrar;

struct SStartSignal
{
	static void ReflectType(CTypeDesc<SStartSignal>& desc);
};

struct SStopSignal
{
	static void ReflectType(CTypeDesc<SStopSignal>& desc);
};

struct SUpdateSignal
{
	SUpdateSignal(float _time = 0.0f);

	static void ReflectType(CTypeDesc<SUpdateSignal>& desc);

	float time = 0.0f;
};

void RegisterCoreEnvSignals(IEnvRegistrar& registrar);

}
