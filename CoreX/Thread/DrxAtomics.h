// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

//////////////////////////////////////////////////////////////////////////
// Interlocked API
//////////////////////////////////////////////////////////////////////////

// Returns the resulting incremented value
LONG DrxInterlockedIncrement(int volatile* pDst);

// Returns the resulting decremented value
LONG DrxInterlockedDecrement(int volatile* pDst);

// Returns the resulting added value
LONG DrxInterlockedAdd(volatile LONG* pVal, LONG add);

// Returns the resulting added value
int64 DrxInterlockedAdd(volatile int64* pVal, int64 add);

// Returns the resulting added value
size_t DrxInterlockedAdd(volatile size_t* pVal, size_t add);

//////////////////////////////////////////////////////////////////////////
// Returns initial value prior exchange
LONG DrxInterlockedExchange(volatile LONG* pDst, LONG exchange);

// Returns initial value prior exchange
int64 DrxInterlockedExchange64(volatile int64* addr, int64 exchange);

// Returns initial value prior exchange
LONG DrxInterlockedExchangeAdd(volatile LONG* pDst, LONG value);

// Returns initial value prior exchange
size_t DrxInterlockedExchangeAdd(volatile size_t* pDst, size_t value);

// Returns initial value prior exchange
LONG DrxInterlockedExchangeAnd(volatile LONG* pDst, LONG value);

// Returns initial value prior exchange
LONG DrxInterlockedExchangeOr(volatile LONG* pDst, LONG value);

// Returns initial value prior exchange
void* DrxInterlockedExchangePointer(void* volatile* pDst, void* pExchange);

//////////////////////////////////////////////////////////////////////////
// Returns initial value prior exchange
LONG DrxInterlockedCompareExchange(volatile LONG* pDst, LONG exchange, LONG comperand);

// Returns initial value prior exchange
int64 DrxInterlockedCompareExchange64(volatile int64* pDst, int64 exchange, int64 comperand);

#if DRX_PLATFORM_64BIT
// Returns initial value prior exchange
unsigned char DrxInterlockedCompareExchange128(volatile int64* pDst, int64 exchangeHigh, int64 exchangeLow, int64* comparandResult);
#endif

// Returns initial address prior exchange
void* DrxInterlockedCompareExchangePointer(void* volatile* pDst, void* pExchange, void* pComperand);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// DrxInterlocked*SList Function, these are specialized C-A-S
// functions for single-linked lists which prevent the A-B-A problem there
// there are implemented in the platform specific DrxThread_*.h files
//NOTE: The sizes are verified at compile-time in the implementation functions, but this is still ugly

#if DRX_PLATFORM_64BIT
	#define LOCK_FREE_LINKED_LIST_DOUBLE_SIZE_PTR_ALIGN 16
#elif DRX_PLATFORM_32BIT
	#define LOCK_FREE_LINKED_LIST_DOUBLE_SIZE_PTR_ALIGN 8
#else
	#error "Unsupported plaform"
#endif

struct SLockFreeSingleLinkedListEntry
{
	DRX_ALIGN(LOCK_FREE_LINKED_LIST_DOUBLE_SIZE_PTR_ALIGN) SLockFreeSingleLinkedListEntry * volatile pNext;
};
static_assert(std::alignment_of<SLockFreeSingleLinkedListEntry>::value == sizeof(uintptr_t) * 2, "Alignment failure for SLockFreeSingleLinkedListEntry");

struct SLockFreeSingleLinkedListHeader
{
	//! Initializes the single-linked list.
	friend void DrxInitializeSListHead(SLockFreeSingleLinkedListHeader& list);

