// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXNetwork/Quantizer.h>
#include <DinrusX/DinrusXNetwork/CommStream.h>

#include <DinrusX/DinrusXNetwork/PredictiveBase.h>

class CNetInputSerializeImpl;
class CNetOutputSerializeImpl;
class CPredictiveFloatTracker;

class CPredictiveFloat :
	public CPredictiveBase<int32>
{
public:
	bool Load(XmlNodeRef node, const string& filename, const string& child);

#if USE_ARITHSTREAM
	bool ReadValue(CCommInputStream& stm, float& value, uint32 mementoAge, uint32 timeFraction32 = 0) const;
	void WriteValue(CCommOutputStream& stm, float value, uint32 mementoAge, uint32 timeFraction32 = 0) const;
#else
	bool ReadValue(CNetInputSerializeImpl* stm, float& value, uint32 mementoAge, uint32 timeFraction32 = 0) const;
	void WriteValue(CNetOutputSerializeImpl* stm, float value, uint32 mementoAge, uint32 timeFraction32 = 0) const;
#endif

#if NET_PROFILE_ENABLE
	int GetBitCount();
#endif

private:
	CQuantizer m_quantizer;
};
