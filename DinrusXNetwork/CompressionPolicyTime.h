// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXNetwork/ICompressionPolicy.h>

template <class T>
class CCompressionPolicyTime :
	public CCompressionPolicy<T>
{
public:
	CCompressionPolicyTime(uint32 key) : CCompressionPolicy<T>(key)
	{

	}

	virtual void SetTimeValue(uint32 timeFraction32)
	{
		CCompressionPolicy<T>::m_impl.SetTimeValue(timeFraction32);
	}

	virtual void Init(CCompressionUpr* pUpr)
	{
		CCompressionPolicy<T>::m_impl.Init(pUpr);
	}

	virtual bool Manage(CCompressionUpr* pUpr)
	{
		return CCompressionPolicy<T>::m_impl.Manage(pUpr);
	}
};


template <class T>
struct CompressionPolicyTimeFactory :public CompressionPolicyFactoryBase
{
	CompressionPolicyTimeFactory(string name)
	{
		CCompressionRegistry::Get()->RegisterPolicy(name, Create);
	}

	static ICompressionPolicyPtr Create(uint32 key)
	{
		return new CCompressionPolicyTime<T>(key);
	}
};

#define REGISTER_COMPRESSION_POLICY_TIME(cls, name)						\
	extern void RegisterCompressionPolicy_ ## cls()					\
	{																\
		static CompressionPolicyTimeFactory<cls> cls ## _Factory(name);	\
	}																\

