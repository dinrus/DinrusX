// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <deque>
#include <DinrusX/DinrusXNetwork/SessionID.h>
#include <DinrusX/DinrusXNetwork/NetResolver.h>

class CDebugOutput
{
public:
	virtual ~CDebugOutput(){}
	void Put(uint8 val);

	void Run(DRXSOCKET sock);

	template<class T>
	void Write(const T& value)
	{
		DrxAutoLock<CDebugOutput> lk(*this);
		const uint8* pBuffer = (uint8*)&value;
		for (int i = 0; i < sizeof(value); i++)
			Put(pBuffer[i]);
	}

	void Write(const string& value)
	{
		DrxAutoLock<CDebugOutput> lk(*this);
		Write((uint32)value.length());
		for (uint32 i = 0; i < value.length(); i++)
			Put(value[i]);
	}

	CSessionID m_sessionID;

	void WriteValue(const TNetAddressVec& vec)
	{
		DrxAutoLock<CDebugOutput> lk(*this);
		Write(vec.size());
		for (size_t i = 0; i < vec.size(); i++)
			Write(CNetAddressResolver().ToString(vec[i]));
	}

	virtual void Lock() = 0;
	virtual void Unlock() = 0;

private:
	static const int DATA_SIZE = 128 - sizeof(int);
	struct SBuf
	{
		SBuf() : ready(true), sz(0), pos(0) { ZeroMemory(data, sizeof(data)); }
		bool ready;
		int  sz;
		int  pos;
		char data[DATA_SIZE];
	};

	std::deque<SBuf> m_buffers;
};
