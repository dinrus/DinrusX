// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// This file should only be included Once in DLL module.

#pragma once

#include <DinrusX/CoreX/Platform/platform.h>

#if defined(_LIB)
#	error It is not allowed to have _LIB defined
#endif

#include <DinrusX/CoreX/TypeInfo_impl.h>
#define DRX_PLATFORM_IMPL_H_FILE 1
#include <DinrusX/CoreX/DrxTypeInfo.inl>
#undef DRX_PLATFORM_IMPL_H_FILE

#include <IRCLog.h>
#include <DinrusX/CoreX/Math/Random.h>

#if DRX_PLATFORM_WINAPI && defined(DRX_IS_APPLICATION) 
// This belongs to the ClassFactoryUpr::the() singleton in ClassFactory.h and must only exist in executables, not in DLLs.
#include <DrxSerialization/yasli/ClassFactory.h>
extern "C" DLL_EXPORT yasli::ClassFactoryUpr* GetYasliClassFactoryUpr()
{
#if defined(NOT_USE_DRX_MEMORY_MANAGER)
	// Cannot be used by code that uses DrxMemoryUpr as it might not be initialized yet.
	static yasli::ClassFactoryUpr* g_classFactoryUpr = nullptr;
	if (g_classFactoryUpr == nullptr)
	{
		g_classFactoryUpr = new yasli::ClassFactoryUpr();
	}
	return g_classFactoryUpr;
#else
	// Cannot be used in Sandbox due as we would create a static while creating a static. MSVC doesn't like that.
	static yasli::ClassFactoryUpr classFactoryUpr;
	return &classFactoryUpr;
#endif
}
#endif

struct SSystemGlobalEnvironment;
SSystemGlobalEnvironment* gEnv = 0;
IRCLog* g_pRCLog = 0;


void SetRCLog(IRCLog* pRCLog)
{
	g_pRCLog = pRCLog;
}

void RCLog(const char* szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);
	if (g_pRCLog)
	{
		g_pRCLog->LogV(IRCLog::eType_Info, szFormat, args);
	}
	else
	{
		vprintf(szFormat, args);
		printf("\n");
		fflush(stdout);
	}
	va_end(args);
}

void RCLogWarning(const char* szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);
	if (g_pRCLog)
	{
		g_pRCLog->LogV(IRCLog::eType_Warning, szFormat, args);
	}
	else
	{
		vprintf(szFormat, args);
		printf("\n");
		fflush(stdout);
	}
	va_end(args);
}

void RCLogError(const char* szFormat, ...)
{
	va_list args;
	va_start (args, szFormat);
	if (g_pRCLog)
	{
		g_pRCLog->LogV(IRCLog::eType_Error, szFormat, args);
	}
	else
	{
		vprintf(szFormat, args);
		printf("\n");
		fflush(stdout);
	}
	va_end(args);
}


//////////////////////////////////////////////////////////////////////////
// Log important data that must be printed regardless verbosity.

void PlatformLogOutput(const char *, ...) PRINTF_PARAMS(1, 2);

inline void PlatformLogOutput( const char *format,... )
{
	assert(g_pRCLog);
	if (g_pRCLog)		
	{
		va_list args;
		va_start(args,format);
		g_pRCLog->LogV(IRCLog::eType_Error,  format, args );
		va_end(args);
	}
}
