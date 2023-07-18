// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "../DrxUnitAsserts.h"
#include "../StringHelpers.h"
using namespace DrxUnit::StringHelpers;

DRX_TEST_WITH_FIXTURE(StringBufferAppendInt, TestDrxUnitFixture)
{
	StringBuffer stringBuffer;
	stringBuffer.Append(12);
	ASSERT_ARE_EQUAL("12", stringBuffer.ToString());
}

DRX_TEST_WITH_FIXTURE(StringBufferAppendUnsinged, TestDrxUnitFixture)
{
	StringBuffer stringBuffer;
	stringBuffer.Append(12u);
	ASSERT_ARE_EQUAL("12", stringBuffer.ToString());
}

DRX_TEST_WITH_FIXTURE(StringBufferAppendCharString, TestDrxUnitFixture)
{
	StringBuffer stringBuffer;
	stringBuffer.Append("str");
	ASSERT_ARE_EQUAL("str", stringBuffer.ToString());
}

DRX_TEST_WITH_FIXTURE(StringBufferAppendFloat, TestDrxUnitFixture)
{
	StringBuffer stringBuffer;
	stringBuffer.Append(12.01f);
	ASSERT_ARE_EQUAL("12.01", stringBuffer.ToString());
}

DRX_TEST_WITH_FIXTURE(StringBufferAppendMixed, TestDrxUnitFixture)
{
	StringBuffer stringBuffer;
	stringBuffer.Append(12.01f);
	stringBuffer.Append(" ");
	stringBuffer.Append("done");
	ASSERT_ARE_EQUAL("12.01 done", stringBuffer.ToString());
}
