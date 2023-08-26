// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <DinrusX/CoreX/Platform/platform.h>

#define DRX_USE_ATL
#include <DinrusX/CoreX/Platform/DrxAtlMfc.h>

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/String/DrxPath.h>

#include <stdio.h>
#include <tchar.h>

#include <vector>
#include <map>

extern HMODULE g_hInst;



#ifndef ReleasePpo
#define ReleasePpo(ppo) \
	if (*(ppo) != NULL) \
		{ \
		(*(ppo))->Release(); \
		*(ppo) = NULL; \
		} \
		else (VOID)0
#endif

// TODO: reference additional headers your program requires here
