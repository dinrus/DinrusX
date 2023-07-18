// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare structures.
struct STimerParams;

struct IScriptTimer : public IScriptElementBase<EScriptElementType::Timer>
{
	virtual ~IScriptTimer() {}

	virtual const char*  GetAuthor() const = 0;
	virtual const char*  GetDescription() const = 0;
	virtual STimerParams GetParams() const = 0;
};
} // Schematyc
