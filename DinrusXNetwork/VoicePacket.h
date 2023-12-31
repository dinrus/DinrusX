// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __VOICEPACKET_H__
#define __VOICEPACKET_H__

#pragma once

#if !defined(OLD_VOICE_SYSTEM_DEPRECATED)

	#include <DinrusX/DinrusXNetwork/SerializeFwd.h>

class CVoicePacket;

typedef _smart_ptr<CVoicePacket> TVoicePacketPtr;

class CVoicePacket
{
public:
	static const int32 MAX_LENGTH = 320;

	void Serialize(TSerialize ser);

	void SetSeq(uint8 seq)
	{
		m_seq = seq;
	}
	uint8 GetSeq() const
	{
		return m_seq;
	}
	void SetLength(int32 length)
	{
		if (length > MAX_LENGTH)
		{
			NetWarning("[сеть] длина голосового пакета %d слишком большая; рассматривается как отсутствие данных.", length);
			m_length = 0;
		}
		else
			m_length = length;
	}
	int32 GetLength() const
	{
		return m_length;
	}
	uint8* GetData()
	{
		return m_data;
	}
	const uint8* GetData() const
	{
		return m_data;
	}

	static TVoicePacketPtr Allocate();
	static int32           GetCount();

	void                   AddRef()
	{
		DrxInterlockedIncrement(&m_cnt);
	}
	void Release()
	{
		if (DrxInterlockedDecrement(&m_cnt) <= 0)
			Deallocate(this);
	}

private:
	volatile int m_cnt;

	static void Deallocate(CVoicePacket* pPkt);

	CVoicePacket();
	~CVoicePacket();

	int32        m_length;
	uint8        m_seq;
	uint8        m_data[MAX_LENGTH];

	static int32 m_count;
};

#endif
#endif
