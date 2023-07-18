// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#if !defined(AFX_STDAFX_H__81DAABA0_0054_42BF_8696_D99BA6832D03__INCLUDED_)
#define AFX_STDAFX_H__81DAABA0_0054_42BF_8696_D99BA6832D03__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule   eDrxM_AISystem
#define RWI_NAME_TAG "RayWorldIntersection(AI)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(AI)"

#define DRXAISYS_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

#if !defined(_RELEASE)
	#define DRXAISYS_DEBUG
	#define DRXAISYS_VERBOSITY
	#define COMPILE_WITH_MOVEMENT_SYSTEM_DEBUG
#endif

#include <stdio.h>

#include <memory>
#include <limits>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <list>
#include <set>
#include <deque>

// Reference additional interface headers your program requires here (not local headers)

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Containers/DrxArray.h>
#include <DinrusX/CoreX/Math/Drx_XOptimise.h> // required by AMD64 compiler
#include <DinrusX/CoreX/Math/Drx_Geo.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/ILog.h>
#include <DinrusX/DinrusXNetwork/ISerialize.h>
#include <DinrusX/DinrusXFlowGraph/IFlowSystem.h>
#include <DinrusX/DinrusXAI/IAIAction.h>
#include <DinrusX/DinrusXPhys/IPhysics.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXAI/IAgent.h>
#include <DinrusX/DinrusXAI/IAIActorProxy.h>
#include <DinrusX/DinrusXAI/IAIGroupProxy.h>
#include <DinrusX/DinrusXEntity/IEntity.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXSys/DrxFile.h>
#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXNetwork/ISerialize.h>
#include <DinrusX/CoreX/Renderer/IRenderAuxGeom.h>
#include <DinrusX/CoreX/Math/Random.h>
#include <DinrusX/DinrusXSys/ICodeCheckpointMgr.h>

#include <DinrusX/DinrusXAI/NavigationIdTypes.h>

#include <DinrusX/DinrusXAI/XMLUtils.h>

//Schematyc includes
#include <DinrusX/DinrusXSchema/CoreAPI.h>

#include <DinrusX/DinrusXAI/Environment.h>

// Hijack the old CCCPOINT definition (and add a semi-colon to allow compilation)
#define CCCPOINT(x) CODECHECKPOINT(x);

#include <DinrusX/DinrusXSys/FrameProfiler_JobSystem.h>  // to be removed

#define AI_STRIP_OUT_LEGACY_LOOK_TARGET_CODE

/// This frees the memory allocation for a vector (or similar), rather than just erasing the contents
template<typename T>
void ClearVectorMemory(T& container)
{
	T().swap(container);
}

// adding some headers here can improve build times... but at the cost of the compiler not registering
// changes to these headers if you compile files individually.
#include <DinrusX/DinrusXAI/AILog.h>
#include <DinrusX/DinrusXAI/CAISystem.h>
#include <DinrusX/DinrusXAI/ActorLookUp.h>
// NOTE: (MATT) Reduced this list to the minimum. {2007/07/18:16:24:59}
//////////////////////////////////////////////////////////////////////////

class CAISystem;
ILINE CAISystem* GetAISystem()
{
	extern CAISystem* g_pAISystem;

	return g_pAISystem;
}

//====================================================================
// SetAABBCornerPoints
//====================================================================
inline void SetAABBCornerPoints(const AABB& b, Vec3* pts)
{
	pts[0].Set(b.min.x, b.min.y, b.min.z);
	pts[1].Set(b.max.x, b.min.y, b.min.z);
	pts[2].Set(b.max.x, b.max.y, b.min.z);
	pts[3].Set(b.min.x, b.max.y, b.min.z);

	pts[4].Set(b.min.x, b.min.y, b.max.z);
	pts[5].Set(b.max.x, b.min.y, b.max.z);
	pts[6].Set(b.max.x, b.max.y, b.max.z);
	pts[7].Set(b.min.x, b.max.y, b.max.z);
}

ILINE float LinStep(float a, float b, float x)
{
	float w = (b - a);
	if (w != 0.0f)
	{
		x = (x - a) / w;
		return min(1.0f, max(x, 0.0f));
	}
	return 0.0f;
}

//===================================================================
// HasPointInRange
// (To be replaced)
//===================================================================
ILINE bool HasPointInRange(const std::vector<Vec3>& list, const Vec3& pos, float range)
{
	float r(sqr(range));
	for (uint32 i = 0; i < list.size(); ++i)
		if (Distance::Point_PointSq(list[i], pos) < r)
			return true;
	return false;
}

#ifndef OUT
	#define OUT
#endif

#ifndef IN
	#define IN
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__81DAABA0_0054_42BF_8696_D99BA6832D03__INCLUDED_)
