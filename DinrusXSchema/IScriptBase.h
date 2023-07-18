// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare structures.
struct SElementId;
// Forward declare classes.
class CAnyConstPtr;

struct IScriptBase : public IScriptElementBase<EScriptElementType::Base>
{
	virtual ~IScriptBase() {}

	virtual SElementId   GetClassId() const = 0;
	virtual CAnyConstPtr GetEnvClassProperties() const = 0;
};
}
