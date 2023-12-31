// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_Network
#define DRXNETWORK_EXPORTS
#include <DinrusX/CoreX/Platform/platform.h>

#if !defined(_DEBUG)
	#define DRXNETWORK_RELEASEBUILD 1
#else
	#define DRXNETWORK_RELEASEBUILD 0
#endif // #if !defined(_DEBUG)

#define NOT_USE_UBICOM_SDK

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <algorithm>

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>
#include <DinrusX/CoreX/StlUtils.h>

#include <DinrusX/CoreX/Renderer/IRenderer.h>

#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID
	#include <unistd.h>
	#include <fcntl.h>
#endif

#include <memory>
#include <vector>

#if DRX_PLATFORM_WINAPI
	#include <process.h>
	#define S_ADDR_IP4(ADDR) ((ADDR).sin_addr.S_un.S_addr)
#else
	#define S_ADDR_IP4(ADDR) ((ADDR).sin_addr.s_addr)
#endif

//#if defined(_DEBUG) && DRX_PLATFORM_WINDOWS
//#include <crtdbg.h>
//#endif

#define NET_ASSERT_LOGGING 1

#if DRXNETWORK_RELEASEBUILD
	#undef NET_ASSERT_LOGGING
	#define NET_ASSERT_LOGGING 0
#endif
void NET_ASSERT_FAIL(const char* check, const char* file, int line);
#undef NET_ASSERT
#if NET_ASSERT_LOGGING
	#define NET_ASSERT(x) if (true) { bool net_ok = true; if (!(x)) net_ok = false; DRX_ASSERT_MESSAGE(net_ok, # x); if (!net_ok) NET_ASSERT_FAIL( # x, __FILE__, __LINE__); } else
#else
	#define NET_ASSERT assert
#endif

#ifndef NET____TRACE
	#define NET____TRACE

// src and trg can be the same pointer (in place encryption)
// len must be in bytes and must be multiple of 8 byts (64bits).
// key is 128bit:  int key[4] = {n1,n2,n3,n4};
// void encipher(unsigned int *const v,unsigned int *const w,const unsigned int *const k )
	#define TEA_ENCODE(src, trg, len, key) {                                                                                      \
	  unsigned int* v = (src), * w = (trg), * k = (key), nlen = (len) >> 3;                                                       \
	  unsigned int delta = 0x9E3779B9, a = k[0], b = k[1], c = k[2], d = k[3];                                                    \
	  while (nlen--) {                                                                                                            \
	    unsigned int y = v[0], z = v[1], n = 32, sum = 0;                                                                         \
	    while (n-- > 0) { sum += delta; y += (z << 4) + a ^ z + sum ^ (z >> 5) + b; z += (y << 4) + c ^ y + sum ^ (y >> 5) + d; } \
	    w[0] = y; w[1] = z; v += 2, w += 2; }                                                                                     \
}

// src and trg can be the same pointer (in place decryption)
// len must be in bytes and must be multiple of 8 byts (64bits).
// key is 128bit: int key[4] = {n1,n2,n3,n4};
// void decipher(unsigned int *const v,unsigned int *const w,const unsigned int *const k)
	#define TEA_DECODE(src, trg, len, key) {                                                                                      \
	  unsigned int* v = (src), * w = (trg), * k = (key), nlen = (len) >> 3;                                                       \
	  unsigned int delta = 0x9E3779B9, a = k[0], b = k[1], c = k[2], d = k[3];                                                    \
	  while (nlen--) {                                                                                                            \
	    unsigned int y = v[0], z = v[1], sum = 0xC6EF3720, n = 32;                                                                \
	    while (n-- > 0) { z -= (y << 4) + c ^ y + sum ^ (y >> 5) + d; y -= (z << 4) + a ^ z + sum ^ (z >> 5) + b; sum -= delta; } \
	    w[0] = y; w[1] = z; v += 2, w += 2; }                                                                                     \
}

// encode size ignore last 3 bits of size in bytes. (encode by 8bytes min)
	#define TEA_GETSIZE(len) ((len) & (~7))

inline void __cdecl __NET_TRACE(const char* sFormat, ...) PRINTF_PARAMS(1, 2);

inline void __cdecl __NET_TRACE(const char* sFormat, ...)
{
	/*
	   va_list args;
	   static char sTraceString[500];

	   va_start(args, sFormat);
	   drx_vsprintf(sTraceString, sFormat, args);
	   va_end(args);

	   NET_ASSERT(strlen(sTraceString) < 500)

	   NetLog(sTraceString);

	   ::OutputDebugString(sTraceString);*/
}

	#if 1

		#define NET_TRACE __NET_TRACE

	#else

		#define NET_TRACE 1 ? (void)0 : __NET_TRACE;

	#endif //NET____TRACE

#endif //_DEBUG

struct IStreamAllocator
{
	virtual ~IStreamAllocator(){}
	virtual void* Alloc(size_t size, void* callerOverride = 0) = 0;
	virtual void* Realloc(void* old, size_t size) = 0;
	virtual void  Free(void* old) = 0;
};

class CDefaultStreamAllocator : public IStreamAllocator
{
	void* Alloc(size_t size, void*)       { return malloc(size); }
	void* Realloc(void* old, size_t size) { return realloc(old, size); }
	void  Free(void* old)                 { free(old); }
};

#include <DinrusX/DinrusXSys/IValidator.h>   // MAX_WARNING_LENGTH
#include <DinrusX/DinrusXSys/ISystem.h>      // NetLog
#include <DinrusX/DinrusXNetwork/NetLog.h>

#if _MSC_VER > 1000
	#pragma intrinsic(memcpy)
#endif

#include <DinrusX/DinrusXNetwork/INetwork.h>

#ifdef __WITH_PB__
	#include <DinrusX/DinrusXNetwork/PunkBuster/pbcommon.h>
#endif

#include <DinrusX/DinrusXNetwork/objcnt.h>

#if defined(_MSC_VER)
extern "C" void* _ReturnAddress(void);
	#pragma intrinsic(_ReturnAddress)
	#define UP_STACK_PTR _ReturnAddress()
#else
	#define UP_STACK_PTR 0
#endif

#include <DinrusX/CoreX/Platform/DrxWindows.h>
// All headers below include <windows.h> not via DrxWindows.h
// This should be fixed
#include <DinrusX/DinrusXNetwork/ArithAlphabet.h>
#include <DinrusX/DinrusXNetwork/ClientContextView.h>
#include <DinrusX/DinrusXNetwork/ContextView.h>
#include <DinrusX/DinrusXNetwork/NetChannel.h>
