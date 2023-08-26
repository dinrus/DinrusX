// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   SaveReaderWriter_DrxPak.h
//  Created:     15/02/2010 by Alex McCarthy.
//  Описание: Implementation of the ISaveReader and ISaveWriter
//               interfaces using DrxPak
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SAVE_READER_WRITER_DRXPAK_H__
#define __SAVE_READER_WRITER_DRXPAK_H__

#include <DinrusX/CoreX/Platform/IPlatformOS.h>

class CDrxPakFile
{
protected:
	CDrxPakFile(const char* fileName, const char* szMode);
	virtual ~CDrxPakFile();

	IPlatformOS::EFileOperationCode CloseImpl();

	std::vector<uint8>              m_data;
	string                          m_fileName;
	FILE*                           m_pFile;
	size_t                          m_filePos;
	IPlatformOS::EFileOperationCode m_eLastError;
	bool                            m_bWriting;
	static const int                s_dataBlockSize = 128 * 1024;

	NO_INLINE_WEAK static FILE*  FOpen(const char* pName, const char* mode, unsigned nFlags = 0);
	NO_INLINE_WEAK static int    FClose(FILE* fp);
	NO_INLINE_WEAK static size_t FGetSize(FILE* fp);
	NO_INLINE_WEAK static size_t FReadRaw(void* data, size_t length, size_t elems, FILE* fp);
	NO_INLINE_WEAK static size_t FWrite(const void* data, size_t length, size_t elems, FILE* fp);
	NO_INLINE_WEAK static size_t FSeek(FILE* fp, long offset, int mode);

private:
	CScopedAllowFileAccessFromThisThread m_allowFileAccess;
};

class CSaveReader_DrxPak : public IPlatformOS::ISaveReader, public CDrxPakFile
{
public:
	CSaveReader_DrxPak(const char* fileName);

	// ISaveReader
	virtual IPlatformOS::EFileOperationCode Seek(long seek, ESeekMode mode);
	virtual IPlatformOS::EFileOperationCode GetFileCursor(long& fileCursor);
	virtual IPlatformOS::EFileOperationCode ReadBytes(void* data, size_t numBytes);
	virtual IPlatformOS::EFileOperationCode GetNumBytes(size_t& numBytes);
	virtual IPlatformOS::EFileOperationCode Close()           { return CloseImpl(); }
	virtual IPlatformOS::EFileOperationCode LastError() const { return m_eLastError; }
	virtual void                            GetMemoryUsage(IDrxSizer* pSizer) const;
	virtual void                            TouchFile();
	//~ISaveReader
};

DECLARE_SHARED_POINTERS(CSaveReader_DrxPak);

class CSaveWriter_DrxPak : public IPlatformOS::ISaveWriter, public CDrxPakFile
{
public:
	CSaveWriter_DrxPak(const char* fileName);

	// ISaveWriter
	virtual IPlatformOS::EFileOperationCode AppendBytes(const void* data, size_t length);
	virtual IPlatformOS::EFileOperationCode Close()           { return CloseImpl(); }
	virtual IPlatformOS::EFileOperationCode LastError() const { return m_eLastError; }
	virtual void                            GetMemoryUsage(IDrxSizer* pSizer) const;
	//~ISaveWriter
};

DECLARE_SHARED_POINTERS(CSaveWriter_DrxPak);

#endif //__SAVE_READER_WRITER_DRXPAK_H__
