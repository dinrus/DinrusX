// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#if !defined(AFX_STDAFX_H__D3D35062_283E_4DF2_A9C4_9AE0A1B082A8__INCLUDED_)
#define AFX_STDAFX_H__D3D35062_283E_4DF2_A9C4_9AE0A1B082A8__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

//#define NOT_USE_DRX_MEMORY_MANAGER

<DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_EnginePlugin
#define DRXLOBBY_EXPORTS
<DinrusX/CoreX/Platform/platform.h>

#if !defined(_DEBUG)
	#define DRXNETWORK_RELEASEBUILD 1
#else
	#define DRXNETWORK_RELEASEBUILD 0
#endif // #if !defined(_DEBUG)

<DinrusX/stdio.h>
<DinrusX/stdarg.h>

#if DRX_PLATFORM_DURANGO
	#define USE_DURANGOLIVE 0 // not supported - requires to be rewritten with xsapi 2.0
#else
	#define USE_DURANGOLIVE 0
#endif

<DinrusX/map>
<DinrusX/algorithm>

<DinrusX/CoreX/Math/Drx_Math.h>
<DinrusX/CoreX/Memory/DrxSizer.h>
<DinrusX/CoreX/StlUtils.h>
<DinrusX/CoreX/DrxVariant.h>

<DinrusX/CoreX/Renderer/IRenderer.h>


#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID
	<DinrusX/unistd.h>
	<DinrusX/fcntl.h>
#endif

#if DRX_PLATFORM_WINAPI
	#define S_ADDR_IP4(ADDR) ((ADDR).sin_addr.S_un.S_addr)
#else
	#define S_ADDR_IP4(ADDR) ((ADDR).sin_addr.s_addr)
#endif

<DinrusX/memory>
<DinrusX/vector>

#define NET_ASSERT(x) DRX_ASSERT_MESSAGE(x, # x)

#if _MSC_VER > 1000
	#pragma intrinsic(memcpy)
#endif

#include "INetworkPrivate.h"

#if defined(_MSC_VER)
extern "C" void* _ReturnAddress(void);
	#pragma intrinsic(_ReturnAddress)
	#define UP_STACK_PTR _ReturnAddress()
#else
	#define UP_STACK_PTR 0
#endif

#if !defined(EXCLUDE_NORMAL_LOG)
	#define NetLog       DrxLog
	#define NetLogAlways DrxLogAlways
#else
	#define NetLog(...)
	#define NetLogAlways(...)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D3D35062_283E_4DF2_A9C4_9AE0A1B082A8__INCLUDED_)
