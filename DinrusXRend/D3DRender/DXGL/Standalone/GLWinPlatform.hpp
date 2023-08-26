// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   GLWinPlatform.hpp
//  Version:     v1.00
//  Created:     27/03/2014 by Valerio Guagliumi.
//  Описание: Platform specific DXGL requirements implementation relying
//               on Windows API
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GLWINPLATFORM__
#define __GLWINPLATFORM__

#include <DinrusX/DinrusXRend/GLCrossPlatform.hpp>
#include <cassert>
#include <direct.h>

namespace NDrxOpenGL
{

namespace NWinPlatformImpl
{

struct SCriticalSection
{
	CRITICAL_SECTION m_kCriticalSection;

	SCriticalSection()
	{
		InitializeCriticalSection(&m_kCriticalSection);
	}

	~SCriticalSection()
	{
		DeleteCriticalSection(&m_kCriticalSection);
	}

	void Lock()
	{
		EnterCriticalSection(&m_kCriticalSection);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_kCriticalSection);
	}
};

}

inline void BreakUnique(const char* szFile, uint32 uLine)
{
	LogMessage(eLS_Warning, "Break at %s(%d)", szFile, uLine);
	if (IsDebuggerPresent())
	{
		::DebugBreak();
	}
}

inline LONG Exchange(LONG volatile* piDestination, LONG iExchange)
{
	return InterlockedExchange(piDestination, iExchange);
}

inline LONG CompareExchange(LONG volatile* piDestination, LONG iExchange, LONG iComparand)
{
	return InterlockedCompareExchange(piDestination, iExchange, iComparand);
}

inline LONG AtomicIncrement(LONG volatile* piDestination)
{
	return InterlockedIncrement(piDestination);
}

inline LONG AtomicDecrement(LONG volatile* piDestination)
{
	return InterlockedDecrement(piDestination);
}

typedef NWinPlatformImpl::SCriticalSection TCriticalSection;

inline void LockCriticalSection(TCriticalSection* pCriticalSection)
{
	pCriticalSection->Lock();
}

inline void UnlockCriticalSection(TCriticalSection* pCriticalSection)
{
	pCriticalSection->Unlock();
}

typedef SLIST_HEADER SLockFreeSingleLinkedListHeader;
typedef SLIST_ENTRY  SLockFreeSingleLinkedListEntry;

inline void DrxInterlockedPushEntrySList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& element)
{
	InterlockedPushEntrySList(alias_cast<PSLIST_HEADER>(&list), alias_cast<PSLIST_ENTRY>(&element));
}

inline void* DrxInterlockedPopEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	return reinterpret_cast<void*>(InterlockedPopEntrySList(alias_cast<PSLIST_HEADER>(&list)));
}

inline void DrxInitializeSListHead(SLockFreeSingleLinkedListHeader& list)
{
	InitializeSListHead(alias_cast<PSLIST_HEADER>(&list));
}

inline void* DrxInterlockedFlushSList(SLockFreeSingleLinkedListHeader& list)
{
	return InterlockedFlushSList(alias_cast<PSLIST_HEADER>(&list));
}

inline void* DrxModuleMemalign(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

inline void DrxModuleMemalignFree(void* memblock)
{
	return _aligned_free(memblock);
}

inline bool MakeDir(const char* szDirectory)
{
	return _mkdir(szDirectory) == 0;
}

}

#endif //__GLWINPLATFORM__
