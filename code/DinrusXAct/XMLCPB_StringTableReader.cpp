// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
*************************************************************************/

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/XMLCPB_StringTableReader.h>
#include <DinrusX/DinrusXAct/XMLCPB_Reader.h>

using namespace XMLCPB;

//////////////////////////////////////////////////////////////////////////
CStringTableReader::CStringTableReader(IGeneralMemoryHeap* pHeap)
	: m_buffer(pHeap)
	, m_stringAddrs(NAlloc::GeneralHeapAlloc(pHeap))
{
}

//////////////////////////////////////////////////////////////////////////
const char* CStringTableReader::GetString(StringID stringId) const
{
	assert(stringId < m_stringAddrs.size());
	char* pString = (char*)(m_buffer.GetPointer(m_stringAddrs[stringId]));
	return pString;
}

//////////////////////////////////////////////////////////////////////////

void CStringTableReader::ReadFromFile(CReader& Reader, IPlatformOS::ISaveReaderPtr pOSSaveReader, const SFileHeader::SStringTable& headerInfo)
{
	assert(m_stringAddrs.empty());

	m_stringAddrs.resize(headerInfo.m_numStrings);
	FlatAddr* pstringAddrsTable = &(m_stringAddrs[0]);
	Reader.ReadDataFromFile(pOSSaveReader, pstringAddrsTable, sizeof(FlatAddr) * headerInfo.m_numStrings);
	for (uint32 i = 0; i < m_stringAddrs.size(); ++i)
		SwapIntegerValue(m_stringAddrs[i]);

	m_buffer.ReadFromFile(Reader, pOSSaveReader, headerInfo.m_sizeStringData);
}

//////////////////////////////////////////////////////////////////////////

void CStringTableReader::ReadFromMemory(CReader& Reader, const uint8* pData, uint32 dataSize, const SFileHeader::SStringTable& headerInfo, uint32& outReadLoc)
{
	assert(m_stringAddrs.empty());

	m_stringAddrs.resize(headerInfo.m_numStrings);
	FlatAddr* pstringAddrsTable = &(m_stringAddrs[0]);
	Reader.ReadDataFromMemory(pData, dataSize, pstringAddrsTable, sizeof(FlatAddr) * headerInfo.m_numStrings, outReadLoc);

	m_buffer.ReadFromMemory(Reader, pData, dataSize, headerInfo.m_sizeStringData, outReadLoc);
}

//////////////////////////////////////////////////////////////////////////
// debug / statistics purposes

#ifndef _RELEASE
void CStringTableReader::WriteStringsIntoTextFile(const char* pFileName)
{
	FILE* pFile = gEnv->pDrxPak->FOpen(pFileName, "wb");
	for (int i = 0; i < m_stringAddrs.size(); ++i)
	{
		string logString;
		logString.Format("%s\r\n", GetString(i));
		gEnv->pDrxPak->FWrite(logString.c_str(), logString.size(), pFile);
	}
	gEnv->pDrxPak->FClose(pFile);
}
#endif
