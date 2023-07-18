// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{
// Forward declare structures.
typedef std::function<void (const DrxGUID&)> ScriptDependencyEnumerator;

enum class EScriptDependencyType
{
	Load,
	Compile,
	Event,
	Reference
};
} // Schematyc
