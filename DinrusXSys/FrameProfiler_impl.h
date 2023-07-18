// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   FrameProfiler_impl.pc.h
//  Version:     v1.00
//  Created:     11/12/2002 by Christopher Bolte.
//  Compilers:   Visual Studio.NET
//  Описание: Platform Profiling Marker Implementation, dispatches to the correct header file
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#if !defined(DRX_PROFILE_MARKER_IMPL_H_)
#define DRX_PROFILE_MARKER_IMPL_H_

#include <DinrusX/DinrusXSys/FrameProfiler.h>
#include <DinrusX/CoreX/Thread/IJobUpr.h>

//! A list of colors which are used to distinguish between different profile label types.
ColorF profile_colors[] =
{
	// When modifying this array, always adjust EProfileDescription

	Col_Red,            //!< 0: Undefined

#if ALLOW_BROFILER
	Col_Transparent,    //!< 1: FunctionEntry - do not show in hierarchical view
	Col_Transparent,    //!< 2: Section - do not show in hierarchical view
#else
	Col_Orchid,         //!< 1: FunctionEntry
	Col_Coral,          //!< 2: Section
#endif
	Col_SkyBlue,        //!< 3: Region

	Col_Transparent,    //!< 4: Undefined

#if ALLOW_BROFILER
	Col_Transparent,    //!< 5: FunctionEntry - Waiting - do not show in hierarchical view
	Col_Transparent,    //!< 6: Section - Waiting - do not show in hierarchical view
#else
	Col_Yellow,         //!< 5: FunctionEntry - Waiting
	Col_Yellow,         //!< 6: Section - Waiting
#endif
	Col_Yellow,         //!< 7: Region - Waiting

	// Do not add any other labels - color lookup is defined as byte
};

#if (!defined(_LIB) || defined(_LAUNCHER)) && defined(ENABLE_PROFILING_CODE) && !((defined(SANDBOX_EXPORTS) || defined(PLUGIN_EXPORTS)))
	#if DRX_PLATFORM_WINDOWS || DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE
		#include <DinrusX/DinrusXSys/FrameProfiler_impl.pc.h>
	#elif DRX_PLATFORM_DURANGO
		#include <DinrusX/DinrusXSys/FrameProfiler_impl.durango.h>
	#elif DRX_PLATFORM_ORBIS
		#include <DinrusX/DinrusXSys/FrameProfiler_impl.orbis.h>
	#else
		#error No Platform support for Profile Marker
	#endif
#else
/* inline */ void DrxProfile::detail::SetThreadName(const char* pName)                           {}
/* inline */ void DrxProfile::detail::SetProfilingEvent(const BYTE colorId, const char* Name)    {}
/* inline */ void DrxProfile::detail::PushProfilingMarker(const BYTE colorId, const char* pName) {}
/* inline */ void DrxProfile::detail::PopProfilingMarker(const BYTE colorId, const char* pName)  {}
/* inline */ void DrxProfile::detail::ProfilerPause()                                            {}
/* inline */ void DrxProfile::detail::ProfilerResume()                                           {}
/* inline */ void DrxProfile::detail::ProfilerFrameStart(int nFrameId)                           {}
/* inline */ void DrxProfile::detail::ProfilerFrameEnd(int nFrameId)                             {}
#endif

//////////////////////////////////////////////////////////////////////////
//! Direct push/pop for profiling markers.
void DrxProfile::PushProfilingMarker(const EProfileDescription desc, const char* pName, ...)
{
	va_list args;
	va_start(args, pName);

	// Format marker name
	char markerName[256];
	if (strlen(pName) == 0)
		__debugbreak();
	const int cNumCharsNeeded = vsnprintf(markerName, DRX_ARRAY_COUNT(markerName), pName, args);
	if (cNumCharsNeeded > DRX_ARRAY_COUNT(markerName) - 1 || cNumCharsNeeded < 0)
	{
		markerName[DRX_ARRAY_COUNT(markerName) - 1] = '\0'; // The WinApi only null terminates if strLen < bufSize
		DrxWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "ProfileEvent: Marker name \"%s\" has been truncated. Max characters allowed: %i. ", pName, DRX_ARRAY_COUNT(markerName) - 1);
	}

	// Set marker
	gEnv->pJobUpr->PushProfilingMarker(markerName);
	va_end(args);
}

//////////////////////////////////////////////////////////////////////////
void DrxProfile::PopProfilingMarker(const EProfileDescription desc, const char* pName)
{
	gEnv->pJobUpr->PopProfilingMarker();
}

#endif // DRX_PROFILE_MARKER_IMPL_H_
