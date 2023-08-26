// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "../DrxUnitString.h"

class TestListener : public DrxUnit::ITestListener
{
public:
	TestListener()
		: m_testFailedCalled(false)
	{

	}

	virtual void TestSuiteRun(const DrxUnit::ITestSuite& testSuite) {}
	virtual void TestSuiteEnd(const DrxUnit::ITestSuite& testSuite) {}
	virtual void StartTestRun(const DrxUnit::STestInfo& testInfo)   {}
	virtual void TestRun(const DrxUnit::STestInfo& testInfo)        {}
	virtual void TestPassed(const DrxUnit::STestInfo& testInfo)     {}
	virtual void TestSkipped(const DrxUnit::STestInfo& testInfo)    {}

	virtual void TestFailed(const DrxUnit::STestInfo& testInfo, const DrxUnit::SFailureInfo& failureInfo)
	{
		m_testFailedCalled = true;
		m_testName.Assign(testInfo.Name);
		m_message.Assign(failureInfo.Message);
		m_fileName.Assign(failureInfo.FileName);
		m_line = failureInfo.FileLineNumber;
	}

	const bool TestFailedCalled() const
	{
		return m_testFailedCalled;
	}

	const char* TestName() const
	{
		return m_testName.GetInternalData();
	}

	const char* Message() const
	{
		return m_message.GetInternalData();
	}

	const char* FileName() const
	{
		return m_fileName.GetInternalData();
	}

	const int Line() const
	{
		return m_line;
	}

private:
	bool            m_testFailedCalled;
	DrxUnit::String m_testName;
	DrxUnit::String m_message;
	DrxUnit::String m_fileName;
	int             m_line;
};

class ThrowingExceptionProtector : public DrxUnit::UnhandledExceptionProtector
{
public:
	ThrowingExceptionProtector(const DrxUnit::STestInfo& testInfo, DrxUnit::ITestListener& listener, const char* message)
		: DrxUnit::UnhandledExceptionProtector(testInfo, listener, message)
	{

	}

	virtual void Protected()
	{
#ifndef PS3
		throw "bye!";
#endif
	}
};

class NotThrowingExceptionProtector : public DrxUnit::UnhandledExceptionProtector
{
public:
	NotThrowingExceptionProtector(const DrxUnit::STestInfo& testInfo, DrxUnit::ITestListener& listener, const char* message)
		: DrxUnit::UnhandledExceptionProtector(testInfo, listener, message)
		, m_called(false)
	{

	}

	virtual void Protected()
	{
		m_called = true;
	}

	const bool Called() const
	{
		return m_called;
	}

private:
	bool m_called;
};

DRX_TEST_WITH_FIXTURE(TestWhenUnhandledExceptionProtectorCatches, TestDrxUnitFixture)
{
	TestListener listener;

	ThrowingExceptionProtector throwingExceptionProtector(CreateEmptyTestInfo(), listener, "");
	throwingExceptionProtector.Execute();

	ASSERT_IS_TRUE(listener.TestFailedCalled());
}

DRX_TEST_WITH_FIXTURE(TestParametersWhenUnhandledExceptionProtectorCatches, TestDrxUnitFixture)
{
	DrxUnit::STestInfo testInfo("TestName", "", NULL, true, "FileName", 12);
	TestListener listener;

	ThrowingExceptionProtector throwingExceptionProtector(testInfo, listener, "Message");
	throwingExceptionProtector.Execute();

	ASSERT_ARE_EQUAL(testInfo.Name, listener.TestName());
	ASSERT_ARE_EQUAL(testInfo.FileName, listener.FileName());
	ASSERT_ARE_EQUAL(testInfo.FileLineNumber, listener.Line());
}

DRX_TEST_WITH_FIXTURE(TestUnhandledExceptionProtectorNotCrashing, TestDrxUnitFixture)
{
	TestListener listener;

	NotThrowingExceptionProtector notThrowingExceptionProtector(CreateEmptyTestInfo(), listener, "");
	notThrowingExceptionProtector.Execute();

	ASSERT_IS_FALSE(listener.TestFailedCalled());
}

DRX_TEST_WITH_FIXTURE(TestUnhandledExceptionProtectorProtectedMethodCalled, TestDrxUnitFixture)
{
	TestListener listener;

	NotThrowingExceptionProtector notThrowingExceptionProtector(CreateEmptyTestInfo(), listener, "");
	notThrowingExceptionProtector.Execute();

	ASSERT_IS_TRUE(notThrowingExceptionProtector.Called());
}
#ifndef PS3
DRX_TEST_WITH_FIXTURE(TestUnhandledExceptionProtectorExcecuteDisabled, TestDrxUnitFixture)
{
	TestListener listener;
	ThrowingExceptionProtector throwingExceptionProtector(CreateEmptyTestInfo(), listener, "");
	bool exceptionThrown = false;
	try
	{
		throwingExceptionProtector.Execute();
	}
	catch (...)
	{
		exceptionThrown = true;
	}

	ASSERT_IS_FALSE(exceptionThrown);
	ASSERT_IS_TRUE(listener.TestFailedCalled());
}
#endif
