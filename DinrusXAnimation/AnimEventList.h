// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

 class  CAnimEventList
	: public IAnimEventList
{
public:
	CAnimEventList();
	virtual ~CAnimEventList();

	virtual uint32                GetCount() const;
	virtual const CAnimEventData& GetByIndex(uint32 animEventIndex) const;
	virtual CAnimEventData&       GetByIndex(uint32 animEventIndex);
	virtual void                  Append(const CAnimEventData& animEvent);
	virtual void                  Remove(uint32 animEventIndex);
	virtual void                  Clear();

	size_t                        GetAllocSize() const;
	void                          GetMemoryUsage(IDrxSizer* pSizer) const;

private:
	DynArray<CAnimEventData> m_animEvents;
};
