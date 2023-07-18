// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

template<uint32 StoreStrings, typename THash>
struct SCRCRef;

#include <DinrusX/CoreX/Serialization/Forward.h>

template<uint32 StoreStrings, typename THash>
bool Serialize(Serialization::IArchive& ar, SCRCRef<StoreStrings, THash>& crcRef, const char* name, const char* label);

#include "CRCRefImpl.h"
