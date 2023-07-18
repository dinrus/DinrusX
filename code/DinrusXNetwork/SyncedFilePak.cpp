// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/SyncedFilePak.h>

#if SERVER_FILE_SYNC_MODE

	#include  <DinrusX/DinrusXNetwork/SyncedFileSet.h>

class CSyncedFilePak::CNonSyncedManip : public CSyncedFilePak::IFileManip
{
public:
	explicit CNonSyncedManip(FILE* f) : m_file(f)
	{
		NET_ASSERT(f);
	}
	~CNonSyncedManip()
	{
		gEnv->pDrxPak->FClose(m_file);
	}

	size_t ReadRaw(void* data, size_t length, size_t elems)
	{
		return gEnv->pDrxPak->FReadRaw(data, length, elems, m_file);
	}

	size_t Seek(long seek, int mode)
	{
		return gEnv->pDrxPak->FSeek(m_file, seek, mode);
	}

	size_t Tell()
	{
		return gEnv->pDrxPak->FTell(m_file);
	}

	int Eof()
	{
		return gEnv->pDrxPak->FEof(m_file);
	}

private:
	FILE* m_file;
};

class CSyncedFilePak::CSyncedManip : public CSyncedFilePak::IFileManip
{
public:
	explicit CSyncedManip(const CSyncedFileDataLock& lk) : m_lk(lk), m_cursor(0) {}

	size_t ReadRaw(void* data, size_t length, size_t elems)
	{
		uint32 elemsleft = (m_lk.GetLength() - m_cursor) / elems;
		uint32 elemsread = std::min(length, (size_t)elemsleft);
		uint32 bytesread = elemsread * elems;
		memcpy(data, m_lk.GetData() + m_cursor, bytesread);
		m_cursor += bytesread;
		return elemsread;
	}

	size_t Seek(long seek, int mode)
	{
		uint32 setpos = -1;
		switch (mode)
		{
		case SEEK_SET:
			setpos = seek;
			break;
		case SEEK_CUR:
			setpos = m_cursor + seek;
			break;
		case SEEK_END:
			setpos = m_lk.GetLength() - seek;
			break;
		}
		if (setpos < m_lk.GetLength())
		{
			m_cursor = setpos;
			return 0;
		}
		else
		{
			return 1;
		}
	}

	size_t Tell()
	{
		return m_cursor;
	}

	int Eof()
	{
		return (m_cursor >= m_lk.GetLength()) ? 1 : 0;
	}

private:
	CSyncedFileDataLock m_lk;
	uint32              m_cursor;
};

const char* CSyncedFilePak::AdjustFileName(const char* src, char dst[g_nMaxPath], unsigned nFlags, bool* bFoundInPak /*=NULL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

bool CSyncedFilePak::Init(const char* szBasePath)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void CSyncedFilePak::Release()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

bool CSyncedFilePak::OpenPack(const char* pName, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::OpenPack(const char* pBindingRoot, const char* pName, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::ClosePack(const char* pName, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::OpenPacks(const char* pWildcard, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::OpenPacks(const char* pBindingRoot, const char* pWildcard, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::ClosePacks(const char* pWildcard, unsigned nFlags /*= FLAGS_PATH_REAL*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

void CSyncedFilePak::AddMod(const char* szMod)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::RemoveMod(const char* szMod)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::ParseAliases(const char* szCommandLine)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::SetAlias(const char* szName, const char* szAlias, bool bAdd)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

const char* CSyncedFilePak::GetAlias(const char* szName, bool bReturnSame /*=true*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void CSyncedFilePak::Lock()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::Unlock()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::SetGameFolder(const char* szFolder)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

const char* CSyncedFilePak::GetGameFolder() const
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

IDrxPak::PakInfo* CSyncedFilePak::GetPakInfo()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void CSyncedFilePak::FreePakInfo(PakInfo*)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

FILE* CSyncedFilePak::FOpen(const char* pName, const char* mode, unsigned nFlags /*= 0*/)
{
	if (0 == strcmp("r", mode) || 0 == strcmp("rb", mode))
	{
		IFileManip* file = 0;
		if (CSyncedFilePtr pSyncedFile = m_fileset.GetSyncedFile(pName))
		{
			CSyncedFileDataLock flk(pSyncedFile);
			if (flk.Ok())
				file = new CSyncedManip(flk);
		}
		else
		{
			FILE* oldfile = gEnv->pDrxPak->FOpen(pName, mode, nFlags);
			if (oldfile)
				file = new CNonSyncedManip(oldfile);
		}
		return reinterpret_cast<FILE*>(file);
	}
	else
	{
		DrxFatalError("%s not implemented for mode '%s'", __FUNCTION__, mode);
		return NULL;
	}
}

