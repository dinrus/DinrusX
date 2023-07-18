// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/FileIOWrapper.h>
#include <DinrusX/DinrusXSys/IDiskProfiler.h>
#include <DinrusX/CoreX/Platform/IPlatformOS.h>
#include <DinrusX/CoreX/String/StringUtils.h>
#include <DinrusX/CoreX/Platform/DrxWindows.h>

bool CIOWrapper::m_bLockReads = false;
DrxCriticalSection CIOWrapper::m_ReadCS;

size_t CIOWrapper::Fread(void* pData, size_t nSize, size_t nCount, FILE* hFile)
{
	DRX_PROFILE_FUNCTION(PROFILE_SYSTEM);

	PROFILE_DISK_READ(nSize * nCount);

	if (m_bLockReads)
	{
		AUTO_LOCK_CS(m_ReadCS);
		return ::fread(pData, nSize, nCount, hFile);
	}
	else
	{
		return ::fread(pData, nSize, nCount, hFile);
	}
}

FILE* CIOWrapper::FopenLocked(const char* file, const char* mode)
{
#if DRX_PLATFORM_WINDOWS
	HANDLE handle;

	// The file must exist if opens for 'r' or 'r+'.
	const auto creationDisposition = strchr(mode, 'r') ? OPEN_EXISTING : OPEN_ALWAYS;

	const auto accessMode = (creationDisposition == OPEN_EXISTING) && !strchr(mode, '+') ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);

	handle = CreateFile(file, accessMode, 0, 0, creationDisposition, 0, 0);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return NULL; // failed to lock or open file
	}

	// Get file descriptor and FILE* from handle
	int fhandle = _open_osfhandle((intptr_t)handle, 0);

	if (fhandle == -1)
	{
		CloseHandle(handle);
		return NULL;
	}

	FILE* ret = _fdopen(fhandle, mode);

	if (ret == NULL)
	{
		CloseHandle(handle);
		return NULL;
	}

	return ret;
	// handle doesn't need to be freed, the eventual fclose on ret will do that for us.
#else
	return Fopen(file, mode);
#endif
}

#if defined(USE_FILE_HANDLE_CACHE)
FILE* CIOWrapper::FopenEx(const char* file, const char* mode, FileIoWrapper::FileAccessType type, bool bSysAppHome)
#else
FILE * CIOWrapper::FopenEx(const char* file, const char* mode)
#endif
{
	LOADING_TIME_PROFILE_SECTION;

#if !defined(SYS_ENV_AS_STRUCT)
	PREFAST_ASSUME(gEnv);
#endif

	FILE* f = NULL;

	IPlatformOS* pOS = gEnv->pSystem->GetPlatformOS();
	if (pOS != nullptr && pOS->IsFileAvailable(file))
	{
#if DRX_PLATFORM_WINDOWS
		f = ::_wfopen(DrxStringUtils::UTF8ToWStr(file), DrxStringUtils::UTF8ToWStr(mode));
#elif DRX_PLATFORM_ORBIS
		char buf[512];
		const char* const absoluteFile = ConvertFileName(buf, sizeof(buf), file);
		f = ::fopen(absoluteFile, mode);
#else
		f = ::fopen(file, mode);
#endif
	}

	return f;
}

//////////////////////////////////////////////////////////////////////////
int CIOWrapper::FcloseEx(FILE* hFile)
{
	return fclose(hFile);
}

int64 CIOWrapper::FTell(FILE* hFile)
{
#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE || DRX_PLATFORM_ORBIS
	return (int64)ftell(hFile);
#else
	return _ftelli64(hFile);
#endif
}

int CIOWrapper::FEof(FILE* hFile)
{
	return feof(hFile);
}

int CIOWrapper::FError(FILE* hFile)
{
	return ferror(hFile);
}

int CIOWrapper::Fseek(FILE* hFile, int64 seek, int mode)
{
	return fseek(hFile, (long)seek, mode);
}

void CIOWrapper::LockReadIO(bool lock)
{
	m_bLockReads = lock;
}
