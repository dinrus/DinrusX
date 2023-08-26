// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/String/DrxFixedString.h>
#include <DinrusX/CoreX/Serialization/Forward.h>

// Note : if you are looking for the DrxStringT serialization, it is handled in Serialization/STL.h

template<size_t N>
bool Serialize(Serialization::IArchive& ar, DrxFixedStringT<N>& value, const char* name, const char* label);

template<size_t N>
bool Serialize(Serialization::IArchive& ar, DrxStackStringT<char, N>& value, const char* name, const char* label);

template<size_t N>
bool Serialize(Serialization::IArchive& ar, DrxStackStringT<wchar_t, N>& value, const char* name, const char* label);

#include <DinrusX/CoreX/Serialization/DrxStringsImpl.h>
