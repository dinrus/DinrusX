// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef GENERALMEMORYHEAP_H
#define GENERALMEMORYHEAP_H

#include <DinrusX/CoreX/Memory/IMemory.h>
#include <DinrusX/CoreX/Platform/DrxDLMalloc.h>

class CPageMappingHeap;

class CGeneralMemoryHeap : public IGeneralMemoryHeap
{
public:
	CGeneralMemoryHeap();

	// Create a heap that will map/unmap pages in the range [baseAddress, baseAddress + upperLimit).
	CGeneralMemoryHeap(UINT_PTR baseAddress, size_t upperLimit, size_t reserveSize, const char* sUsage);

	// Create a heap that will assumes all memory in the range [base, base + size) is already mapped.
	CGeneralMemoryHeap(void* base, size_t size, const char* sUsage);

	~CGeneralMemoryHeap();

public: // IGeneralMemoryHeap Members
	bool   Cleanup();

	int    AddRef();
	int    Release();

	bool   IsInAddressRange(void* ptr) const;

	void*  Calloc(size_t nmemb, size_t size, const char* sUsage = NULL);
	void*  Malloc(size_t sz, const char* sUsage = NULL);
	size_t Free(void* ptr);
	void*  Realloc(void* ptr, size_t sz, const char* sUsage = NULL);
	void*  ReallocAlign(void* ptr, size_t size, size_t alignment, const char* sUsage = NULL);
	void*  Memalign(size_t boundary, size_t size, const char* sUsage = NULL);
	size_t UsableSize(void* ptr) const;

private:
	static void* DLMMap(void* self, size_t sz);
	static int   DLMUnMap(void* self, void* mem, size_t sz);

private:
	CGeneralMemoryHeap(const CGeneralMemoryHeap&);
	CGeneralMemoryHeap& operator=(const CGeneralMemoryHeap&);

private:
	volatile int                   m_nRefCount;

	bool                           m_isResizable;

	DrxCriticalSectionNonRecursive m_mspaceLock;
	dlmspace                       m_mspace;

	CPageMappingHeap*              m_pHeap;
	void*                          m_pBlock;
	size_t                         m_blockSize;

	int                            m_numAllocs;

#ifdef DRX_TRACE_HEAP
	IMemoryUpr::HeapHandle m_nTraceHeapHandle;
#endif
};

#endif
