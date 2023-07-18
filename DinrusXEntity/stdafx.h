// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule   eDrxM_EntitySystem
#define RWI_NAME_TAG "RayWorldIntersection(EntitySys)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(EntitySys)"

#define DRXENTITYDLL_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

#pragma warning (error : 4018) //Cannot align catch objects to greater than 16 bytes

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#pragma warning (disable : 4768) //Cannot align catch objects to greater than 16 bytes
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <map>

//////////////////////////////////////////////////////////////////////////

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Math/Drx_XOptimise.h>
#include <DinrusX/CoreX/Math/Drx_GeoIntersect.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>
#include <DinrusX/CoreX/smartptr.h>
#include <DinrusX/CoreX/StlUtils.h>

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

//#include <DinrusX/DinrusXEntity/EntityDesc.h>
//#include <IEntitySystem.h>

#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXPhys/IPhysics.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>
#include <DinrusX/DinrusXSys/TimeValue.h>
#include <DinrusX/CoreX/Memory/PoolAllocator.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>

#include <DinrusX/DinrusXEntity/EntityCVars.h>

#if !defined(_RELEASE)
	#define INCLUDE_ENTITYSYSTEM_PRODUCTION_CODE
#endif // !_RELEASE

//////////////////////////////////////////////////////////////////////////
class CEntitySystem;
extern CEntitySystem* g_pIEntitySystem;
ILINE CEntitySystem* GetIEntitySystem() { return g_pIEntitySystem; }
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//! Reports a Game Warning to validator with WARNING severity.
inline void EntityWarning(const char* format, ...) PRINTF_PARAMS(1, 2);
inline void EntityWarning(const char* format, ...)
{
	if (!format)
		return;

	va_list args;
	va_start(args, format);
	gEnv->pSystem->WarningV(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, 0, NULL, format, args);
	va_end(args);
}

inline void EntityFileWarning(const char* file, const char* format, ...) PRINTF_PARAMS(2, 3);
inline void EntityFileWarning(const char* file, const char* format, ...)
{
	if (!format)
		return;

	va_list args;
	va_start(args, format);
	gEnv->pSystem->WarningV(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, 0, file, format, args);
	va_end(args);
}

#define ENTITY_PROFILER DRX_PROFILE_FUNCTION(PROFILE_ENTITY);
#define ENTITY_PROFILER_NAME(str) DRX_PROFILE_REGION(PROFILE_ENTITY, str);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
