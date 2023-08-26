// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

template<typename TValue, typename TAllocator>
void RadixSort(
  /*OUT*/ uint32* pRanksBegin, uint32* pRanksEnd,
  /*IN*/ const TValue* pValuesBegin, const TValue* pValuesEnd,
  /*IN*/ TAllocator& allocator = TAllocator());

#include "RadixSortImpl.h"
