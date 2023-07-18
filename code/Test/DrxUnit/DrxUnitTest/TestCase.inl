// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>

// -----------------------------------------------------------------------------

DrxUnit::ITestCase* DummyTestFactoryFunction()
{
	return 0;
}

DRX_TEST_WITH_FIXTURE(TestTestInfoName, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo("TestName", 0, 0, false, 0, 0);

	ASSERT_ARE_EQUAL("TestName", testInfo.Name);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoFixture, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, "FixtureName", 0, false, 0, 0);

	ASSERT_ARE_EQUAL("FixtureName", testInfo.Fixture);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoTestFactory, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, DummyTestFactoryFunction, false, 0, 0);

	ASSERT_ARE_EQUAL((const void*)DummyTestFactoryFunction, (const void*)testInfo.TestFactory);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoEnabled, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, true, 0, 0);

	ASSERT_IS_TRUE(testInfo.Enabled);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoFileName, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, "FileName", 0);

	ASSERT_ARE_EQUAL("FileName", testInfo.FileName);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoFileLineNumber, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 1);

	ASSERT_ARE_EQUAL(1, testInfo.FileLineNumber);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoNameNull, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 0);

	ASSERT_IS_NULL(testInfo.Name);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoFixtureNull, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 0);

	ASSERT_IS_NULL(testInfo.Fixture);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoTestFactoryNull, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 0);

	ASSERT_IS_NULL((const void*)testInfo.TestFactory);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoDisabled, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 0);

	ASSERT_IS_FALSE(testInfo.Enabled);
}

DRX_TEST_WITH_FIXTURE(TestTestInfoFileNameNULL, CTestSuiteTestFixture)
{
	DrxUnit::STestInfo testInfo(0, 0, 0, false, 0, 0);

	ASSERT_IS_NULL(testInfo.FileName);
}
