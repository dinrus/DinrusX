// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
//	File: CTPEndpoint.cpp
//  Описание: нить, посылающая поддерживающие активность уведомления зарегистрированным сокетам,
// в случае если глобальный замок удерживается длительное время.
//
//	History:
//	-August 24,2007:Created by Craig Tiller
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <DinrusX/DinrusXNetwork/NetAddress.h>
#include <DinrusX/DinrusXNetwork/DrxSocks.h>

#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <memory>

extern volatile uint32 g_watchdogTimerGlobalLockCount;
extern volatile uint32 g_watchdogTimerLockedCounter;

class CAutoUpdateWatchdogCounters
{
public:
	ILINE CAutoUpdateWatchdogCounters()
	{
		++g_watchdogTimerGlobalLockCount;
		++g_watchdogTimerLockedCounter;
	}
	ILINE ~CAutoUpdateWatchdogCounters()
	{
		--g_watchdogTimerLockedCounter;
	}
};

namespace detail
{
class CTimerThread;
}

class CWatchdogTimer
{
public:
	CWatchdogTimer();
	~CWatchdogTimer();

	void RegisterTarget(DRXSOCKET sock, const TNetAddress& addr);
	void UnregisterTarget(DRXSOCKET sock, const TNetAddress& addr);

	bool HasStalled() const;
	void ClearStalls();

private:
	std::auto_ptr<detail::CTimerThread> m_pThread;
};
