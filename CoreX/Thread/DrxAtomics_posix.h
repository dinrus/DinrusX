// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <sched.h>
#include <DinrusX/CoreX/Assert/DrxAssert.h>

//////////////////////////////////////////////////////////////////////////
// Interlocked API
//////////////////////////////////////////////////////////////////////////

// Returns the resulting incremented value
ILINE LONG DrxInterlockedIncrement(volatile int* pDst)
{
	static_assert(sizeof(int) == sizeof(LONG), "Unsecured cast. int is not same size as LONG.");
	return __sync_add_and_fetch(pDst, 1);
}

// Returns the resulting decremented value
ILINE LONG DrxInterlockedDecrement(volatile int* pDst)
{
	static_assert(sizeof(int) == sizeof(LONG), "Unsecured cast. int is not same size as LONG.");
	return __sync_sub_and_fetch(pDst, 1);
}

// Returns the resulting added value
ILINE LONG DrxInterlockedAdd(volatile LONG* pDst, LONG add)
{
	return __sync_add_and_fetch(pDst, add);
}

// Returns the resulting added value
ILINE int64 DrxInterlockedAdd(volatile int64* pDst, int64 add)
{
	return __sync_add_and_fetch(pDst, add);
}

// Returns the resulting added value
ILINE size_t DrxInterlockedAdd(volatile size_t* pDst, size_t add)
{
	return __sync_add_and_fetch(pDst, add);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchange(volatile LONG* pDst, LONG exchange)
{
#if !(DRX_PLATFORM_X64 || DRX_PLATFORM_X86)
	__sync_synchronize(); // follow X64 standard and ensure full memory barrier (this also adds mfence)
#endif
	return __sync_lock_test_and_set(pDst, exchange); // only creates acquire memory barrier

}

// Returns initial value prior exchange
ILINE int64 DrxInterlockedExchange64(volatile int64* addr, int64 exchange)
{
	__sync_synchronize();
	return __sync_lock_test_and_set(addr, exchange);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeAdd(volatile LONG* pDst, LONG value)
{
	return __sync_fetch_and_add(pDst, value);
}

// Returns initial value prior exchange
ILINE size_t DrxInterlockedExchangeAdd(volatile size_t* pDst, size_t add)
{
	return __sync_fetch_and_add(pDst, add);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeAnd(volatile LONG* pDst, LONG value)
{
	return __sync_fetch_and_and(pDst, value);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeOr(volatile LONG* pDst, LONG value)
{
	return __sync_fetch_and_or(pDst, value);
}

// Returns initial address prior exchange
ILINE void* DrxInterlockedExchangePointer(void* volatile* pDst, void* pExchange)
{
	__sync_synchronize();                             // follow X86/X64 standard and ensure full memory barrier
	return __sync_lock_test_and_set(pDst, pExchange); // only creates acquire memory barrier
}

// Returns initial address prior exchange
ILINE LONG DrxInterlockedCompareExchange(volatile LONG* pDst, LONG exchange, LONG comperand)
{
	return __sync_val_compare_and_swap(pDst, comperand, exchange);
}

// Returns initial address prior exchange
ILINE int64 DrxInterlockedCompareExchange64(volatile int64* addr, int64 exchange, int64 comperand)
{
	return __sync_val_compare_and_swap(addr, comperand, exchange);
}

#if DRX_PLATFORM_64BIT
// Returns initial address prior exchange
// Chipset needs to support cmpxchg16b which most do
//https://blog.lse.epita.fr/articles/42-implementing-generic-double-word-compare-and-swap-.html
ILINE unsigned char DrxInterlockedCompareExchange128(volatile int64* pDst, int64 exchangehigh, int64 exchangelow, int64* pComparandResult)
{
	#if DRX_PLATFORM_IOS
		#error Ensure DrxInterlockedCompareExchange128 is working on IOS also
	#endif
	DRX_ASSERT_MESSAGE((((int64)pDst) & 15) == 0, "The destination data must be 16-byte aligned to avoid a general protection fault.");
	#if DRX_PLATFORM_X64 || DRX_PLATFORM_X86
		bool bEquals;
		__asm__ __volatile__(
		"lock cmpxchg16b %1\n\t"
		"setz %0"
		: "=q" (bEquals), "+m" (*pDst), "+d" (pComparandResult[1]), "+a" (pComparandResult[0])
		: "c" (exchangehigh), "b" (exchangelow)
		: "cc");
		return (char)bEquals;
	#else
		// Use lock for targeted CPU architecture that does not support DCAS/CAS2/MCAS
		static pthread_mutex_t mutex;
		bool bResult = false;
		pthread_mutex_lock (&mutex);
		if (pDst[0] == pComparandResult[0] && pDst[1] == pComparandResult[1])
		{
			pDst[0] = exchangelow;
			pDst[1] = exchangehigh;
			bResult = true;
		}
		else
		{
			pComparandResult[0] = pDst[0];
			pComparandResult[1] = pDst[1];
		}
		pthread_mutex_unlock (&mutex);
		return bResult;
	#endif
}
#endif

// Returns initial address prior exchange
ILINE void* DrxInterlockedCompareExchangePointer(void* volatile* pDst, void* pExchange, void* pComperand)
{
	return __sync_val_compare_and_swap(pDst, pComperand, pExchange);
}

#if DRX_PLATFORM_64BIT
//////////////////////////////////////////////////////////////////////////
// Linux 64-bit implementation of lockless single-linked list
//////////////////////////////////////////////////////////////////////////
typedef __uint128_t uint128;

//////////////////////////////////////////////////////////////////////////
// Implementation for Linux64 with gcc using __int128_t
//////////////////////////////////////////////////////////////////////////
ILINE void DrxInterlockedPushEntrySList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& element)
{
	uint64 curSetting[2];
	uint64 newSetting[2];
	uint64 newPointer = (uint64) & element;
	do
	{
		curSetting[0] = (uint64)list.pNext;
		curSetting[1] = list.salt;
		element.pNext = (SLockFreeSingleLinkedListEntry*)curSetting[0];
		newSetting[0] = newPointer;        // new pointer
		newSetting[1] = curSetting[1] + 1; // new salt
	}
	// while (false == __sync_bool_compare_and_swap( (volatile uint128*)&list.pNext,*(uint128*)&curSetting[0],*(uint128*)&newSetting[0] ));
	while (0 == DrxInterlockedCompareExchange128((volatile int64*)&list.pNext, (int64)newSetting[1], (int64)newSetting[0], (int64*)&curSetting[0]));
}

//////////////////////////////////////////////////////////////////////////
ILINE void DrxInterlockedPushListSList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& first, SLockFreeSingleLinkedListEntry& last, uint32 count)
{
	(void)count; // unused

	uint64 curSetting[2];
	uint64 newSetting[2];
	uint64 newPointer = (uint64) & first;
	do
	{
		curSetting[0] = (uint64)list.pNext;
		curSetting[1] = list.salt;
		last.pNext = (SLockFreeSingleLinkedListEntry*)curSetting[0];
		newSetting[0] = newPointer;        // new pointer
		newSetting[1] = curSetting[1] + 1; // new salt
	}
	while (0 == DrxInterlockedCompareExchange128((volatile int64*)&list.pNext, (int64)newSetting[1], (int64)newSetting[0], (int64*)&curSetting[0]));
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxInterlockedPopEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	uint64 curSetting[2];
	uint64 newSetting[2];
	do
	{
		curSetting[1] = list.salt;
		curSetting[0] = (uint64)list.pNext;
		if (curSetting[0] == 0)
			return NULL;
		newSetting[0] = *(uint64*)curSetting[0]; // new pointer
		newSetting[1] = curSetting[1] + 1;       // new salt
	}
	//while (false == __sync_bool_compare_and_swap( (volatile uint128*)&list.pNext,*(uint128*)&curSetting[0],*(uint128*)&newSetting[0] ));
	while (0 == DrxInterlockedCompareExchange128((volatile int64*)&list.pNext, (int64)newSetting[1], (int64)newSetting[0], (int64*)&curSetting[0]));
	return (void*)curSetting[0];
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxRtlFirstEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	return list.pNext;
}

//////////////////////////////////////////////////////////////////////////
ILINE void DrxInitializeSListHead(SLockFreeSingleLinkedListHeader& list)
{
	list.salt = 0;
	list.pNext = NULL;
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxInterlockedFlushSList(SLockFreeSingleLinkedListHeader& list)
{
	uint64 curSetting[2];
	uint64 newSetting[2];
	uint64 newSalt;
	uint64 newPointer;
	do
	{
		curSetting[1] = list.salt;
		curSetting[0] = (uint64)list.pNext;
		if (curSetting[0] == 0)
			return NULL;
		newSetting[0] = 0;
		newSetting[1] = curSetting[1] + 1;
	}
	//	while (false == __sync_bool_compare_and_swap( (volatile uint128*)&list.pNext,*(uint128*)&curSetting[0],*(uint128*)&newSetting[0] ));
	while (0 == DrxInterlockedCompareExchange128((volatile int64*)&list.pNext, (int64)newSetting[1], (int64)newSetting[0], (int64*)&curSetting[0]));
	return (void*)curSetting[0];
}
//////////////////////////////////////////////////////////////////////////
#elif DRX_PLATFORM_32BIT
//////////////////////////////////////////////////////////////////////////
// Implementation for Linux32 with gcc using uint64
//////////////////////////////////////////////////////////////////////////
ILINE void DrxInterlockedPushEntrySList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& element)
{
	uint32 curSetting[2];
	uint32 newSetting[2];
	uint32 newPointer = (uint32) & element;
	do
	{
		curSetting[0] = (uint32)list.pNext;
		curSetting[1] = list.salt;
		element.pNext = (SLockFreeSingleLinkedListEntry*)curSetting[0];
		newSetting[0] = newPointer;        // new pointer
		newSetting[1] = curSetting[1] + 1; // new salt
	}
	while (false == __sync_bool_compare_and_swap((volatile uint64*)&list.pNext, *(uint64*)&curSetting[0], *(uint64*)&newSetting[0]));
}

//////////////////////////////////////////////////////////////////////////
ILINE void DrxInterlockedPushListSList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& first, SLockFreeSingleLinkedListEntry& last, uint32 count)
{
	(void)count; //unused

	uint32 curSetting[2];
	uint32 newSetting[2];
	uint32 newPointer = (uint32) & first;
	do
	{
		curSetting[0] = (uint32)list.pNext;
		curSetting[1] = list.salt;
		last.pNext = (SLockFreeSingleLinkedListEntry*)curSetting[0];
		newSetting[0] = newPointer;        // new pointer
		newSetting[1] = curSetting[1] + 1; // new salt
	}
	while (false == __sync_bool_compare_and_swap((volatile uint64*)&list.pNext, *(uint64*)&curSetting[0], *(uint64*)&newSetting[0]));
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxInterlockedPopEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	uint32 curSetting[2];
	uint32 newSetting[2];
	do
	{
		curSetting[1] = list.salt;
		curSetting[0] = (uint32)list.pNext;
		if (curSetting[0] == 0)
			return NULL;
		newSetting[0] = *(uint32*)curSetting[0]; // new pointer
		newSetting[1] = curSetting[1] + 1;       // new salt
	}
	while (false == __sync_bool_compare_and_swap((volatile uint64*)&list.pNext, *(uint64*)&curSetting[0], *(uint64*)&newSetting[0]));
	return (void*)curSetting[0];
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxRtlFirstEntrySList(SLockFreeSingleLinkedListHeader& list)
{
	return list.pNext;
}

//////////////////////////////////////////////////////////////////////////
ILINE void DrxInitializeSListHead(SLockFreeSingleLinkedListHeader& list)
{
	list.salt = 0;
	list.pNext = NULL;
}

//////////////////////////////////////////////////////////////////////////
ILINE void* DrxInterlockedFlushSList(SLockFreeSingleLinkedListHeader& list)
{
	uint32 curSetting[2];
	uint32 newSetting[2];
	uint32 newSalt;
	uint32 newPointer;
	do
	{
		curSetting[1] = list.salt;
		curSetting[0] = (uint32)list.pNext;
		if (curSetting[0] == 0)
			return NULL;
		newSetting[0] = 0;
		newSetting[1] = curSetting[1] + 1;
	}
	while (false == __sync_bool_compare_and_swap((volatile uint64*)&list.pNext, *(uint64*)&curSetting[0], *(uint64*)&newSetting[0]));
	return (void*)curSetting[0];
}
#endif

//////////////////////////////////////////////////////////////////////////
// Helper
//////////////////////////////////////////////////////////////////////////

class CSimpleThreadBackOff
{
public:
	static const uint32 kSoftYieldInterval = 0x3FF;
	static const uint32 kHardYieldInterval = 0x1FFF;

public:
	CSimpleThreadBackOff() : m_counter(0) {}

	void reset() { m_counter = 0; }

	void backoff()
	{
#if !DRX_PLATFORM_ANDROID
		_mm_pause();
#endif

		if (!(++m_counter & kHardYieldInterval))
		{
			// give other threads with other prio right to run
			usleep(1);
		}
		else if (!(m_counter & kSoftYieldInterval))
		{
			// give threads with same prio chance to run
			sched_yield();
		}
	}

private:
	int m_counter;
};
