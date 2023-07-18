// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __NETWORK_STALL_TICKER_H__
#define __NETWORK_STALL_TICKER_H__

#pragma once

//--------------------------------------------------------------------------
// a special ticker thread to run during load and unload of levels

#ifdef USE_NETWORK_STALL_TICKER_THREAD

	#include <DinrusX/DinrusXSys/ISystem.h>
	#include <DinrusX/DinrusXSys/ICmdLine.h>

	#include <DinrusX/CoreX/Game/IGameFramework.h>
	#include <DinrusX/CoreX/Thread/IThreadUpr.h>

class CNetworkStallTickerThread : public IThread  //in multiplayer mode
{
public:
	CNetworkStallTickerThread()
	{
		m_threadRunning = true;
	}

	// Start accepting work on thread
	virtual void ThreadEntry();

	// Signals the thread that it should not accept anymore work and exit
	void SignalStopWork()
	{
		m_threadRunning = false;
	}

private:
	volatile bool m_threadRunning;
};

#endif // #ifdef USE_NETWORK_STALL_TICKER_THREAD
//--------------------------------------------------------------------------

#endif
