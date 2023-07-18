// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

#include <DinrusX/DinrusXSchema/IScriptElement.h>
#include <DinrusX/DinrusXSchema/IScriptBase.h>
#include <DinrusX/DinrusXSchema/IScriptClass.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/IString.h>

namespace Schematyc
{
namespace ScriptUtils
{

// #SchematycTODO : Do we still need IScriptElement::VisitChildren?
inline void VisitChildren(const IScriptElement& scriptElement, const std::function<void(const IScriptElement&)>& visitor)
{
	SCHEMATYC_CORE_ASSERT(visitor);
	if (visitor)
	{
		for (const IScriptElement* pScriptChildElement = scriptElement.GetFirstChild(); pScriptChildElement; pScriptChildElement = pScriptChildElement->GetNextSibling())
		{
			visitor(*pScriptChildElement);
		}
	}
}

template<typename TYPE> inline void VisitChildren(const IScriptElement& scriptElement, const std::function<void(const TYPE&)>& visitor)
{
	SCHEMATYC_CORE_ASSERT(visitor);
	if (visitor)
	{
		for (const IScriptElement* pScriptChildElement = scriptElement.GetFirstChild(); pScriptChildElement; pScriptChildElement = pScriptChildElement->GetNextSibling())
		{
			const TYPE* pScriptChildElementImpl = DynamicCast<TYPE>(pScriptChildElement);
			if (pScriptChildElementImpl)
			{
				visitor(*pScriptChildElementImpl);
			}
		}
	}
}

inline const IScriptClass* GetClass(const IScriptElement& scriptElement)
{
	for (const IScriptElement* pScriptElement = &scriptElement; pScriptElement; pScriptElement = pScriptElement->GetParent())
	{
		const IScriptClass* pScriptClass = DynamicCast<IScriptClass>(pScriptElement);
		if (pScriptClass)
		{
			return pScriptClass;
		}
	}
	return nullptr;
}

inline const IScriptBase* GetBase(const IScriptClass& scriptClass)
{
	for (const IScriptElement* pScriptElement = scriptClass.GetFirstChild(); pScriptElement; pScriptElement = pScriptElement->GetNextSibling())
	{
		const IScriptBase* pScriptBase = DynamicCast<IScriptBase>(pScriptElement);
		if (pScriptBase)
		{
			return pScriptBase;
		}
	}
	return nullptr;
}

inline void QualifyName(IString& output, const IScriptElement& element, EScriptElementType scopeType = EScriptElementType::Root)
{
	output.clear();
	for (const IScriptElement* pScope = &element; pScope; pScope = pScope->GetParent())
	{
		if (pScope->GetType() == scopeType)
		{
			break;
		}
		else
		{
			if (!output.empty())
			{
				output.insert(0, "::");
			}
			output.insert(0, pScope->GetName());
		}
	}
}

} // ScriptUtils
} // Schematyc