	//! Push one element atomically onto the front of a single-linked list.
	friend void DrxInterlockedPushEntrySList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& element);

	//! Push a list of elements atomically onto the front of a single-linked list.
	//! \note The entries must already be linked (ie, last must be reachable by moving through pNext starting at first).
	friend void DrxInterlockedPushListSList(SLockFreeSingleLinkedListHeader& list, SLockFreeSingleLinkedListEntry& first, SLockFreeSingleLinkedListEntry& last, uint32 count);

	//! Retrieves a pointer to the first item on a single-linked list.
	//! \note This does not remove the item from the list, and it's unsafe to inspect anything but the returned address.
	friend void* DrxRtlFirstEntrySList(SLockFreeSingleLinkedListHeader& list);

	//! Pops one element atomically from the front of a single-linked list, and returns a pointer to the item.
	//! \note If the list was empty, nullptr is returned instead.
	friend void* DrxInterlockedPopEntrySList(SLockFreeSingleLinkedListHeader& list);

	//! Flushes the entire single-linked list, and returns a pointer to the first item that was on the list.
	//! \note If the list was empty, nullptr is returned instead.
	friend void* DrxInterlockedFlushSList(SLockFreeSingleLinkedListHeader& list);

private:
	DRX_ALIGN(LOCK_FREE_LINKED_LIST_DOUBLE_SIZE_PTR_ALIGN) SLockFreeSingleLinkedListEntry * volatile pNext;

#if DRX_PLATFORM_ORBIS
	// Only need "salt" on platforms using CAS (ORBIS uses embedded salt)
#elif DRX_PLATFORM_POSIX
	// If pointers 32bit, salt should be as well. Otherwise we get 4 bytes of padding between pNext and salt and CAS operations fail
	#if DRX_PLATFORM_64BIT
	volatile uint64 salt;
	#else
	volatile uint32 salt;
	#endif
#endif
};
static_assert(std::alignment_of<SLockFreeSingleLinkedListHeader>::value == sizeof(uintptr_t) * 2, "Alignment failure for SLockFreeSingleLinkedListHeader");
#undef LOCK_FREE_LINKED_LIST_DOUBLE_SIZE_PTR_ALIGN

#if DRX_PLATFORM_WINAPI
	#include "DrxAtomics_win32.h"
#elif DRX_PLATFORM_ORBIS
	#include "DrxAtomics_sce.h"
#elif DRX_PLATFORM_POSIX
	#include "DrxAtomics_posix.h"
#endif

#define WRITE_LOCK_VAL (1 << 16)

void*      DrxCreateCriticalSection();
#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE
void       DrxCreateCriticalSectionInplace(void*);
#endif
void       DrxDeleteCriticalSection(void* cs);
#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE
void       DrxDeleteCriticalSectionInplace(void* cs);
#endif
void       DrxEnterCriticalSection(void* cs);
bool       DrxTryCriticalSection(void* cs);
void       DrxLeaveCriticalSection(void* cs);

ILINE void DrxSpinLock(volatile int* pLock, int checkVal, int setVal)
{
	static_assert(sizeof(int) == sizeof(LONG), "Unsecured cast. Int is not same size as LONG.");
	CSimpleThreadBackOff threadBackoff;
	while (DrxInterlockedCompareExchange((volatile LONG*)pLock, setVal, checkVal) != checkVal)
	{
		threadBackoff.backoff();
	}
}

ILINE void DrxReleaseSpinLock(volatile int* pLock, int setVal)
{
	*pLock = setVal;
}

ILINE void DrxReadLock(volatile int* rw)
{
	DrxInterlockedAdd(rw, 1);
#ifdef NEED_ENDIAN_SWAP
	volatile char* pw = (volatile char*)rw + 1;
#else
	volatile char* pw = (volatile char*)rw + 2;
#endif

	CSimpleThreadBackOff backoff;
	for (; *pw; )
	{
		backoff.backoff();
	}
}

ILINE void DrxReleaseReadLock(volatile int* rw)
{
	DrxInterlockedAdd(rw, -1);
}

ILINE void DrxWriteLock(volatile int* rw)
{
	DrxSpinLock(rw, 0, WRITE_LOCK_VAL);
}

