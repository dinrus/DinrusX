// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name: ChunkFileReaders.h
//  Created:   2013/11/18 by Sergey Sokov
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ChunkFileReaders_h__
#define __ChunkFileReaders_h__

#include <DinrusX/DinrusX3dEng/ChunkFileComponents.h>
#include <DinrusX/DinrusX3dEng/IChunkFile.h>

namespace ChunkFile
{

struct IReader
{
	virtual ~IReader()
	{
	}

	virtual void  Close() = 0;
	virtual int32 GetSize() = 0;
	virtual bool  SetPos(int32 pos) = 0;
	virtual bool  Read(void* buffer, size_t size) = 0;
};

class DrxFileReader
	: public IReader
{
public:
	DrxFileReader();
	virtual ~DrxFileReader();

	bool Open(const char* filename);

	//-------------------------------------------------------
	// IReader interface
	virtual void  Close();
	virtual int32 GetSize();
	virtual bool  SetPos(int32 pos);
	virtual bool  Read(void* buffer, size_t size);
	//-------------------------------------------------------

private:
	CDrxFile m_f;
	int32    m_offset;
};

class MemoryReader
	: public IReader
{
public:
	MemoryReader();
	virtual ~MemoryReader();

	bool Start(void* ptr, int32 size);

	//-------------------------------------------------------
	// IReader interface
	virtual void  Close();
	virtual int32 GetSize();
	virtual bool  SetPos(int32 pos);
	virtual bool  Read(void* buffer, size_t size);
	//-------------------------------------------------------

private:
	char* m_ptr;
	int32 m_size;
	int32 m_offset;
};

const char* GetChunkTableEntries_0x744_0x745(IReader* pReader, std::vector<IChunkFile::ChunkDesc>& chunks);
const char* GetChunkTableEntries_0x744_0x745(IReader* pReader, std::vector<IChunkFile::ChunkDesc*>& chunks);

const char* GetChunkTableEntries_0x746(IReader* pReader, std::vector<IChunkFile::ChunkDesc>& chunks);
const char* GetChunkTableEntries_0x746(IReader* pReader, std::vector<IChunkFile::ChunkDesc*>& chunks);

const char* StripChunkHeaders_0x744_0x745(IReader* pReader, std::vector<IChunkFile::ChunkDesc>& chunks);
const char* StripChunkHeaders_0x744_0x745(IReader* pReader, std::vector<IChunkFile::ChunkDesc*>& chunks);

}  // namespace ChunkFile

#endif
