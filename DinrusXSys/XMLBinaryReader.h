// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// Created by: Michael Smith
//---------------------------------------------------------------------------
#ifndef __XMLBINARYREADER_H__
#define __XMLBINARYREADER_H__

#include <DinrusX/DinrusXSys/XMLBinaryHeaders.h>
#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXSys/DrxFile.h>

class CBinaryXmlData;

namespace XMLBinary
{
class XMLBinaryReader
{
public:
	enum EResult
	{
		eResult_Success,
		eResult_NotBinXml,
		eResult_Error
	};

	enum EBufferMemoryHandling
	{
		eBufferMemoryHandling_MakeCopy,
		eBufferMemoryHandling_TakeOwnership
	};

public:
	XMLBinaryReader();
	~XMLBinaryReader();

	XmlNodeRef LoadFromFile(const char* filename, EResult& result);

	// Note: if bufferMemoryHandling == eBufferMemoryHandling_TakeOwnership and
	// returned result is eResult_Success, then buffer's memory is owned and
	// will be released by XMLBinaryReader (by a 'delete[] buffer' call).
	// Otherwise, the caller is responsible for releasing buffer's memory.
	XmlNodeRef  LoadFromBuffer(EBufferMemoryHandling bufferMemoryHandling, const char* buffer, size_t size, EResult& result);

	const char* GetErrorDescription() const;

private:
	void            Check(const char* buffer, size_t size, EResult& result);
	void            CheckHeader(const BinaryFileHeader& layout, size_t size, EResult& result);
	CBinaryXmlData* Create(const char* buffer, size_t size, EResult& result);
	void            SetErrorDescription(const char* text);

private:
	char m_errorDescription[64];
};
}

#endif //__XMLBINARYREADER_H__
