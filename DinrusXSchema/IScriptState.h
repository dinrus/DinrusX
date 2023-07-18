// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptState : public IScriptElementBase<EScriptElementType::State>
{
	virtual ~IScriptState() {}

	virtual DrxGUID GetPartnerGUID() const = 0;
};
} // Schematyc
