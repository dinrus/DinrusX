// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptInterface : public IScriptElementBase<EScriptElementType::Interface>
{
	virtual ~IScriptInterface() {}

	virtual const char* GetAuthor() const = 0;
	virtual const char* GetDescription() const = 0;
};
} // Schematyc
