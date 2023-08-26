// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <intsafe.h>
#include <intrin.h>
#include <DinrusX/CoreX/Assert/DrxAssert.h>

#if DRX_PLATFORM_64BIT
__MINGW_EXTENSION __int64 _InterlockedCompare64Exchange128(__int64 volatile *Destination,__int64 ExchangeHigh,__int64 ExchangeLow,__int64 Comparand);
#endif

//////////////////////////////////////////////////////////////////////////
// Interlocked API
//////////////////////////////////////////////////////////////////////////

// Returns the resulting incremented value
ILINE LONG DrxInterlockedIncrement(volatile int* pDst)
{
	static_assert(sizeof(int) == sizeof(LONG), "Unsecured cast. int is not same size as LONG.");
	return _InterlockedIncrement((volatile LONG*)pDst);
}

// Returns the resulting decremented value
ILINE LONG DrxInterlockedDecrement(volatile int* pDst)
{
	static_assert(sizeof(int) == sizeof(LONG), "Unsecured cast. int is not same size as LONG.");
	return _InterlockedDecrement((volatile LONG*)pDst);
}

// Returns the resulting added value
ILINE LONG DrxInterlockedAdd(volatile int* pDst, int add)
{
	return DrxInterlockedAdd((volatile LONG*)pDst, (LONG)add);
}

// Returns the resulting added value
ILINE LONG DrxInterlockedAdd(volatile LONG* pDst, LONG add)
{
#if DRX_PLATFORM_X64
	return _InterlockedExchangeAdd(pDst, add) + add;
#else
	return DrxInterlockedExchangeAdd(pDst, add) + add;
#endif
}

// Returns the resulting added value
ILINE int64 DrxInterlockedAdd(volatile int64* pDst, int64 add)
{
#if DRX_PLATFORM_X64
	static_assert(sizeof(int64) == sizeof(LONGLONG), "Unsecured cast. int64 is not same size as LONGLONG.");
	return (int64)_InterlockedExchangeAdd64((volatile LONGLONG*)pDst, add) + add;
#else
	int64 val;
	int64 sum;
	do {
		val = *pDst;
		sum = val + add;
	}
	while (DrxInterlockedCompareExchange64(pDst, sum, val) != val);
	return sum;
#endif
}

// Returns the resulting added value
ILINE size_t DrxInterlockedAdd(volatile size_t* pDst, size_t add)
{
#if DRX_PLATFORM_X64
	static_assert(sizeof(size_t) == sizeof(LONG64), "Unsecured cast. size_t is not same size as LONG64.");
	return (size_t)_InterlockedExchangeAdd64((volatile LONG64*)pDst, add) + add;
#else
	static_assert(sizeof(size_t) == sizeof(LONG), "Unsecured cast. size_t is not same size as LONG.");
	return (size_t)DrxInterlockedAdd((volatile LONG*)pDst, (LONG)add);
#endif
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchange(volatile int* pDst, int exchange)
{
	return DrxInterlockedExchange((volatile LONG*)pDst, (LONG)exchange);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchange(volatile LONG* pDst, LONG exchange)
{
	return _InterlockedExchange(pDst, exchange);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeAdd(volatile LONG* pDst, LONG value)
{
	return _InterlockedExchangeAdd(pDst, value);
}

// Returns initial value prior exchange
ILINE size_t DrxInterlockedExchangeAdd(volatile size_t* pDst, size_t add)
{
#if DRX_PLATFORM_X64
	static_assert(sizeof(size_t) == sizeof(LONGLONG), "Unsecured cast. size_t is not same size as LONGLONG(int64).");
	return _InterlockedExchangeAdd64((volatile LONGLONG*)pDst, add); // intrinsic returns previous value
#else
	static_assert(sizeof(size_t) == sizeof(LONG), "Unsecured cast. size_t is not same size as LONG.");
	return (size_t)DrxInterlockedExchangeAdd((volatile LONG*)pDst, (LONG)add); // intrinsic returns previous value
#endif
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeAnd(volatile LONG* pDst, LONG value)
{
	return _InterlockedAnd(pDst, value);
}

// Returns initial value prior exchange
ILINE LONG DrxInterlockedExchangeOr(volatile LONG* pDst, LONG value)
{
	return _InterlockedOr(pDst, value);
}

// Returns initial address prior exchange
ILINE void* DrxInterlockedExchangePointer(void* volatile* pDst, void* pExchange)
{
#if DRX_PLATFORM_X64 || _MSC_VER > 1700
	return _InterlockedExchangePointer(pDst, pExchange);
#else
	static_assert(sizeof(void*) == sizeof(LONG), "Unsecured cast. void* is not same size as LONG.");
	return (void*)_InterlockedExchange((LONG volatile*)pDst, (LONG)pExchange);
#endif
}

// Returns initial address prior exchange
ILINE LONG DrxInterlockedCompareExchange(LONG volatile* pDst, LONG exchange, LONG comperand)
{
	return _InterlockedCompareExchange(pDst, exchange, comperand);
}

// Returns initial address prior exchange
ILINE void* DrxInterlockedCompareExchangePointer(void* volatile* pDst, void* pExchange, void* pComperand)
{
#if DRX_PLATFORM_X64 || _MSC_VER > 1700
	return _InterlockedCompareExchangePointer(pDst, pExchange, pComperand);
#else
	static_assert(sizeof(void*) == sizeof(LONG), "Unsecured cast. void* is not same size as LONG.");
	return (void*)_InterlockedCompareExchange((LONG volatile*)pDst, (LONG)pExchange, (LONG)pComperand);
#endif
}

// Returns initial value prior exchange
ILINE int64 DrxInterlockedCompareExchange64(volatile int64* pDst, int64 exchange, int64 compare)
{
	static_assert(sizeof(int64) == sizeof(__int64), "Unsecured cast. int64 is not same size as __int64.");
	return _InterlockedCompareExchange64(pDst, exchange, compare);
}


#if DRX_PLATFORM_64BIT
// Returns initial value prior exchange
ILINE unsigned char DrxInterlockedCompareExchange128(volatile int64* pDst, int64 exchangeHigh, int64 exchangeLow, int64* pComparandResult)
{
	DRX_ASSERT_MESSAGE((((int64)pDst) & 15) == 0, "The destination data must be 16-byte aligned to avoid a general protection fault.");
	#if DRX_PLATFORM_X64 || DRX_PLATFORM_X86
		bool bEquals;
		__asm__ __volatile__(
		"lock cmpxchg16b %1\n\t"
		"setz %0"
		: "=q" (bEquals), "+m" (*pDst), "+d" (pComparandResult[1]), "+a" (pComparandResult[0])
		: "c" (exchangeHigh), "b" (exchangeLow)
		: "cc");
		return (char)bEquals;
	#else
	static_assert(sizeof(int64) == sizeof(__int64), "Unsecured cast. int64 is not same size as __int64.");
	DRX_ASSERT_MESSAGE((((int64)pDst) & 15) == 0, "The destination data must be 16-byte aligned to avoid a general protection fault.");
	return (unsigned char)_InterlockedCompare64Exchange128( pDst, (__int64)exchangeHigh, (__int64)exchangeLow, (__int64)*pComparandResult);
	#endif
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
		// Note: Not using Sleep(x) and SwitchToThread()
		// Sleep(0): Give OS the CPU ... something none game related could block the core for a while (same for SwitchToThread())
		// Sleep(1): System timer resolution dependent. Usual default is 1/64sec. So the worst case is we have to wait 15.6ms.

		// Simply yield processor (good for hyper threaded systems. Allows the logical core to run)
		_mm_pause();
	}

private:
	int m_counter;
};