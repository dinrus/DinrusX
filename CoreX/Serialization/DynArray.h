// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/STL.h>

template<class T, class I, class STORE>
struct DynArray;

template<class T, class I, class S>
bool Serialize(Serialization::IArchive& ar, DynArray<T, I, S>& container, const char* name, const char* label)
{
	Serialization::ContainerSTL<DynArray<T, I, S>, T> ser(&container);
	return ar(static_cast<Serialization::IContainer&>(ser), name, label);
}