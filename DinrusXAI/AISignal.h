// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _AISIGNAL_H_
#define _AISIGNAL_H_

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXAI/IAISystem.h>
#include <DinrusX/DinrusXNetwork/ISerialize.h>
#include <DinrusX/CoreX/Memory/PoolAllocator.h>

struct AISignalExtraData : public IAISignalExtraData
{
public:
	static void CleanupPool();

public:
	AISignalExtraData();
	AISignalExtraData(const AISignalExtraData& other) : IAISignalExtraData(other), sObjectName() { SetObjectName(other.sObjectName); }
	virtual ~AISignalExtraData();

	AISignalExtraData& operator=(const AISignalExtraData& other);
	virtual void       Serialize(TSerialize ser);

	inline void*       operator new(size_t size)
	{
		return m_signalExtraDataAlloc.Allocate();
	}

	inline void operator delete(void* p)
	{
		return m_signalExtraDataAlloc.Deallocate(p);
	}

	virtual const char* GetObjectName() const { return sObjectName ? sObjectName : ""; }
	virtual void        SetObjectName(const char* objectName);

	// To/from script table
	virtual void ToScriptTable(SmartScriptTable& table) const;
	virtual void FromScriptTable(const SmartScriptTable& table);

private:
	char* sObjectName;

	typedef stl::PoolAllocator<sizeof(IAISignalExtraData) + sizeof(void*),
	                           stl::PoolAllocatorSynchronizationSinglethreaded> SignalExtraDataAlloc;
	static SignalExtraDataAlloc m_signalExtraDataAlloc;
};

#endif
