// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SERIALIZE_BITS__H__
#define __SERIALIZE_BITS__H__

#if _MSC_VER > 1000
	#pragma once
#endif
#include <DinrusX/DinrusXAct/StdAfx.h>

class CBitArray
{
public:
	/* This is a naive implementation, but actually packs bits without padding, unlike DinrusXNetwork */
	CBitArray(TSerialize* m_ser);

	void          ResetForWrite();
	void          ResetForRead();

	bool          IsReading() { return m_isReading; }
	bool          IsWriting() { return m_isReading == false; }

	int           NumberOfBitsPushed();
	void          PushBit(int bit);
	int           PopBit();
	void          ReadBits(unsigned char* out, int numBits);
	void          WriteBits(const unsigned char* in, int numBits);
	inline uint32 bitsneeded(uint32 v);

	inline void   SerializeInt(int* v, int min, int max);
	inline void   SerializeUInt(unsigned int* v, unsigned int min, unsigned int max);
	void          SerializeFloat(float* data, float min, float max, int totalNumBits, int reduceRange = 0);
	inline void   SerializeEntity(EntityId& id);

	inline void   Serialize(bool& v);
	inline void   Serialize(uint32& v, uint32 min = 0, uint32 max = 0xffffffff);
	inline void   Serialize(int32& v, int min, int max);
	inline void   Serialize(int16& v, int min, int max);
	inline void   Serialize(uint16& v, uint16 min = 0, uint16 max = 0xffff);
	inline void   Serialize(unsigned char& v, int min, int max);
	inline void   Serialize(float& f, float min, float max, int totalNumBits, int reduceRange = 0);
	inline void   Serialize(Vec3& v, float min, float max, int numBitsPerElem, int reduceRange = 0);
	inline void   Serialize(Quat& v);

	void          WriteToSerializer();

private:
	template<class INT> void SerializeInt_T(INT* v, INT min, INT max);

public:
	enum { maxBytes = 1 << 13 };
	int           m_bytePos;
	int           m_bitPos;
	int           m_numberBytes;
	bool          m_isReading;
	int           m_multiplier;
	TSerialize*   m_ser;
	unsigned char m_readByte;
	unsigned char m_data[maxBytes];
};

/*
   =========================================================================================================
   Implementation
   =========================================================================================================
 */

inline uint32 CBitArray::bitsneeded(uint32 v)
{
	// See bit twiddling hacks
	static const int MultiplyDeBruijnBitPosition[32] =
	{
		0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
		8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31
	};

	v |= v >> 1; // first round down to one less than a power of 2
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return 1 + MultiplyDeBruijnBitPosition[(uint32)(v * 0x07C4ACDDU) >> 27];
}

template<class INT>
NO_INLINE_WEAK void CBitArray::SerializeInt_T(INT* v, INT min, INT max)
{
	INT range = max - min;
	INT nbits = bitsneeded(range);
	unsigned char c;

	if (IsReading())
	{
		INT multiplier = 1;
		*v = 0;
		while (nbits > 8)
		{
			ReadBits(&c, 8);
			*v |= multiplier * c;   // Note: there is no need for endian swapping with this method
			multiplier = multiplier * 256;
			nbits = nbits - 8;
		}
		ReadBits(&c, nbits);
		*v |= multiplier * c;
		*v = *v + min;
	}
	else
	{
		INT tmp = std::min(*v - min, (INT)0);
		if (tmp > range) tmp = range;
		while (nbits > 8)
		{
			c = tmp & 0xff;
			WriteBits(&c, 8);   // Note: there is no need for endian swapping with this method
			tmp = tmp >> 8;
			nbits = nbits - 8;
		}
		c = tmp & 0xff;
		WriteBits(&c, nbits);
	}
}

inline void CBitArray::Serialize(bool& v)
{
	int tmp = v ? 1 : 0;
	SerializeInt_T<int>(&tmp, 0, 1);
	v = (tmp != 0);
}

inline void CBitArray::SerializeInt(int* v, int min, int max)
{
	SerializeInt_T<int>(v, min, max);
}

inline void CBitArray::SerializeUInt(unsigned int* v, unsigned int min, unsigned int max)
{
	SerializeInt_T<unsigned int>(v, min, max);
}

inline void CBitArray::Serialize(uint32& v, uint32 min, uint32 max)
{
	unsigned int tmp = v;
	SerializeUInt(&tmp, min, max);
	v = (uint32)tmp;
}

inline void CBitArray::Serialize(int32& v, int min, int max)
{
	int tmp = v;
	SerializeInt(&tmp, min, max);
	v = (int32)tmp;
}

inline void CBitArray::Serialize(int16& v, int min, int max)
{
	int tmp = v;
	SerializeInt(&tmp, min, max);
	v = (int16)tmp;
}

