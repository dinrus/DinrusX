// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>
#include <DinrusX/DinrusXSchema2/UnitTests/UnitTestRegistrar.h>

#include <DinrusX/DinrusXSchema2/ILog.h>

namespace Schematyc2
{
	CUnitTestRegistrar::CUnitTestRegistrar(UnitTestFunctionPtr pFuntion, const char* szName)
		: m_pFuntion(pFuntion)
		, m_szName(szName)
	{}

	void CUnitTestRegistrar::RunUnitTests()
	{
		EUnitTestResultFlags allResultFlags = EUnitTestResultFlags::Success;
		for(const CUnitTestRegistrar* pInstance = GetFirstInstance(); pInstance; pInstance = pInstance->GetNextInstance())
		{
			const EUnitTestResultFlags testResultFlags = pInstance->m_pFuntion();
			if(testResultFlags != EUnitTestResultFlags::Success)
			{
				SCHEMATYC2_SYSTEM_ERROR("Failed unit test '%s'!", pInstance->m_szName);
			}
			allResultFlags |= testResultFlags;
		}

		if((allResultFlags & EUnitTestResultFlags::FatalError) != 0)
		{
			SCHEMATYC2_SYSTEM_FATAL_ERROR("Failed one or more unit tests!");
		}
		else if((allResultFlags & EUnitTestResultFlags::CriticalError) != 0)
		{
			SCHEMATYC2_SYSTEM_CRITICAL_ERROR("Failed one or more unit tests!");
		}
	}
}
