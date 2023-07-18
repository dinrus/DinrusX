// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/RuntimeRegistry.h>

#include <DinrusX/DinrusXSchema/RuntimeClass.h>

#ifdef RegisterClass
#undef RegisterClass
#endif

namespace Schematyc
{
IRuntimeClassConstPtr CRuntimeRegistry::GetClass(const DrxGUID& guid) const
{
	Classes::const_iterator itClass = m_classes.find(guid);
	return itClass != m_classes.end() ? itClass->second : IRuntimeClassConstPtr();
}

void CRuntimeRegistry::RegisterClass(const CRuntimeClassConstPtr& pClass)
{
	SCHEMATYC_CORE_ASSERT(pClass);

	const DrxGUID guid = pClass->GetGUID();
	if (GUID::IsEmpty(guid) || GetClass(guid))
	{
		SCHEMATYC_CORE_CRITICAL_ERROR("Unable to register runtime class!");
		return;
	}

	m_classes.insert(Classes::value_type(guid, pClass));
}

void CRuntimeRegistry::ReleaseClass(const DrxGUID& guid)
{
	m_classes.erase(guid);
}

CRuntimeClassConstPtr CRuntimeRegistry::GetClassImpl(const DrxGUID& guid) const
{
	Classes::const_iterator itClass = m_classes.find(guid);
	return itClass != m_classes.end() ? itClass->second : CRuntimeClassConstPtr();
}

void CRuntimeRegistry::Reset()
{
	m_classes.clear();
}
} // Schematyc
