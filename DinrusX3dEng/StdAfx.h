// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   stdafx.h
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Compilers:   Visual Studio.NET
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__8B93AD4E_EE86_4127_9BED_37AC6D0F978B__INCLUDED_3DENGINE)
#define AFX_STDAFX_H__8B93AD4E_EE86_4127_9BED_37AC6D0F978B__INCLUDED_3DENGINE

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule   eDrxM_3DEngine
#define RWI_NAME_TAG "RayWorldIntersection(3dEngine)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(3dEngine)"
#include <DinrusX/CoreX/Platform/platform.h>

#define DRX3DENGINE_EXPORTS

const int nThreadsNum = 3;

//#define DEFINE_MODULE_NAME "DinrusX3dEng"

//////////////////////////////////////////////////////////////////////////////////////////////
// Highlevel defines

// deferred cull queue handling - currently disabled
// #define USE_CULL_QUEUE

// Drxsis3 as it's dx11 only can simply use the zbufferculler everywhere
// Older CCoverageBuffer currently does not compile
#if 1
	#define OCCLUSIONCULLER CZBufferCuller
#endif

// Compilation (Export to Engine) not needed on consoles
#if DRX_PLATFORM_DESKTOP
	#define ENGINE_ENABLE_COMPILATION 1
#else
	#define ENGINE_ENABLE_COMPILATION 0
#endif

#if !defined(_RELEASE) && DRX_PLATFORM_WINDOWS
	#define ENABLE_CONSOLE_MTL_VIZ
#endif

#pragma warning( error: 4018 )

#include <stdio.h>

#define MAX_PATH_LENGTH 512

#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXSys/IProcess.h>
#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Math/Drx_XOptimise.h>
#include <DinrusX/CoreX/Math/Drx_Geo.h>
#include <DinrusX/DinrusXSys/ILog.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXPhys/IPhysics.h>
#include <DinrusX/CoreX/Renderer/IRenderer.h>
#include <DinrusX/CoreX/Renderer/IRenderAuxGeom.h>
#include <DinrusX/DinrusX3dEng/IRenderNode.h>
#include <DinrusX/CoreX/Containers/StackContainer.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>
#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/DinrusXSys/DrxFile.h>
#include <DinrusX/CoreX/smartptr.h>
#include <DinrusX/CoreX/Containers/DrxArray.h>
#include <DinrusX/DinrusX3dEng/DrxHeaders.h>
#include <DinrusX/DinrusX3dEng/DinrusX3dEngBase.h>
#include <float.h>
#include <DinrusX/CoreX/Containers/DrxArray.h>
#include "cvars.h"
#include <DinrusX/CoreX/Memory/DrxSizer.h>
#include <DinrusX/CoreX/StlUtils.h>
#include "Array2d.h"
#include "Material.h"
#include <DinrusX/DinrusX3dEng/3dEngine.h>
#include <DinrusX/DinrusX3dEng/ObjMan.h>
#include <DinrusX/DinrusX3dEng/Vegetation.h>
#include <DinrusX/DinrusX3dEng/terrain.h>
#include <DinrusX/DinrusX3dEng/ObjectsTree.h>

#include <DinrusX/DinrusXSys/FrameProfiler_JobSystem.h>  // to be removed

template<class T>
void AddToPtr(byte*& pPtr, T& rObj, EEndian eEndian)
{
	PREFAST_SUPPRESS_WARNING(6326) static_assert((sizeof(T) % 4) == 0, "Invalid type size!");
	assert(!((INT_PTR)pPtr & 3));
	memcpy(pPtr, &rObj, sizeof(rObj));
	SwapEndian(*(T*)pPtr, eEndian);
	pPtr += sizeof(rObj);
	assert(!((INT_PTR)pPtr & 3));
}

template<class T>
void AddToPtr(byte*& pPtr, int& nDataSize, T& rObj, EEndian eEndian)
{
	PREFAST_SUPPRESS_WARNING(6326) static_assert((sizeof(T) % 4) == 0, "Invalid type size!");
	assert(!((INT_PTR)pPtr & 3));
	memcpy(pPtr, &rObj, sizeof(rObj));
	SwapEndian(*(T*)pPtr, eEndian);
	pPtr += sizeof(rObj);
	nDataSize -= sizeof(rObj);
	assert(nDataSize >= 0);
	assert(!((INT_PTR)pPtr & 3));
}

inline void FixAlignment(byte*& pPtr, int& nDataSize)
{
	while ((UINT_PTR)pPtr & 3)
	{
		*pPtr = 222;
		pPtr++;
		nDataSize--;
	}
}

inline void FixAlignment(byte*& pPtr)
{
	while ((UINT_PTR)pPtr & 3)
	{
		*pPtr = 222;
		pPtr++;
	}
}

template<class T>
void AddToPtr(byte*& pPtr, int& nDataSize, const T* pArray, int nElemNum, EEndian eEndian, bool bFixAlignment = false)
{
	assert(!((INT_PTR)pPtr & 3));
	memcpy(pPtr, pArray, nElemNum * sizeof(T));
	SwapEndian((T*)pPtr, nElemNum, eEndian);
	pPtr += nElemNum * sizeof(T);
	nDataSize -= nElemNum * sizeof(T);
	assert(nDataSize >= 0);

	if (bFixAlignment)
		FixAlignment(pPtr, nDataSize);
	else
		assert(!((INT_PTR)pPtr & 3));
}

template<class T>
void AddToPtr(byte*& pPtr, const T* pArray, int nElemNum, EEndian eEndian, bool bFixAlignment = false)
{
	assert(!((INT_PTR)pPtr & 3));
	memcpy(pPtr, pArray, nElemNum * sizeof(T));
	SwapEndian((T*)pPtr, nElemNum, eEndian);
	pPtr += nElemNum * sizeof(T);

	if (bFixAlignment)
		FixAlignment(pPtr);
	else
		assert(!((INT_PTR)pPtr & 3));
}

struct TriangleIndex
{
	TriangleIndex() { ZeroStruct(*this); }
	uint16&       operator[](const int& n)       { assert(n >= 0 && n < 3); return idx[n]; }
	const uint16& operator[](const int& n) const { assert(n >= 0 && n < 3); return idx[n]; }
	uint16        idx[3];
	uint16        nCull;
};

#define FUNCTION_PROFILER_3DENGINE DRX_PROFILE_FUNCTION(PROFILE_3DENGINE)

#if DRX_PLATFORM_DESKTOP
	#define INCLUDE_SAVECGF
#endif

#define ENABLE_TYPE_INFO_NAMES 1

#endif // !defined(AFX_STDAFX_H__8B93AD4E_EE86_4127_9BED_37AC6D0F978B__INCLUDED_3DENGINE)
