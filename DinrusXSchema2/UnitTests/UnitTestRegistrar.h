// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/StaticInstanceList.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_PreprocessorUtils.h>

#define SCHEMATYC2_REGISTER_UNIT_TEST(function, name) static Schematyc2::CUnitTestRegistrar PP_JOIN_XY(schematycUnitTestRegistrar, __COUNTER__)(function, name);

namespace Schematyc2
{
	enum class EUnitTestResultFlags
	{
		Success       = 0,
		CriticalError = BIT(0),
		FatalError    = BIT(1)
	};

	DECLARE_ENUM_CLASS_FLAGS(EUnitTestResultFlags)

	typedef EUnitTestResultFlags (*UnitTestFunctionPtr)(); 

	class CUnitTestRegistrar : public CStaticInstanceList<CUnitTestRegistrar>
	{
	public:

		CUnitTestRegistrar(UnitTestFunctionPtr pFuntion, const char* szName);

		static void RunUnitTests();

	private:

		UnitTestFunctionPtr m_pFuntion;
		const char*         m_szName;
	};
}