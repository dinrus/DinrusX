// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/StaticInstanceList.h>

#include <DinrusX/DinrusXSchema/EnumFlags.h>
#include <DinrusX/DinrusXSchema/PreprocessorUtils.h>
#include <DinrusX/DinrusXSchema/TypeUtils.h>

#define SCHEMATYC_REGISTER_UNIT_TEST(function, name) static Schematyc::CUnitTestRegistrar SCHEMATYC_PP_JOIN_XY(schematycUnitTestRegistrar, __COUNTER__)(function, name);

namespace Schematyc
{
enum class EUnitTestResultFlags
{
	Success       = 0,
	CriticalError = BIT(0),
	FatalError    = BIT(1)
};

typedef CEnumFlags<EUnitTestResultFlags> UnitTestResultFlags;

typedef UnitTestResultFlags (*           UnitTestFunctionPtr)();

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
