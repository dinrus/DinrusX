// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:  splits a compression policy into two pieces, one for the
               witness, and another for all other clients
   -------------------------------------------------------------------------
   History:
   - 02/11/2006   12:34 : Created by Craig Tiller
*************************************************************************/

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/OwnChannelCompressionPolicy.h>

#if USE_ARITHSTREAM
	#define SERIALIZATION_TYPE(T)                                                                                                                                                                    \
	  bool COwnChannelCompressionPolicy::ReadValue(CCommInputStream & in, T & value, CArithModel * pModel, uint32 age, bool own, CByteInputStream * pCurState, CByteOutputStream * pNewState) const  \
	  {                                                                                                                                                                                              \
	    return Get(own)->ReadValue(in, value, pModel, age, own, pCurState, pNewState);                                                                                                               \
	  }                                                                                                                                                                                              \
	  bool COwnChannelCompressionPolicy::WriteValue(CCommOutputStream & out, T value, CArithModel * pModel, uint32 age, bool own, CByteInputStream * pCurState, CByteOutputStream * pNewState) const \
	  {                                                                                                                                                                                              \
	    return Get(own)->WriteValue(out, value, pModel, age, own, pCurState, pNewState);                                                                                                             \
	  }
	#include <DinrusX/DinrusXNetwork/SerializationTypes.h>
	#undef SERIALIZATION_TYPE

bool COwnChannelCompressionPolicy::ReadValue(CCommInputStream& in, SSerializeString& value, CArithModel* pModel, uint32 age, bool own, CByteInputStream* pCurState, CByteOutputStream* pNewState) const
{
	return Get(own)->ReadValue(in, value, pModel, age, own, pCurState, pNewState);
}

bool COwnChannelCompressionPolicy::WriteValue(CCommOutputStream& out, const SSerializeString& value, CArithModel* pModel, uint32 age, bool own, CByteInputStream* pCurState, CByteOutputStream* pNewState) const
{
	return Get(own)->WriteValue(out, value, pModel, age, own, pCurState, pNewState);
}
#else
	#define SERIALIZATION_TYPE(T)                                                                                                                                                    \
	  bool COwnChannelCompressionPolicy::ReadValue(CNetInputSerializeImpl * in, T & value, uint32 age, bool own, CByteInputStream * pCurState, CByteOutputStream * pNewState) const  \
	  {                                                                                                                                                                              \
	    return Get(own)->ReadValue(in, value, age, own, pCurState, pNewState);                                                                                                       \
	  }                                                                                                                                                                              \
	  bool COwnChannelCompressionPolicy::WriteValue(CNetOutputSerializeImpl * out, T value, uint32 age, bool own, CByteInputStream * pCurState, CByteOutputStream * pNewState) const \
	  {                                                                                                                                                                              \
	    return Get(own)->WriteValue(out, value, age, own, pCurState, pNewState);                                                                                                     \
	  }
	#include <DinrusX/DinrusXNetwork/SerializationTypes.h>
	#undef SERIALIZATION_TYPE

bool COwnChannelCompressionPolicy::ReadValue(CNetInputSerializeImpl* in, SSerializeString& value, uint32 age, bool own, CByteInputStream* pCurState, CByteOutputStream* pNewState) const
{
	return Get(own)->ReadValue(in, value, age, own, pCurState, pNewState);
}

bool COwnChannelCompressionPolicy::WriteValue(CNetOutputSerializeImpl* out, const SSerializeString& value, uint32 age, bool own, CByteInputStream* pCurState, CByteOutputStream* pNewState) const
{
	return Get(own)->WriteValue(out, value, age, own, pCurState, pNewState);
}
#endif

#if NET_PROFILE_ENABLE
	#define SERIALIZATION_TYPE(T)                            \
	  int COwnChannelCompressionPolicy::GetBitCount(T value) \
	  {                                                      \
	    const int own = m_pOwn->GetBitCount(value);          \
	    const int other = m_pOther->GetBitCount(value);      \
	    return own > other ? own : other;                    \
	  }
	#include <DinrusX/DinrusXNetwork/SerializationTypes.h>
	#undef SERIALIZATION_TYPE

int COwnChannelCompressionPolicy::GetBitCount(SSerializeString& value)
{
	assert(0);
	return 0;
}
#endif
