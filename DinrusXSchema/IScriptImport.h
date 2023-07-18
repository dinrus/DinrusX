// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include #include  <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptImport : public IScriptElementBase<EScriptElementType::Import>
{
	virtual ~IScriptImport() {}

	virtual SGUID GetModuleGUID() const = 0;
};
} // Schematyc
