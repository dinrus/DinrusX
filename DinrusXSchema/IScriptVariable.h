// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

struct IScriptVariable : public IScriptElementBase<EScriptElementType::Variable>
{
	virtual ~IScriptVariable() {}

	virtual SElementId      GetTypeId() const = 0;
	virtual bool            IsArray() const = 0;
	virtual CAnyConstPtr    GetData() const = 0;
	virtual DrxGUID           GetBaseGUID() const = 0;
	virtual EOverridePolicy GetOverridePolicy() const = 0;
};
} // Schematyc
