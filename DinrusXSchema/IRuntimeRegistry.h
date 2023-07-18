// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{
// Forward declare interfaces.
struct IRuntimeClass;

// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(IRuntimeClass)

struct IRuntimeRegistry
{
	virtual ~IRuntimeRegistry() {}

	virtual IRuntimeClassConstPtr GetClass(const DrxGUID& guid) const = 0;
};
} // Schematyc