inline void CBitArray::Serialize(uint16& v, uint16 min, uint16 max)
{
	unsigned int tmp = v;
	SerializeUInt(&tmp, min, max);
	v = (uint16)tmp;
}

inline void CBitArray::Serialize(unsigned char& v, int min, int max)
{
	int tmp = v;
	SerializeInt(&tmp, min, max);
	v = (unsigned char)tmp;
}

inline void CBitArray::Serialize(float& v, float min, float max, int totalNumBits, int reduceRange)
{
	SerializeFloat(&v, min, max, totalNumBits, reduceRange);
}

inline void CBitArray::Serialize(Vec3& v, float min, float max, int numBitsPerElem, int reduceRange)
{
	SerializeFloat(&v.x, min, max, numBitsPerElem, reduceRange);
	SerializeFloat(&v.y, min, max, numBitsPerElem, reduceRange);
	SerializeFloat(&v.z, min, max, numBitsPerElem, reduceRange);
}

inline void CBitArray::Serialize(Quat& q)
{
	// Should this compression migratate to Drx_Quat.h ?
	float quat[4];
	if (IsWriting())
	{
		uint32 out = 0;
		uint32 i;
		float scale = 1.0f;

		quat[0] = q.w;
		quat[1] = q.v.x;
		quat[2] = q.v.y;
		quat[3] = q.v.z;

		uint32 largest = 0;
		for (i = 1; i < 4; i++)
		{
			if (fabsf(quat[i]) > fabsf(quat[largest]))
				largest = i;
		}

		// Scale the quat so that reconstruction always deals with positive value
		scale = (float)__fsel(quat[largest], 1.f, -1.f);

		out |= largest; // first 2 bits denote which is the largest

		uint32 entry = 0;
		uint32 multiply = 4;
		for (i = 0; i < 4; i++)
		{
			if (i != largest)
			{
				// Encode each remaining value in 10 bits, using range 0-1022. NB, range is chosen so zero is reproduced correctly
				int val = (int)((((scale * quat[i]) + 0.7071f) * (1022.f / 1.4142f)) + 0.5f);
				if (val < 0) val = 0;
				if (val > 1022) val = 1022;
				out |= val * multiply;
				multiply *= 1024;
				entry++;
			}
		}
		Serialize(out);
	}
	else // Reading
	{
		uint32 in;
		Serialize(in);

		static int idx[4][3] = {
			{ 1, 2, 3 }, { 0, 2, 3 }, { 0, 1, 3 }, { 0, 1, 2 }
		};
		int mv = in & 3;
		int* indices = idx[mv];
		uint32 c0 = (in >> 2) & 1023;
		uint32 c1 = (in >> 12) & 1023;
		uint32 c2 = (in >> 22) & 1023;
		float outDatai0 = (c0 * (1.4142f / 1022.f)) - 0.7071f;
		float outDatai1 = (c1 * (1.4142f / 1022.f)) - 0.7071f;
		float outDatai2 = (c2 * (1.4142f / 1022.f)) - 0.7071f;
		float sumOfSqs = 1.f - outDatai0 * outDatai0;
		quat[indices[0]] = outDatai0;
		sumOfSqs -= outDatai1 * outDatai1;
		quat[indices[1]] = outDatai1;
		sumOfSqs -= outDatai2 * outDatai2;
		quat[indices[2]] = outDatai2;
		sumOfSqs = (float)__fsel(sumOfSqs, sumOfSqs, 0.0f);
		quat[mv] = sqrtf(sumOfSqs);

		q.w = quat[0];
		q.v.x = quat[1];
		q.v.y = quat[2];
		q.v.z = quat[3];
	}
}

inline void CBitArray::SerializeFloat(float* data, float min, float max, int totalNumBits, int reduceRange)
{
	int range = (1 << totalNumBits) - 1 - reduceRange;

	if (IsReading())
	{
		int n;
		SerializeInt(&n, 0, range);
		*data = ((float)n) * (max - min) / (float)(range) + min;
	}
	else
	{
		float f = clamp_tpl(*data, min, max);
		int n = (int)(((float)range) / (max - min) * (f - min));
		SerializeInt(&n, 0, range);
	}
}

inline void CBitArray::SerializeEntity(EntityId& entId)
{
	INetContext* pContext = gEnv->pGameFramework->GetNetContext();
	if (IsReading())
	{
		SNetObjectID netId;
		Serialize(netId.id);
		if (pContext)
		{
			pContext->Resaltify(netId);
			entId = pContext->GetEntityID(netId);
		}
		else
		{
			entId = 0;
			GameWarning("brk: can't decode entid, pContext==NULL!");
		}
	}
	else
	{
		SNetObjectID netId;
		if (pContext)
		{
			netId = pContext->GetNetID(entId, false);
		}
		else
		{
			GameWarning("brk: can't send entid, pContext==NULL!");
		}
		Serialize(netId.id);
	}
}

#endif
