// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

struct IScriptStruct : public IScriptElementBase<EScriptElementType::Struct>
{
	virtual ~IScriptStruct() {}

	virtual uint32       GetFieldCount() const = 0;
	virtual const char*  GetFieldName(uint32 fieldIdx) const = 0;
	virtual CAnyConstPtr GetFieldValue(uint32 fieldIdx) const = 0;
};
} // Schematyc
