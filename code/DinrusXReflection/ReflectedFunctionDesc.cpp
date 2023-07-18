// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "ReflectedFunctionDesc.h"

#include "ReflectedTypeDesc.h"

namespace Drx {
namespace Reflection {

const ITypeDesc* CReflectedFunctionDesc::GetObjectTypeDesc() const
{
	return static_cast<const ITypeDesc*>(m_pObjectType);
}

} // ~Reflection namespace
} // ~Drx namespace
