// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

template<class T> class _smart_ptr;

template<class T>
bool Serialize(Serialization::IArchive& ar, _smart_ptr<T>& ptr, const char* name, const char* label);

#include "SmartPtrImpl.h"
