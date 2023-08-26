// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "DrxThread_posix.h"

//////////////////////////////////////////////////////////////////////////
// DrxEvent(Timed) implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DrxEventTimed::Reset()
{
	m_lockNotify.Lock();
	m_flag = false;
	m_lockNotify.Unlock();
}

//////////////////////////////////////////////////////////////////////////
void DrxEventTimed::Set()
{
	m_lockNotify.Lock();
	m_flag = true;
	m_cond.Notify();
	m_lockNotify.Unlock();
}

//////////////////////////////////////////////////////////////////////////
void DrxEventTimed::Wait()
{
	m_lockNotify.Lock();
	if (!m_flag)
		m_cond.Wait(m_lockNotify);
	m_flag = false;
	m_lockNotify.Unlock();
}

//////////////////////////////////////////////////////////////////////////
bool DrxEventTimed::Wait(const uint32 timeoutMillis)
{
	bool bResult = true;
	m_lockNotify.Lock();
	if (!m_flag)
		bResult = m_cond.TimedWait(m_lockNotify, timeoutMillis);
	m_flag = false;
	m_lockNotify.Unlock();
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// DrxCriticalSection implementation
///////////////////////////////////////////////////////////////////////////////
typedef DrxLockT<DRXLOCK_RECURSIVE> TCritSecType;

void DrxDeleteCriticalSection(void* cs)
{
	delete ((TCritSecType*)cs);
}

void DrxEnterCriticalSection(void* cs)
{
	((TCritSecType*)cs)->Lock();
}

bool DrxTryCriticalSection(void* cs)
{
	return false;
}

void DrxLeaveCriticalSection(void* cs)
{
	((TCritSecType*)cs)->Unlock();
}

void DrxCreateCriticalSectionInplace(void* pCS)
{
	new(pCS) TCritSecType;
}

void DrxDeleteCriticalSectionInplace(void*)
{
}

void* DrxCreateCriticalSection()
{
	return (void*) new TCritSecType;
}
