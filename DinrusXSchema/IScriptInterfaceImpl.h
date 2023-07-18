// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
struct IScriptInterfaceImpl : public IScriptElementBase<EScriptElementType::InterfaceImpl>
{
	virtual ~IScriptInterfaceImpl() {}

	virtual EDomain GetDomain() const = 0;
	virtual DrxGUID   GetRefGUID() const = 0;
};
} // Schematyc
