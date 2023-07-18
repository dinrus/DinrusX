// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>

namespace Schematyc
{

struct IQuickSearchOptions
{
	virtual ~IQuickSearchOptions() {}

	virtual uint32      GetCount() const = 0;
	virtual const char* GetLabel(uint32 optionIdx) const = 0;
	virtual const char* GetDescription(uint32 optionIdx) const = 0;
	virtual const char* GetHeader() const = 0;
	virtual const char* GetDelimiter() const = 0;
};

} // Schematyc
