// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DrxSystem/DrxUnitTest.h>
#include "../CallStackBase.h"

DRX_TEST_WITH_FIXTURE(TestCallStackElementDefaultValues, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;

	ASSERT_ARE_EQUAL("Unknown", callStackElement.GetFunctionName());
	ASSERT_ARE_EQUAL("Unknown", callStackElement.GetModuleName());
	ASSERT_ARE_EQUAL("Unknown", callStackElement.GetFileName());
	ASSERT_ARE_EQUAL(0, callStackElement.GetFileLineNumber());
}

DrxUnit::CallStackElement CreateCallStackElement(const char* functionName, const char* fileName)
{
	DrxUnit::CallStackElement callStackElement;

	callStackElement.SetFunctionName(functionName);
	callStackElement.SetFileName(fileName);

	return callStackElement;
}

DrxUnit::CallStackElement CreateCallStackElement(const char* functionName)
{
	DrxUnit::CallStackElement callStackElement;

	callStackElement.SetFunctionName(functionName);

	return callStackElement;
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementFunctionName, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement = CreateCallStackElement("FunctionName");

	ASSERT_ARE_EQUAL("FunctionName", callStackElement.GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementModuleName, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;

	callStackElement.SetModuleName("ModuleName");
	ASSERT_ARE_EQUAL("ModuleName", callStackElement.GetModuleName());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementFileName, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;
	callStackElement.SetFileName("FileName");
	ASSERT_ARE_EQUAL("FileName", callStackElement.GetFileName());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementFileLineNumber, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;

	callStackElement.SetFileLineNumber(12);
	ASSERT_ARE_EQUAL(12, callStackElement.GetFileLineNumber());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementIsUnknownByDefault, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;
	ASSERT_IS_TRUE(callStackElement.IsUnknown());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementIsUnknownIfUnknownFunctionName, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;
	callStackElement.SetModuleName("module");
	callStackElement.SetFileName("fileName");
	ASSERT_IS_TRUE(callStackElement.IsUnknown());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementIsknownIfUnknownModule, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement = CreateCallStackElement("function", "fileName");
	ASSERT_IS_FALSE(callStackElement.IsUnknown());
}

DRX_TEST_WITH_FIXTURE(TestCallStackElementIsUnknownIfUnknownFileName, TestDrxUnitFixture)
{
	DrxUnit::CallStackElement callStackElement;
	callStackElement.SetFunctionName("function");
	callStackElement.SetModuleName("module");
	ASSERT_IS_TRUE(callStackElement.IsUnknown());
}

DRX_TEST_WITH_FIXTURE(TestCallStackIsEmptyByDefault, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;
	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestCallStackHasSizeOneWhenInsertingAnElement, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	DrxUnit::CallStackElement element = CreateCallStackElement("functionName", "fileName");
	callStack.Add(element);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestCallStackHasSizeZeroWhenInsertingAnUnknownElement, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;
	callStack.Add(DrxUnit::CallStackElement());
	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestCallStackHasTwoElements, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));

	ASSERT_ARE_EQUAL(2, callStack.GetSize());
	ASSERT_ARE_EQUAL("function1", callStack.GetElementByIndex(0).GetFunctionName());
	ASSERT_ARE_EQUAL("function2", callStack.GetElementByIndex(1).GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestFilterFirstElement, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));

	callStack.FilterTopElements(1);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
	ASSERT_ARE_EQUAL("function2", callStack.GetElementByIndex(0).GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestFilterFirstTwoElements, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));
	callStack.Add(CreateCallStackElement("function3", ""));

	callStack.FilterTopElements(2);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
	ASSERT_ARE_EQUAL("function3", callStack.GetElementByIndex(0).GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestFilterFirstTwoElementsWhenSizeIsTwo, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));

	callStack.FilterTopElements(2);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterFirstTwoElementsWhenSizeIsOne, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));

	callStack.FilterTopElements(2);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterFirstElementWhenSizeIsZero, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.FilterTopElements(1);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterZeroElementWhenSizeIsZero, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.FilterTopElements(0);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterZeroElementWhenSizeIsOne, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;
	callStack.Add(CreateCallStackElement("function1", ""));

	callStack.FilterTopElements(0);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomFirstElement, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));

	callStack.FilterBottomElements(1);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
	ASSERT_ARE_EQUAL("function1", callStack.GetElementByIndex(0).GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomFirstTwoElements, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));
	callStack.Add(CreateCallStackElement("function3", ""));

	callStack.FilterBottomElements(2);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
	ASSERT_ARE_EQUAL("function1", callStack.GetElementByIndex(0).GetFunctionName());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomFirstTwoElementsWhenSizeIsTwo, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));
	callStack.Add(CreateCallStackElement("function2", ""));

	callStack.FilterBottomElements(2);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomFirstTwoElementsWhenSizeIsOne, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.Add(CreateCallStackElement("function1", ""));

	callStack.FilterBottomElements(2);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomFirstElementWhenSizeIsZero, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.FilterBottomElements(1);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomZeroElementWhenSizeIsZero, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;

	callStack.FilterBottomElements(0);

	ASSERT_ARE_EQUAL(0, callStack.GetSize());
}

DRX_TEST_WITH_FIXTURE(TestFilterBottomZeroElementWhenSizeIsOne, TestDrxUnitFixture)
{
	DrxUnit::CallStackBase callStack;
	callStack.Add(CreateCallStackElement("function1", ""));

	callStack.FilterBottomElements(0);

	ASSERT_ARE_EQUAL(1, callStack.GetSize());
}
