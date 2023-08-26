// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/Signal.h>

namespace Schematyc
{
// Forward declare interfaces.
struct IRuntimeClass;

typedef CSignal<void (const IRuntimeClass&)> ClassCompilationSignal;

struct ICompiler
{
	virtual ~ICompiler() {}

	virtual void                           CompileAll() = 0;
	virtual void                           CompileDependencies(const DrxGUID& guid) = 0;
	virtual ClassCompilationSignal::Slots& GetClassCompilationSignalSlots() = 0;
};
} // Schematyc
