// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace Schematyc
{
// Forward declare structures.


struct IGUIDRemapper
{
	virtual ~IGUIDRemapper() {}

	virtual bool  Empty() const = 0;
	virtual void  Bind(const DrxGUID& from, const DrxGUID& to) = 0;
	virtual DrxGUID Remap(const DrxGUID& from) const = 0;
};
} // Schematyc
