// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Memory/IMemory.h>

class CCustomMemoryHeap;

//////////////////////////////////////////////////////////////////////////
class CCustomMemoryHeapBlock final : public ICustomMemoryBlock
{
public:
	CCustomMemoryHeapBlock(CCustomMemoryHeap* pHeap, void* pData, size_t const allocateSize, char const* const szUsage);
	virtual ~CCustomMemoryHeapBlock();

	//////////////////////////////////////////////////////////////////////////
	// ICustomMemoryBlock implementation
	virtual void* GetData() override;
	virtual int   GetSize() override { return static_cast<int>(m_size); }
	virtual void  CopyMemoryRegion(void* pOutputBuffer, size_t offset, size_t size) override;
	//////////////////////////////////////////////////////////////////////////

private:
	CCustomMemoryHeap* m_pHeap;
	void*              m_pData;
	const size_t       m_size;
#if !defined(_RELEASE)
	const string       m_usage;
#endif
};

//////////////////////////////////////////////////////////////////////////
class CCustomMemoryHeap final : public ICustomMemoryHeap
{
public:
	explicit CCustomMemoryHeap(IMemoryUpr::EAllocPolicy const allocPolicy);

	//////////////////////////////////////////////////////////////////////////
	// ICustomMemoryHeap implementation
	virtual ICustomMemoryBlock* AllocateBlock(size_t const allocateSize, char const* const szUsage, size_t const alignment = 16) override;
	virtual void                GetMemoryUsage(IDrxSizer* pSizer) override;
	virtual size_t              GetAllocated() override;
	//////////////////////////////////////////////////////////////////////////

	void DeallocateBlock(CCustomMemoryHeapBlock* pBlock);

private:
	int m_allocatedSize;
	IMemoryUpr::EAllocPolicy const m_allocPolicy;
};
