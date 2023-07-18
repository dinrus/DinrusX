// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "../TestFailedException.h"

#ifdef WIN32

DRX_TEST_WITH_FIXTURE(TestAssertAreEqual, TestDrxUnitFixture)
{
	DrxUnit::AssertAreEqual(10, 10, "", 0);
}

DRX_TEST_WITH_FIXTURE(TestAssertAreEqualForChars, TestDrxUnitFixture)
{
	DrxUnit::AssertAreEqual('a', 'a', "", 0);
}

DRX_TEST_WITH_FIXTURE(TestAssertAreNotEqual, TestDrxUnitFixture)
{
	bool test_failed = false;
	try
	{
		DrxUnit::AssertAreEqual(5, 10, "", 0);
	}
	catch (...)
	{
		test_failed = true;
	}

	ASSERT_IS_TRUE(test_failed);
}

DRX_TEST_WITH_FIXTURE(TestAssertAreEqualForCStrings, TestDrxUnitFixture)
{
	const char str1[] = "XXX";
	const char str2[] = "XXX";
	ASSERT_ARE_EQUAL(str1, str2);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsTrue, TestDrxUnitFixture)
{
	ASSERT_IS_TRUE(true);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsFalse, TestDrxUnitFixture)
{
	ASSERT_IS_FALSE(false);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotTrue, TestDrxUnitFixture)
{
	bool exceptionThrown = false;
	try
	{
		ASSERT_IS_TRUE(false);
	}
	catch (...)
	{
		exceptionThrown = true;
	}

	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotFalse, TestDrxUnitFixture)
{
	bool exceptionThrown = false;
	try
	{
		ASSERT_IS_FALSE(true);
	}
	catch (...)
	{
		exceptionThrown = true;
	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFail, TestDrxUnitFixture)
{
	bool failExceptionThrown = false;
	try
	{
		DRX_UNIT_FAIL();
	}
	catch (const DrxUnit::ITestFailedException&)
	{
		failExceptionThrown = true;
	}

	ASSERT_IS_TRUE(failExceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFailWithMessageForCString, TestDrxUnitFixture)
{
	bool failExceptionThrown = false;
	const char* expectedMessage = "FailMessage";
	try
	{
		DRX_UNIT_FAIL_MESSAGE(expectedMessage);
	}
	catch (const DrxUnit::ITestFailedException& testFailedException)
	{
		failExceptionThrown = true;
		ASSERT_ARE_EQUAL(expectedMessage, testFailedException.GetMessage());
	}

	ASSERT_IS_TRUE(failExceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFailWithMessage, TestDrxUnitFixture)
{
	bool failExceptionThrown = false;
	const char* expectedMessage = "FailMessage";
	try
	{
		DRX_UNIT_FAIL_MESSAGE(expectedMessage);
	}
	catch (const DrxUnit::ITestFailedException& testFailedException)
	{
		failExceptionThrown = true;
		ASSERT_ARE_EQUAL(expectedMessage, testFailedException.GetMessage());
	}

	ASSERT_IS_TRUE(failExceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFailWithMessageAndErrorString, TestDrxUnitFixture)
{
	const char expectedMessage[] = "ErrorMessage";
	try
	{
		DRX_UNIT_FAIL_MESSAGE(expectedMessage);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL(expectedMessage, exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertAreEqualGivesCorrectErrorMessageForInts, TestDrxUnitFixture)
{
	try
	{
		ASSERT_ARE_EQUAL(2, 3);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("AssertAreEqual failed: Expected: '2', got: '3'", exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertAreNotEqualGivesCorrectErrorMessageForInts, TestDrxUnitFixture)
{
	try
	{
		ASSERT_ARE_NOT_EQUAL(2, 2);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("AssertAreNotEqual failed: Expected to not be: '2'", exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertAreEqualGivesCorrectErrorMessageForStrings, TestDrxUnitFixture)
{
	try
	{
		ASSERT_ARE_EQUAL("2", "3");
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("AssertAreEqual failed: Expected: '2', got: '3'", exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertAreNotEqualGivesCorrectErrorMessageForStrings, TestDrxUnitFixture)
{
	try
	{
		ASSERT_ARE_NOT_EQUAL("2", "2");
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("AssertAreNotEqual failed: Expected to not be: '2'", exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertIsTrueGivesCorrectErrorMessageForInts, TestDrxUnitFixture)
{
	try
	{
		ASSERT_IS_TRUE(false);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("AssertIsTrue failed.", exception.GetMessage());
		return;
	}

	DRX_UNIT_FAIL();
}

DRX_TEST_WITH_FIXTURE(TestAssertIsTrueGetsCorrectFileNameAndLineWhenFailing, TestDrxUnitFixture)
{
	try
	{
		DrxUnit::AssertIsTrue(false, "file", 10);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		ASSERT_ARE_EQUAL("file", exception.GetFilePath());
		ASSERT_ARE_EQUAL(10, exception.GetFileLine());
	}
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNullMacro, TestDrxUnitFixture)
{
	ASSERT_IS_NULL((void*)0);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNullMacroFail, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		ASSERT_IS_NULL((void*)1);
	}
	catch (const DrxUnit::ITestFailedException&)
	{
		exceptionThrown = true;
	}

	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotNullMacro, TestDrxUnitFixture)
{
	ASSERT_IS_NOT_NULL((void*)1);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotNullMacroFail, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		ASSERT_IS_NOT_NULL((void*)0);
	}
	catch (const DrxUnit::ITestFailedException&)
	{
		exceptionThrown = true;
	}

	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNullGetsCorrectFileNameAndLineWhenFailing, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		DrxUnit::AssertIsNull((void*)1, "file", 10);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		exceptionThrown = true;
		ASSERT_ARE_EQUAL("file", exception.GetFilePath());
		ASSERT_ARE_EQUAL(10, exception.GetFileLine());
	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotNullGetsCorrectFileNameAndLineWhenFailing, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		DrxUnit::AssertIsNotNull((void*)0, "file", 10);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		exceptionThrown = true;
		ASSERT_ARE_EQUAL("file", exception.GetFilePath());
		ASSERT_ARE_EQUAL(10, exception.GetFileLine());
	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNullFailureMessage, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		ASSERT_IS_NULL((void*)1);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		exceptionThrown = true;
		ASSERT_ARE_EQUAL("AssertIsNull failed: Pointer is not NULL", exception.GetMessage());

	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestAssertIsNotNullFailureMessage, TestDrxUnitFixture)
{
	bool exceptionThrown = false;

	try
	{
		ASSERT_IS_NOT_NULL((void*)0);
	}
	catch (const DrxUnit::ITestFailedException& exception)
	{
		exceptionThrown = true;
		ASSERT_ARE_EQUAL("AssertIsNotNull failed: Pointer is NULL", exception.GetMessage());
	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFailMessageWithMultibyteString, TestDrxUnitFixture)
{
	bool failExceptionThrown = false;
	try
	{
		DrxUnit::Fail("", "message", "", 0);
	}
	catch (const DrxUnit::ITestFailedException& testFailedException)
	{
		failExceptionThrown = true;
		ASSERT_ARE_EQUAL("", testFailedException.GetCondition());
		ASSERT_ARE_EQUAL("message", testFailedException.GetMessage());
		ASSERT_ARE_EQUAL("", testFailedException.GetFilePath());
		ASSERT_ARE_EQUAL(0, testFailedException.GetFileLine());

	}

	ASSERT_IS_TRUE(failExceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFailErrorMessageFileLineWithMultibyteString, TestDrxUnitFixture)
{
	bool failExceptionThrown = false;
	try
	{
		DrxUnit::Fail("condition", "message", "file", 1);
	}
	catch (const DrxUnit::ITestFailedException& testFailedException)
	{
		failExceptionThrown = true;
		ASSERT_ARE_EQUAL("condition", testFailedException.GetCondition());
		ASSERT_ARE_EQUAL("message", testFailedException.GetMessage());
		ASSERT_ARE_EQUAL("file", testFailedException.GetFilePath());
		ASSERT_ARE_EQUAL(1, testFailedException.GetFileLine());
	}

	ASSERT_IS_TRUE(failExceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFloatAreEqualWithEpsilon, TestDrxUnitFixture)
{
	float actual = 1.559f;
	float expected = 1.561f;
	float epsilon = 0.01f;

	DrxUnit::AssertFloatAreEqual(expected, actual, epsilon, "", 0);
}

DRX_TEST_WITH_FIXTURE(TestFloatAreEqualWithEpsilonFails, TestDrxUnitFixture)
{
	float actual = 1.559f;
	float expected = 1.561f;
	float epsilon = 0.001f;

	bool exceptionThrown = false;

	try
	{
		DrxUnit::AssertFloatAreEqual(expected, actual, epsilon, "", 0);
	}
	catch (const DrxUnit::ITestFailedException&)
	{
		exceptionThrown = true;
	}
	ASSERT_IS_TRUE(exceptionThrown);
}

DRX_TEST_WITH_FIXTURE(TestFloatAreEqualFailFileNameLine, TestDrxUnitFixture)
{
	float actual = 1.559f;
	float expected = 1.561f;
	float epsilon = 0.001f;

	try
	{
		DrxUnit::AssertFloatAreEqual(expected, actual, epsilon, "FileName", 12);
		DRX_UNIT_FAIL();
	}
	catch (const DrxUnit::ITestFailedException& testFailedException)
	{
		ASSERT_ARE_EQUAL("", testFailedException.GetCondition());
		ASSERT_ARE_EQUAL("AssertFloatAreEqual failed: Expected: '1.56', got: '1.56'", testFailedException.GetMessage());
		ASSERT_ARE_EQUAL("FileName", testFailedException.GetFilePath());
		ASSERT_ARE_EQUAL(12, testFailedException.GetFileLine());
	}
}

#endif
