// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_Font

#define DRXFONT_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

#include <DinrusX/DinrusXFont/IFont.h>

#include <DinrusX/DinrusXSys/ILog.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/CoreX/Renderer/IRenderer.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>

// USE_NULLFONT should be defined for all platforms running as a pure dedicated server
#if DRX_PLATFORM_DESKTOP
	#ifndef USE_NULLFONT
		#define USE_NULLFONT
	#endif
#endif
