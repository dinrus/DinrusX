
// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/yasli/Enum.h>

namespace yasli {

class Archive;

struct BitFlagsWrapper{
	int* variable;
	const EnumDescription* description;

	void YASLI_SERIALIZE_METHOD(Archive& ar);
};

template<class Enum>
BitFlagsWrapper BitFlags(Enum& value)
{
	BitFlagsWrapper wrapper;
	wrapper.variable = (int*)&value;
	wrapper.description = &getEnumDescription<Enum>();
	return wrapper;
}

template<class Enum>
BitFlagsWrapper BitFlags(int& value)
{
	BitFlagsWrapper wrapper;
	wrapper.variable = &value;
	wrapper.description = &getEnumDescription<Enum>();
	return wrapper;
}

template<class Enum>
BitFlagsWrapper BitFlags(unsigned int& value)
{
	BitFlagsWrapper wrapper;
	wrapper.variable = (int*)&value;
	wrapper.description = &getEnumDescription<Enum>();
	return wrapper;
}

}
