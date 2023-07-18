// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptClass : public IScriptElementBase<EScriptElementType::Class>
{
	virtual ~IScriptClass() {}

	virtual const char* GetAuthor() const = 0;
	virtual const char* GetDescription() const = 0;
};
} // Schematyc
