// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/CoreX/Serialization/yasli/Serializer.h>
#include <DinrusX/CoreX/Serialization/yasli/KeyValue.h>

#if defined(__GNUC__)
// Reinforce GCC's linker to properly export missing symbols (yasli::Serialize etc.)
// into DinrusSystem library:
	#include <DinrusX/CoreX/Serialization/yasli/ClassFactory.h>
#endif

namespace Serialization
{
typedef yasli::Serializer         SStruct;
typedef yasli::ContainerInterface IContainer;
typedef yasli::PointerInterface   IPointer;
typedef yasli::KeyValueInterface  IKeyValue;
typedef yasli::StringInterface    IString;
typedef yasli::WStringInterface   IWString;
}
