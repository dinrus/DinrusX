// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "TestSuiteTestFixture.h"
#include "../DrxUnitString.h"

DRX_TEST_FIXTURE(CTestListenerFixture, CTestSuiteTestFixtureBase, TestDrxUnitSuite)
{
	class CMockTestListener : public DrxUnit::ITestListener
	{
	public:
		CMockTestListener()
			: m_TestSuiteIsRun(false)
			, m_TestSuiteEndIsCalled(false)
			, m_Line(0)
			, m_StartRunTestInfo(CreateEmptyTestInfo())
			, m_RunTestInfo(CreateEmptyTestInfo())
			, m_PassedTestInfo(CreateEmptyTestInfo())
			, m_FailedTestInfo(CreateEmptyTestInfo())
			, m_TestSkipped(CreateEmptyTestInfo())
		{
		}

		virtual void TestSuiteRun(const DrxUnit::ITestSuite& testSuite)
		{
			m_TestSuiteIsRun = true;
			m_TestSuiteRunName.Assign(testSuite.GetName());
		}

		virtual void TestSuiteEnd(const DrxUnit::ITestSuite& testSuite)
		{
			m_TestSuiteEndIsCalled = true;
		}

		virtual void StartTestRun(const DrxUnit::STestInfo& testInfo)
		{
			m_StartRunTestInfo = testInfo;
		}

		virtual void TestRun(const DrxUnit::STestInfo& testInfo)
		{
			m_RunTestInfo = testInfo;
		}

		virtual void TestPassed(const DrxUnit::STestInfo& testInfo)
		{
			m_PassedTestInfo = testInfo;
		}

		virtual void TestFailed(const DrxUnit::STestInfo& testInfo, const DrxUnit::SFailureInfo& failureInfo)
		{
			m_FailedTestInfo = testInfo;
			m_ErrorMessage.Assign(failureInfo.Message);
			m_FileName.Assign(failureInfo.FileName);
			m_Line = failureInfo.FileLineNumber;
		}

		virtual void TestSkipped(const DrxUnit::STestInfo& testInfo)
		{
			m_TestSkipped = testInfo;
		}

		bool               m_TestSuiteIsRun;
		bool               m_TestSuiteEndIsCalled;

		DrxUnit::String    m_TestSuiteRunName;
		DrxUnit::String    m_ErrorMessage;
		DrxUnit::String    m_FileName;

		int                m_Line;

		DrxUnit::STestInfo m_StartRunTestInfo;
		DrxUnit::STestInfo m_RunTestInfo;
		DrxUnit::STestInfo m_PassedTestInfo;
		DrxUnit::STestInfo m_FailedTestInfo;
		DrxUnit::STestInfo m_TestSkipped;
	};

public:
	virtual void SetUp()
	{
		m_listener = new CMockTestListener();
	}

	virtual void TearDown()
	{
		delete m_listener;
	}

	virtual CMockTestListener& GetListener()
	{
		return *m_listener;
	}

	CMockTestListener* m_listener;
};

// -----------------------------------------------------------------------------

DRX_TEST_WITH_FIXTURE(GetNameFromListenerForPassingTest, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CPassingDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(CPassingDummyTest::Info().Name, GetListener().m_PassedTestInfo.Name);
}

DRX_TEST_WITH_FIXTURE(GetNameFromListenerForRunTest, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CPassingDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(CPassingDummyTest::Info().Name, GetListener().m_RunTestInfo.Name);
}

DRX_TEST_WITH_FIXTURE(GetNameFromListenerForFailedTest, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CFailingDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(GetListener().m_FailedTestInfo.Name, CFailingDummyTest::Info().Name);
}

DRX_TEST_WITH_FIXTURE(TestSuiteStartRunIsNotifiedToListener, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CFailingDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(GetListener().m_StartRunTestInfo.Name, CFailingDummyTest::Info().Name);
}

DRX_TEST_WITH_FIXTURE(TestSuiteRunIsNotifiedToListener, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CFailingDummyTest::Info());
	RunTests();

	ASSERT_IS_TRUE(GetListener().m_TestSuiteIsRun);
}

DRX_TEST_WITH_FIXTURE(GetErrorMessageFromListenerForFailedTest, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CFailingEqualityDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(GetListener().m_ErrorMessage.GetInternalData(), "AssertAreEqual failed: Expected: '2', got: '3'");
}

DRX_TEST_WITH_FIXTURE(GetFileNameFromListenerForFailedTest, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CFailingEqualityDummyTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(GetListener().m_FileName.GetInternalData(), "file");
	ASSERT_ARE_EQUAL(GetListener().m_Line, 10);
}

DRX_TEST_WITH_FIXTURE(TestSuiteEndIsNotifiedFromListener, CTestListenerFixture)
{
	RunTests();
	ASSERT_IS_TRUE(GetListener().m_TestSuiteEndIsCalled);
}

DRX_TEST_WITH_FIXTURE(GetTestSuiteNameFromListener, CTestListenerFixture)
{
	RunTests();
	ASSERT_ARE_EQUAL(GetListener().m_TestSuiteRunName.GetInternalData(), m_TestSuite.GetName());
}

DRX_TEST_WITH_FIXTURE(TestSuiteRunTestSkippedIsNotifiedToListener, CTestListenerFixture)
{
	m_TestSuite.RegisterTestCase(CPassingDummyDisabledTest::Info());
	RunTests();

	ASSERT_ARE_EQUAL(CPassingDummyDisabledTest::Info().Name, GetListener().m_TestSkipped.Name);
}
