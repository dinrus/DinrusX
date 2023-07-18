// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>

#define eDrxModule eDrxM_LiveCreate
#define DRXLIVECREATE_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

#if defined(_DEBUG) && DRX_PLATFORM_WINAPI
	#include <crtdbg.h>
#endif

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXMovie/IMovieSystem.h>
#include <DinrusX/CoreX/smartptr.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>

#include <vector>
#include <set>
#include <map>

#if (defined(DEDICATED_SERVER) || defined(_RELEASE)) && !defined(NO_LIVECREATE)
	#define NO_LIVECREATE
#endif

#include <DinrusX/DinrusXLiveCreate/ILiveCreateCommon.h>
#include <DinrusX/DinrusXLiveCreate/ILiveCreatePlatform.h>
#include <DinrusX/DinrusXLiveCreate/ILiveCreateUpr.h>
#include <DinrusX/DinrusXLiveCreate/ILiveCreateHost.h>
