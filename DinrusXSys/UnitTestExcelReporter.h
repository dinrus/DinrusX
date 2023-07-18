// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   UnitTestExcelReporter.h
//  Created:     19/03/2008 by Timur.
//  Описание: Implementation of the DinrusX Unit Testing framework
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DinrusX/DinrusXSys/DrxUnitTest.h>
#include <DinrusX/DinrusXSys/ExcelExport.h>
#include <DinrusX/CoreX/String/DrxString.h>
#include <DinrusX/DinrusXSys/ILog.h>

namespace DrxUnitTest
{
//! Writes multiple excel documents for detailed results
class CUnitTestExcelReporter : public CExcelExportBase, public IUnitTestReporter
{
public:
	explicit CUnitTestExcelReporter(ILog& log) : m_log(log) {}

protected:
	virtual void OnStartTesting(const SUnitTestRunContext& context) override {}
	virtual void OnFinishTesting(const SUnitTestRunContext& context) override;
	virtual void OnSingleTestStart(const IUnitTest& test) override;
	virtual void OnSingleTestFinish(const IUnitTest& test, float fRunTimeInMs, bool bSuccess, char const* szFailureDescription) override;

	virtual void PostFinishTesting(const SUnitTestRunContext& context, bool bSavedReports) const {}

	bool         SaveJUnitCompatableXml();

	ILog& m_log;

	struct STestResult
	{
		CUnitTestInfo testInfo;
		SAutoTestInfo autoTestInfo;
		float         fRunTimeInMs;
		bool          bSuccess;
		string        failureDescription;
	};
	std::vector<STestResult> m_results;
};

//! Extends Excel reporter by opening failed report
class CUnitTestExcelNotificationReporter : public CUnitTestExcelReporter
{
public:
	using CUnitTestExcelReporter::CUnitTestExcelReporter;
protected:
	virtual void PostFinishTesting(const SUnitTestRunContext& context, bool bSavedReports) const override;
};
}
