// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Thread/DrxThread.h>

// Include architecture specific code.
#if DRX_PLATFORM_WINAPI
	#include <DinrusX/CoreX/Thread/DrxAtomics_impl_win32.h>
	#include <DinrusX/CoreX/Thread/DrxThreadImpl_win32.h>
#elif DRX_PLATFORM_ORBIS
	#include <DinrusX/CoreX/Thread/DrxThreadImpl_sce.h>
#elif DRX_PLATFORM_POSIX
	#include <DinrusX/CoreX/Thread/DrxThreadImpl_posix.h>
#else
// Put other platform specific includes here!
#endif

// vim:ts=2
