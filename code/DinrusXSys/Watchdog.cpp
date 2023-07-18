// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/Watchdog.h>
#include <DinrusX/CoreX/Thread/IThreadUpr.h>

void CWatchdogThread::SignalStopWork()
{
	m_bQuit = true;
}

CWatchdogThread::~CWatchdogThread()
{
	SignalStopWork();
}

void CWatchdogThread::SetTimeout(int timeOutSeconds)
{
	DRX_ASSERT(timeOutSeconds > 0);
	m_timeOutSeconds = timeOutSeconds;
}

CWatchdogThread::CWatchdogThread(int timeOutSeconds)
	: m_timeOutSeconds(timeOutSeconds)
{
	DRX_ASSERT(timeOutSeconds > 0);
	if (!gEnv->pThreadUpr->SpawnThread(this, "Watch Dog"))
	{
		DRX_ASSERT_MESSAGE(false, "Error spawning \"Watch Dog\" thread.");
	}
}

uint64 CWatchdogThread::GetSystemUpdateCounter()
{
	DRX_ASSERT(GetISystem());
	return GetISystem()->GetUpdateCounter();
}

void CWatchdogThread::ThreadEntry()
{
	uint64 prevUpdateCounter = GetSystemUpdateCounter();
	Sleep();
	while (!m_bQuit)
	{
		auto curUpdateCounter = GetSystemUpdateCounter();
		if (prevUpdateCounter != curUpdateCounter)
		{
			prevUpdateCounter = curUpdateCounter;
		}
		else
		{
			DrxFatalError("Freeze detected. Watchdog timeout.");
		}
		Sleep();
	}
}

void CWatchdogThread::Sleep() const
{
	DRX_ASSERT(m_timeOutSeconds > 0);
	DrxSleep(1000 * m_timeOutSeconds);
}
