// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __ADAPTIVEVELOCITY_H__
#define __ADAPTIVEVELOCITY_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Quantizer.h>
#include <DinrusX/DinrusXNetwork/BoolCompress.h>

class CAdaptiveVelocity
{
public:
	bool Load(XmlNodeRef node, const string& filename, const string& child);
#if USE_MEMENTO_PREDICTORS
	void ReadMemento(CByteInputStream& stm) const;
	void WriteMemento(CByteOutputStream& stm) const;
	void NoMemento() const;
#endif
#if USE_ARITHSTREAM
	bool ReadValue(CCommInputStream& stm, float& value) const;
	void WriteValue(CCommOutputStream& stm, float value) const;
#else
	bool ReadValue(CNetInputSerializeImpl* stm, float& value) const;
	void WriteValue(CNetOutputSerializeImpl* stm, float value) const;
#endif
#if NET_PROFILE_ENABLE
	int GetBitCount();
#endif

private:
	CQuantizer     m_quantizer;
#if USE_MEMENTO_PREDICTORS
	uint32         m_nHeight;
	CBoolCompress  m_boolCompress;

	mutable uint32 m_nLastQuantized;
#endif
};

#endif
