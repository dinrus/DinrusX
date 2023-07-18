// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "../DrxUnitString.h"

DRX_TEST_WITH_FIXTURE(TestEmptyStringByDefault, TestDrxUnitFixture)
{
	DrxUnit::String str;
	ASSERT_ARE_EQUAL("", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestEmptyStringIfConstructorWithEmptyString, TestDrxUnitFixture)
{
	DrxUnit::String str("");
	ASSERT_ARE_EQUAL("", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestConstructorWithSomeString, TestDrxUnitFixture)
{
	DrxUnit::String str("test");
	ASSERT_ARE_EQUAL("test", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAppendEmptyString, TestDrxUnitFixture)
{
	DrxUnit::String str("test");
	str.Append("");

	ASSERT_ARE_EQUAL("test", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAppendString, TestDrxUnitFixture)
{
	DrxUnit::String str("test");
	str.Append(" trying to make it bigger");
	ASSERT_ARE_EQUAL("test trying to make it bigger", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestCopyEmptyString, TestDrxUnitFixture)
{
	DrxUnit::String emptyStr1;
	DrxUnit::String emptyStr2(emptyStr1);

	ASSERT_ARE_EQUAL("", emptyStr2.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestCopyString, TestDrxUnitFixture)
{
	DrxUnit::String str1("test");
	DrxUnit::String str2(str1);

	ASSERT_ARE_EQUAL("test", str2.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAssignOperatorEmptyString, TestDrxUnitFixture)
{
	DrxUnit::String emptyStr1("old");
	DrxUnit::String emptyStr2;
	emptyStr1 = emptyStr2;

	ASSERT_ARE_EQUAL("", emptyStr1.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAssignOperatorString, TestDrxUnitFixture)
{
	DrxUnit::String str1("test with a quite bigger string");
	DrxUnit::String str2("old");
	str2 = str1;

	ASSERT_ARE_EQUAL("test with a quite bigger string", str2.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestStringSelfAssignment, TestDrxUnitFixture)
{
	DrxUnit::String str1("test");
	str1 = str1;

	ASSERT_ARE_EQUAL("test", str1.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAssignEmptyString, TestDrxUnitFixture)
{
	DrxUnit::String emptyStr("old");
	emptyStr.Assign("");

	ASSERT_ARE_EQUAL("", emptyStr.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestAssignString, TestDrxUnitFixture)
{
	DrxUnit::String str;
	str.Assign("test with a quite bigger string");

	ASSERT_ARE_EQUAL("test with a quite bigger string", str.GetInternalData());
}

DRX_TEST_WITH_FIXTURE(TestEmptyStringsAreEqual, TestDrxUnitFixture)
{
	DrxUnit::String str1;
	DrxUnit::String str2;

	ASSERT_ARE_EQUAL(str1, str2);
}

DRX_TEST_WITH_FIXTURE(TestEqualStringsAreEqual, TestDrxUnitFixture)
{
	DrxUnit::String str1("str");
	DrxUnit::String str2("str");

	ASSERT_ARE_EQUAL(str1, str2);
}

DRX_TEST_WITH_FIXTURE(TestSameStringsAreEqual, TestDrxUnitFixture)
{
	DrxUnit::String str("str");

	ASSERT_ARE_EQUAL(str, str);
}

DRX_TEST_WITH_FIXTURE(TestNotEqualStringsAreNotEqual, TestDrxUnitFixture)
{
	DrxUnit::String str1("str1");
	DrxUnit::String str2("str2");

	ASSERT_ARE_NOT_EQUAL(str1, str2);
}
