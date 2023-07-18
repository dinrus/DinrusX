// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _AUTODETECT_CPU_TESTSUITE_
#define _AUTODETECT_CPU_TESTSUITE_

#pragma once

#if DRX_PLATFORM_WINDOWS

class CCPUTestSuite
{
public:
	int RunTest();
};

#endif // #if DRX_PLATFORM_WINDOWS

#endif // #ifndef _AUTODETECT_CPU_TESTSUITE_
