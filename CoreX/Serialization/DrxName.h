// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

inline bool Serialize(Serialization::IArchive & ar, class CDrxName & drxName, const char* name, const char* label);

#include "DrxNameImpl.h"
