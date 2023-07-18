// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Containers/VectorMap.h>
#include <DinrusX/DinrusXSchema/TypeName.h>

namespace Schematyc
{
template<typename BASE_INTERFACE> struct IInterfaceMap
{
public:

	template<typename INTERFACE> inline bool Add(INTERFACE* pInterface)
	{
		return Add(GetTypeName<INTERFACE>(), static_cast<BASE_INTERFACE*>(pInterface));
	}

	template<typename INTERFACE> inline INTERFACE* Query() const
	{
		return static_cast<INTERFACE*>(Query(GetTypeName<INTERFACE>()));
	}

private:

	virtual bool Add(const CTypeName& typeName, BASE_INTERFACE* pInterface) = 0;
	virtual BASE_INTERFACE* Query(const CTypeName& typeName) const = 0;
};
} // Schematyc
