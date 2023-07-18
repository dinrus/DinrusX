// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/Delegate.h>

namespace Schematyc
{
namespace EnvUtils
{

// #SchematycTODO : Do we still need IEnvElement::VisitChildren?
inline void VisitChildren(const IEnvElement& envElement, const std::function<void(const IEnvElement&)>& visitor)
{
	SCHEMATYC_CORE_ASSERT(visitor);
	if (visitor)
	{
		for (const IEnvElement* pEnvChildElement = envElement.GetFirstChild(); pEnvChildElement; pEnvChildElement = pEnvChildElement->GetNextSibling())
		{
			visitor(*pEnvChildElement);
		}
	}
}

template<typename TYPE> inline void VisitChildren(const IEnvElement& envElement, const std::function<void(const TYPE&)>& visitor)
{
	SCHEMATYC_CORE_ASSERT(visitor);
	if (visitor)
	{
		for (const IEnvElement* pEnvChildElement = envElement.GetFirstChild(); pEnvChildElement; pEnvChildElement = pEnvChildElement->GetNextSibling())
		{
			const TYPE* pEnvChildElementImpl = DynamicCast<TYPE>(pEnvChildElement);
			if (pEnvChildElementImpl)
			{
				visitor(*pEnvChildElementImpl);
			}
		}
	}
}

} // EnvUtils
} // Schematyc