ILINE void DrxReleaseWriteLock(volatile int* rw)
{
	DrxInterlockedAdd(rw, -WRITE_LOCK_VAL);
}

//////////////////////////////////////////////////////////////////////////
struct ReadLock
{
	ReadLock(volatile int& rw) : prw(&rw)
	{
		DrxReadLock(prw);
	}

	~ReadLock()
	{
		DrxReleaseReadLock(prw);
	}

private:
	volatile int* const prw;
};

struct ReadLockCond
{
	ReadLockCond(volatile int& rw, int bActive) : iActive(0),prw(&rw)
	{
		if (bActive)
		{
			iActive = 1;
			DrxReadLock(prw);
		}
	}

	void SetActive(int bActive = 1)
	{
		iActive = bActive;
	}

	void Release()
	{
		DrxInterlockedAdd(prw, -iActive);
	}

	~ReadLockCond()
	{
		DrxInterlockedAdd(prw, -iActive);
	}

private:
	int                 iActive;
	volatile int* const prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteLock
{
	WriteLock(volatile int& rw) : prw(&rw)
	{
		DrxWriteLock(&rw);
	}

	~WriteLock()
	{
		DrxReleaseWriteLock(prw);
	}

private:
	volatile int* const prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteAfterReadLock
{
	WriteAfterReadLock(volatile int& rw) : prw(&rw)
	{
		DrxSpinLock(&rw, 1, WRITE_LOCK_VAL + 1);
	}

	~WriteAfterReadLock()
	{
		DrxInterlockedAdd(prw, -WRITE_LOCK_VAL);
	}

private:
	volatile int* const prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteLockCond
{
	WriteLockCond(volatile int& rw, int bActive = 1) : prw(&rw), iActive(0)
	{
		if (bActive)
		{
			iActive = WRITE_LOCK_VAL;
			DrxSpinLock(prw, 0, iActive);
		}
	}

	WriteLockCond() : iActive(0), prw(&iActive) {}

	~WriteLockCond()
	{
		DrxInterlockedAdd(prw, -iActive);
	}

	void SetActive(int bActive = 1)
	{
		iActive = -bActive & WRITE_LOCK_VAL;
	}

	void Release()
	{
		DrxInterlockedAdd(prw, -iActive);
	}

	int           iActive; //!< Not private because used directly in Physics RWI.
	volatile int* prw;     //!< Not private because used directly in Physics RWI.
};

//////////////////////////////////////////////////////////////////////////
#if defined(EXCLUDE_PHYSICS_THREAD)
ILINE void SpinLock(volatile int* pLock, int checkVal, int setVal)
{
	*(int*)pLock = setVal;
}
ILINE void AtomicAdd(volatile int* pVal, int iAdd)                    { *(int*)pVal += iAdd; }
ILINE void AtomicAdd(volatile unsigned int* pVal, int iAdd)           { *(unsigned int*)pVal += iAdd; }

ILINE void JobSpinLock(volatile int* pLock, int checkVal, int setVal) { DrxSpinLock(pLock, checkVal, setVal); }
#else
ILINE void SpinLock(volatile int* pLock, int checkVal, int setVal)
{
	DrxSpinLock(pLock, checkVal, setVal);
}
ILINE void AtomicAdd(volatile int* pVal, int iAdd)                    { DrxInterlockedAdd(pVal, iAdd); }
ILINE void AtomicAdd(volatile unsigned int* pVal, int iAdd)           { DrxInterlockedAdd((volatile int*)pVal, iAdd); }

ILINE void JobSpinLock(volatile int* pLock, int checkVal, int setVal) { SpinLock(pLock, checkVal, setVal); }
#endif

ILINE void JobAtomicAdd(volatile int* pVal, int iAdd)
{
	DrxInterlockedAdd(pVal, iAdd);
}
ILINE void JobAtomicAdd(volatile unsigned int* pVal, int iAdd) { DrxInterlockedAdd((volatile int*)pVal, iAdd); }
