// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   stdafx.h
//  Version:     v1.00
//  Created:     30/9/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Описание: Precompiled Header.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __stdafx_h__
#define __stdafx_h__

#if _MSC_VER > 1000
	#pragma once
#endif

//#define DEFINE_MODULE_NAME "DinrusSystem"

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_System

#define DRXSYSTEM_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

//////////////////////////////////////////////////////////////////////////
// CRT
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#if !DRX_PLATFORM_LINUX && !DRX_PLATFORM_ANDROID && !DRX_PLATFORM_APPLE && !DRX_PLATFORM_ORBIS
	#include <memory.h>
	#include <malloc.h>
#endif
#include <fcntl.h>

#if !DRX_PLATFORM_ORBIS && !DRX_PLATFORM_APPLE && !DRX_PLATFORM_ANDROID
	#if DRX_PLATFORM_LINUX
		#include <sys/io.h>
	#else
		#include <io.h>
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// DRX Stuff ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Math/Random.h>
#include <DinrusX/CoreX/smartptr.h>
#include <DinrusX/CoreX/Math/Range.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>
#include <DinrusX/CoreX/StlUtils.h>

static inline int RoundToClosestMB(size_t memSize)
{
	// add half a MB and shift down to get closest MB
	return((int) ((memSize + (1 << 19)) >> 20));
}

//////////////////////////////////////////////////////////////////////////
// For faster compilation
//////////////////////////////////////////////////////////////////////////
#include <DinrusX/CoreX/Renderer/IRenderer.h>
#include <DinrusX/DinrusXSys/DrxFile.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/CoreX/Audio/IAudioSystem.h>
#include <DinrusX/DinrusXPhys/IPhysics.h>
#include <DinrusX/DinrusXAI/IAISystem.h>
#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXSys/ICmdLine.h>
#include <DinrusX/DinrusXInput/IInput.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/ILog.h>

#include <DinrusX/DinrusXSys/FrameProfiler_JobSystem.h>  // to be removed

/////////////////////////////////////////////////////////////////////////////
//forward declarations for common Interfaces.
/////////////////////////////////////////////////////////////////////////////
class ITexture;
struct IRenderer;
struct ISystem;
struct IScriptSystem;
struct ITimer;
struct IFFont;
struct IInput;
struct IKeyboard;
struct ICVar;
struct IConsole;
struct IEntitySystem;
struct IProcess;
struct IDrxPak;
struct IDrxFont;
struct I3DEngine;
struct IMovieSystem;
struct IPhysicalWorld;

/////////////////////////////////////////////////////////////////////////////
// HMD libraries
/////////////////////////////////////////////////////////////////////////////
#include<DinrusX/DinrusXSys/IHMDDevice.h>

#endif // __stdafx_h__
