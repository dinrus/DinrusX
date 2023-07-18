// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptEnum : public IScriptElementBase<EScriptElementType::Enum>
{
	virtual ~IScriptEnum() {}

	virtual uint32      GetConstantCount() const = 0;
	virtual uint32      FindConstant(const char* szConstant) const = 0;
	virtual const char* GetConstant(uint32 constantIdx) const = 0;
};
} // Schematyc
