// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_AudioImplementation
#include <DinrusX/CoreX/Platform/platform.h>
#include <DinrusX/CoreX/StlUtils.h>
#include <DinrusX/CoreX/Project/ProjectDefines.h>
#include <DinrusX/DinrusXSys/ISystem.h>

#if !defined(_RELEASE)
	#define INCLUDE_SDLMIXER_IMPL_PRODUCTION_CODE
#endif // _RELEASE
