// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
*************************************************************************/

#pragma once
#ifndef XMLCPB_STRING_TABLE_READER_H
	#define XMLCPB_STRING_TABLE_READER_H

	#include <DinrusX/DinrusXAct/XMLCPB_Common.h>
	#include <DinrusX/DinrusXAct/XMLCPB_BufferReader.h>
	#include <DinrusX/CoreX/Platform/IPlatformOS.h>

namespace XMLCPB {

class CReader;

class CStringTableReader
{
public:
	explicit CStringTableReader(IGeneralMemoryHeap* pHeap);

	const char* GetString(StringID stringId) const;
	int         GetNumStrings() const { return m_stringAddrs.size(); }

	void        ReadFromFile(CReader& Reader, IPlatformOS::ISaveReaderPtr pOSSaveReader, const SFileHeader::SStringTable& headerInfo);
	void        ReadFromMemory(CReader& Reader, const uint8* pData, uint32 dataSize, const SFileHeader::SStringTable& headerInfo, uint32& outReadLoc);

	// for debug and testing
	#ifndef _RELEASE
	void WriteStringsIntoTextFile(const char* pFileName);
	#endif

private:
	typedef DynArray<FlatAddr, int, NArray::SmallDynStorage<NAlloc::GeneralHeapAlloc>> FlatAddrVec;

private:
	SBufferReader m_buffer;
	FlatAddrVec   m_stringAddrs;
};

}  // end namespace

#endif
