// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/UnitTestRegistrar.h>

#include <DinrusX/DinrusXSchema/ILog.h>

namespace Schematyc
{
CUnitTestRegistrar::CUnitTestRegistrar(UnitTestFunctionPtr pFuntion, const char* szName)
	: m_pFuntion(pFuntion)
	, m_szName(szName)
{}

void CUnitTestRegistrar::RunUnitTests()
{
	UnitTestResultFlags allResultFlags = EUnitTestResultFlags::Success;
	for (const CUnitTestRegistrar* pInstance = GetFirstInstance(); pInstance; pInstance = pInstance->GetNextInstance())
	{
		const UnitTestResultFlags testResultFlags = pInstance->m_pFuntion();
		if (testResultFlags != EUnitTestResultFlags::Success)
		{
			SCHEMATYC_CORE_ERROR("Failed unit test '%s'!", pInstance->m_szName);
		}
		allResultFlags.Add(testResultFlags);
	}

	if (allResultFlags.Check(EUnitTestResultFlags::FatalError))
	{
		SCHEMATYC_CORE_FATAL_ERROR("Failed one or more unit tests!");
	}
	else if (allResultFlags.Check(EUnitTestResultFlags::CriticalError))
	{
		SCHEMATYC_CORE_CRITICAL_ERROR("Failed one or more unit tests!");
	}
}
}