FILE* CSyncedFilePak::FOpen(const char* pName, const char* mode, char* szFileGamePath, int nLen)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return NULL;
}

size_t CSyncedFilePak::FReadRaw(void* data, size_t length, size_t elems, FILE* handle)
{
	return reinterpret_cast<IFileManip*>(handle)->ReadRaw(data, length, elems);
}

size_t CSyncedFilePak::FReadRawAll(void* data, size_t nFileSize, FILE* handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void* CSyncedFilePak::FGetCachedFileData(FILE* handle, size_t& nFileSize)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

size_t CSyncedFilePak::FWrite(const void* data, size_t length, size_t elems, FILE* handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::FPrintf(FILE* handle, const char* format, ...)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

char* CSyncedFilePak::FGets(char*, int, FILE*)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::Getc(FILE*)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

size_t CSyncedFilePak::FGetSize(FILE* f)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

size_t CSyncedFilePak::FGetSize(const char* pName)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::Ungetc(int c, FILE*)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

bool CSyncedFilePak::IsInPak(FILE* handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::RemoveFile(const char* pName)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::RemoveDir(const char* pName, bool bRecurse)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::IsAbsPath(const char* pPath)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::CopyFileOnDisk(const char* source, const char* dest, bool bFailIfExist)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

size_t CSyncedFilePak::FSeek(FILE* handle, long seek, int mode)
{
	return reinterpret_cast<IFileManip*>(handle)->Seek(seek, mode);
}

long CSyncedFilePak::FTell(FILE* handle)
{
	return reinterpret_cast<IFileManip*>(handle)->Tell();
}

int CSyncedFilePak::FClose(FILE* handle)
{
	delete reinterpret_cast<IFileManip*>(handle);
	return 0;
}

int CSyncedFilePak::FEof(FILE* handle)
{
	return reinterpret_cast<IFileManip*>(handle)->Eof();
}

int CSyncedFilePak::FError(FILE* handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::FGetErrno()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::FFlush(FILE* handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void* CSyncedFilePak::PoolMalloc(size_t size)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void CSyncedFilePak::PoolFree(void* p)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

intptr_t CSyncedFilePak::FindFirst(const char* pDir, _finddata_t* fd, unsigned int nFlags /*=0 */)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::FindNext(intptr_t handle, _finddata_t* fd)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::FindClose(intptr_t handle)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

IDrxPak::FileTime CSyncedFilePak::GetModificationTime(FILE* f)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

bool CSyncedFilePak::IsFileExist(const char* sFilename)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

bool CSyncedFilePak::MakeDir(const char* szPath, bool bGamePathMapping /*=false */)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

IDrxArchive* CSyncedFilePak::OpenArchive(const char* szPath, unsigned int nFlags /*=0*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

const char* CSyncedFilePak::GetFileArchivePath(FILE* f)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::RawCompress(const void* pUncompressed, unsigned long* pDestSize, void* pCompressed, unsigned long nSrcSize, int nLevel /*= -1*/)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

int CSyncedFilePak::RawUncompress(void* pUncompressed, unsigned long* pDestSize, const void* pCompressed, unsigned long nSrcSize)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

void CSyncedFilePak::RecordFileOpen(const ERecordFileOpenList eList)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::RecordFile(FILE* in, const char* szFilename)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

IResourceList* CSyncedFilePak::GetResourceList(const ERecordFileOpenList eList)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

IDrxPak::ERecordFileOpenList CSyncedFilePak::GetRecordFileOpenList()
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return RFOM_Disabled;
}

void CSyncedFilePak::Notify(ENotifyEvent event)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

uint32 CSyncedFilePak::ComputeCRC(const char* szPath)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return 0;
}

bool CSyncedFilePak::ComputeMD5(const char* szPath, unsigned char* md5)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

void CSyncedFilePak::RegisterFileAccessSink(IDrxPakFileAcesssSink* pSink)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

void CSyncedFilePak::UnregisterFileAccessSink(IDrxPakFileAcesssSink* pSink)
{
	DrxFatalError("%s not implemented", __FUNCTION__);
}

bool CSyncedFilePak::GetLvlResStatus() const
{
	DrxFatalError("%s not implemented", __FUNCTION__);
	return false;
}

#endif
