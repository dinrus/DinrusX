// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_AudioImplementation
#include <DinrusX/CoreX/Platform/platform.h>
#include <DinrusX/CoreX/StlUtils.h>
#include <DinrusX/CoreX/Project/ProjectDefines.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <fmod_studio.hpp>

#if !defined(_RELEASE)
	#define INCLUDE_FMOD_IMPL_PRODUCTION_CODE
#endif // _RELEASE

#if DRX_PLATFORM_DURANGO
	#define PROVIDE_FMOD_IMPL_SECONDARY_POOL
// Memory Allocation
	#include <DinrusX/CoreX/Memory/Pool/PoolAlloc.h>
#endif // DRX_PLATFORM_DURANGO

namespace DrxAudio
{
namespace Impl
{
namespace Fmod
{
#if defined(PROVIDE_FMOD_IMPL_SECONDARY_POOL)
typedef NDrxPoolAlloc::CThreadSafe<NDrxPoolAlloc::CBestFit<NDrxPoolAlloc::CReferenced<NDrxPoolAlloc::CMemoryDynamic, 4* 1024, true>, NDrxPoolAlloc::CListItemReference>> MemoryPoolReferenced;

extern MemoryPoolReferenced g_audioImplMemoryPoolSecondary;

//////////////////////////////////////////////////////////////////////////
inline void* Secondary_Allocate(size_t const nSize)
{
	// Secondary Memory is Referenced. To not loose the handle, more memory is allocated
	// and at the beginning the handle is saved.

	/* Allocate in Referenced Secondary Pool */
	uint32 const allocHandle = g_audioImplMemoryPoolSecondary.Allocate<uint32>(nSize, AUDIO_MEMORY_ALIGNMENT);
	DRX_ASSERT(allocHandle > 0);
	void* pAlloc = NULL;

	if (allocHandle > 0)
	{
		pAlloc = g_audioImplMemoryPoolSecondary.Resolve<void*>(allocHandle);
	}

	return pAlloc;
}

//////////////////////////////////////////////////////////////////////////
inline bool Secondary_Free(void* pFree)
{
	// Secondary Memory is Referenced. To not loose the handle, more memory is allocated
	// and at the beginning the handle is saved.

	// retrieve handle
	bool bFreed = (pFree == NULL);      //true by default when passing NULL
	uint32 const allocHandle = g_audioImplMemoryPoolSecondary.AddressToHandle(pFree);

	if (allocHandle > 0)
	{
		bFreed = g_audioImplMemoryPoolSecondary.Free(allocHandle);
	}

	return bFreed;
}
#endif // PROVIDE_FMOD_IMPL_SECONDARY_POOL
}      // Fmod
}      // Impl
}      // DrxAudio
