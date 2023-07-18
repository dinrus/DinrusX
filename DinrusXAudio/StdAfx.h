// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_AudioSystem
#include <DinrusX/CoreX/Platform/platform.h>
#include <DinrusX/CoreX/StlUtils.h>
#include <DinrusX/CoreX/Project/ProjectDefines.h>
#include <DinrusX/DinrusXSys/ITimer.h>

#if !defined(_RELEASE)
	#define INCLUDE_AUDIO_PRODUCTION_CODE
	#define ENABLE_AUDIO_LOGGING
#endif // _RELEASE
