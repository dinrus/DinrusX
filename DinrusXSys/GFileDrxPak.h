// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _GFILE_DRXPAK_H_
#define _GFILE_DRXPAK_H_

#pragma once

#ifdef INCLUDE_SCALEFORM_SDK

	#pragma warning(push)
	#pragma warning(disable : 6326)// Potential comparison of a constant with another constant
	#pragma warning(disable : 6011)// Dereferencing NULL pointer
	#include <DinrusX/CoreX/Platform/DrxWindows.h>
	#include <GFile.h> // includes <windows.h>
	#pragma warning(pop)
	#include <DinrusX/DinrusXSys/IStreamEngineDefs.h>

struct IDrxPak;

class GFileDrxPak : public GFile
{
public:
	// GFile interface
	virtual const char* GetFilePath();
	virtual bool        IsValid();
	virtual bool        IsWritable();
	virtual SInt        Tell();
	virtual SInt64      LTell();
	virtual SInt        GetLength();
	virtual SInt64      LGetLength();
	virtual SInt        GetErrorCode();
	virtual SInt        Write(const UByte* pBuf, SInt numBytes);
	virtual SInt        Read(UByte* pBuf, SInt numBytes);
	virtual SInt        SkipBytes(SInt numBytes);
	virtual SInt        BytesAvailable();
	virtual bool        Flush();
	virtual SInt        Seek(SInt offset, SInt origin = Seek_Set);
	virtual SInt64      LSeek(SInt64 offset, SInt origin = Seek_Set);
	virtual bool        ChangeSize(SInt newSize);
	virtual SInt        CopyFromStream(GFile* pStream, SInt byteSize);
	virtual bool        Close();

public:
	GFileDrxPak(const char* pPath, unsigned int openFlags = 0);
	virtual ~GFileDrxPak();

private:
	bool   IsValidInternal() const { return m_fileHandle != 0; }
	long   TellInternal();
	size_t GetLengthInternal();
	long   SeekInternal(long offset, int origin);

private:
	IDrxPak* m_pPak;
	FILE*    m_fileHandle;
	GString  m_relativeFilePath;
};

class GFileDrxStream : public GFile
{
public:
	// GFile interface
	virtual const char* GetFilePath();
	virtual bool        IsValid();
	virtual bool        IsWritable();
	virtual SInt        Tell();
	virtual SInt64      LTell();
	virtual SInt        GetLength();
	virtual SInt64      LGetLength();
	virtual SInt        GetErrorCode();
	virtual SInt        Write(const UByte* pBuf, SInt numBytes);
	virtual SInt        Read(UByte* pBuf, SInt numBytes);
	virtual SInt        SkipBytes(SInt numBytes);
	virtual SInt        BytesAvailable();
	virtual bool        Flush();
	virtual SInt        Seek(SInt offset, SInt origin = Seek_Set);
	virtual SInt64      LSeek(SInt64 offset, SInt origin = Seek_Set);
	virtual bool        ChangeSize(SInt newSize);
	virtual SInt        CopyFromStream(GFile* pStream, SInt byteSize);
	virtual bool        Close();

public:
	GFileDrxStream(const char* pPath, EStreamTaskType streamType);
	virtual ~GFileDrxStream();

private:
	bool   IsValidInternal() const { return m_isValid; }
	long   TellInternal();
	size_t GetLengthInternal();
	long   SeekInternal(long offset, int origin);
	bool   InRange(long pos) const { return pos >= 0 && (size_t) pos <= m_fileSize; }

private:
	IStreamEngine*  m_pStreamEngine;
	GString         m_relativeFilePath;
	size_t          m_fileSize;
	long            m_curPos;
	bool            m_isValid;
	bool            m_readError;
	EStreamTaskType m_streamType;
};

class GFileInMemoryDrxStream : public GFile
{
public:
	// GFile interface
	virtual const char* GetFilePath();
	virtual bool        IsValid();
	virtual bool        IsWritable();
	virtual SInt        Tell();
	virtual SInt64      LTell();
	virtual SInt        GetLength();
	virtual SInt64      LGetLength();
	virtual SInt        GetErrorCode();
	virtual SInt        Write(const UByte* pBuf, SInt numBytes);
	virtual SInt        Read(UByte* pBuf, SInt numBytes);
	virtual SInt        SkipBytes(SInt numBytes);
	virtual SInt        BytesAvailable();
	virtual bool        Flush();
	virtual SInt        Seek(SInt offset, SInt origin = Seek_Set);
	virtual SInt64      LSeek(SInt64 offset, SInt origin = Seek_Set);
	virtual bool        ChangeSize(SInt newSize);
	virtual SInt        CopyFromStream(GFile* pStream, SInt byteSize);
	virtual bool        Close();

public:
	GFileInMemoryDrxStream(const char* pPath, EStreamTaskType streamType);
	virtual ~GFileInMemoryDrxStream();

private:
	enum ErrCode
	{
		EC_Ok,
		EC_FileNotFound,
		EC_IOError,
		EC_Undefined
	};

private:
	GPtr<GMemoryFile> m_pMemFile;
	unsigned char*    m_pData;
	size_t            m_dataSize;
	ErrCode           m_errCode;
};

#endif // #ifdef INCLUDE_SCALEFORM_SDK

#endif // #ifndef _GFILE_DRXPAK_H_
