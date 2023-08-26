// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#ifndef _DinrusX_StdAfx_h_
#define _DinrusX_StdAfx_h_

#pragma once

#if DRX_PLATFORM_WINDOWS
	#include <DinrusX/CoreX/Platform/DrxWindows.h>
	#include <ShellApi.h>
#endif

#include <DinrusX/CoreX/Platform/DrxLibrary.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_DinrusX
#endif
