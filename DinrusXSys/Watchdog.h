// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#pragma once
#include <DinrusX/CoreX/Thread/IThreadUpr.h>

//! Runs in its own thread to watch over game freezes longer than user specified time out.
//! Turns time out into fatal error.
class CWatchdogThread : public IThread
{
public:

	void SignalStopWork();

	//! Creates and starts running watchdog thread.
	//! \param timeOutSeconds	time out value in seconds, must be positive
	explicit CWatchdogThread(int timeOutSeconds);
	~CWatchdogThread();

	//! Changes time out value.
	//! \param timeOutSeconds	time out value in seconds, must be positive
	void SetTimeout(int timeOutSeconds);
private:

	static uint64 GetSystemUpdateCounter();
	void          Sleep() const;

	virtual void  ThreadEntry() override;

	volatile bool m_bQuit = false;
	int           m_timeOutSeconds = 0;
};
