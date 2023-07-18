// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   dllmain.cpp
//  Version:     v1.00
//  Created:     1/10/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/System.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/DebugCallStack.h>

#if DRX_PLATFORM_DURANGO
	#include "DurangoDebugCallstack.h"
#endif

#ifdef DRX_USE_CRASHRPT
	#include <CrashRpt.h>
extern bool g_bCrashRptInstalled;
#endif // DRX_USE_CRASHRPT

#if defined(INCLUDE_SCALEFORM_SDK) || defined(DRX_FEATURE_SCALEFORM_HELPER)
	#include <DinrusX/DinrusXSys/IScaleformHelper.h>
#endif

// For lua debugger
//#include <malloc.h>

HMODULE gDLLHandle = NULL;

struct DummyInitializer
{
	DummyInitializer()
	{
		dummyValue = 1;
	}

	int dummyValue;
};

DummyInitializer& initDummy()
{
	static DummyInitializer* p = new DummyInitializer;
	return *p;
}

static int warmAllocator = initDummy().dummyValue;

#if !defined(_LIB) && !DRX_PLATFORM_LINUX && !DRX_PLATFORM_ANDROID && !DRX_PLATFORM_APPLE && !DRX_PLATFORM_ORBIS
	#pragma warning( push )
	#pragma warning( disable : 4447 )
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
	gDLLHandle = (HMODULE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;

	//////////////////////////////////////////////////////////////////////////
	case DLL_THREAD_ATTACH:
	#ifdef DRX_USE_CRASHRPT
		if (g_bCrashRptInstalled)
		{
			crInstallToCurrentThread2(0);
		}
	#endif //DRX_USE_CRASHRPT
		break;

	//////////////////////////////////////////////////////////////////////////
	case DLL_THREAD_DETACH:
	#ifdef DRX_USE_CRASHRPT
		if (g_bCrashRptInstalled)
		{
			crUninstallFromCurrentThread();
		}
	#endif //DRX_USE_CRASHRPT
		break;

	}
	//	int sbh = _set_sbh_threshold(1016);

	return TRUE;
}
	#pragma warning( pop )
#endif

#if defined(USE_GLOBAL_BUCKET_ALLOCATOR)
	#include <DinrusX/CoreX/Memory/DrxMemoryAllocator.h>
	#include <DinrusX/CoreX/Memory/BucketAllocator.h>
extern void EnableDynamicBucketCleanups(bool enable);
#endif

//////////////////////////////////////////////////////////////////////////
struct CSystemEventListener_System : public ISystemEventListener
{
public:
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
	{
#if defined(USE_GLOBAL_BUCKET_ALLOCATOR)
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
		case ESYSTEM_EVENT_LEVEL_LOAD_START:
		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			EnableDynamicBucketCleanups(true);
			break;

		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			EnableDynamicBucketCleanups(false);
			break;
		}
#endif

		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
			gEnv->pSystem->SetThreadState(ESubsys_Physics, false);
			break;

		case ESYSTEM_EVENT_LEVEL_LOAD_START:
		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			{
#if defined(INCLUDE_SCALEFORM_SDK) || defined(DRX_FEATURE_SCALEFORM_HELPER)
				if (!gEnv->IsDedicated() && gEnv->pScaleformHelper)
				{
					gEnv->pScaleformHelper->ResetMeshCache();
				}
#endif
				DrxCleanup();
				break;
			}

		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			{
				DrxCleanup();
				gEnv->pSystem->SetThreadState(ESubsys_Physics, true);
				break;
			}
		case ESYSTEM_EVENT_FULL_SHUTDOWN:
		case ESYSTEM_EVENT_FAST_SHUTDOWN:
			{
				if (gEnv && gEnv->pSystem)
				{
					ISystemEventDispatcher* pSystemEventDispatcher = gEnv->pSystem->GetISystemEventDispatcher();
					if (pSystemEventDispatcher)
					{
						pSystemEventDispatcher->RemoveListener(this);
					}
				}
			}
			break;
		}
	}
};

static CSystemEventListener_System g_system_event_listener_system;

extern "C"
{
	DRXSYSTEM_API ISystem* CreateSystemInterface(SSystemInitParams& startupParams, bool bManualEngineLoop)
	{
#if CAPTURE_REPLAY_LOG
		DrxGetIMemReplay()->StartOnCommandLine(startupParams.szSystemCmdLine);
#endif

#if DRX_PLATFORM_DURANGO && defined(ENABLE_PROFILING_CODE)
		DurangoDebugCallStack::InstallExceptionHandler();
#endif

		std::unique_ptr<CSystem> pSystem = stl::make_unique<CSystem>(startupParams);
		startupParams.pSystem = pSystem.get();

		LOADING_TIME_PROFILE_SECTION_NAMED("CreateSystemInterface");
		ModuleInitISystem(pSystem.get(), "DinrusSystem");
#if DRX_PLATFORM_DURANGO
#if !defined(_LIB)
		gEnv = pSystem->GetGlobalEnvironment();
#endif
		gEnv->pWindow = startupParams.hWnd;
#endif

#if DRX_PLATFORM_WINDOWS
		// Install exception handler in Release modes.
		((DebugCallStack*)IDebugCallStack::instance())->installErrorHandler(pSystem.get());
#endif

		IDrxFactoryRegistryImpl* pDrxFactoryImpl = static_cast<IDrxFactoryRegistryImpl*>(pSystem->GetDrxFactoryRegistry());
		pDrxFactoryImpl->RegisterFactories(g_pHeadToRegFactories);

		// the earliest point the system exists - w2e tell the callback
		if (startupParams.pUserCallback)
			startupParams.pUserCallback->OnSystemConnect(pSystem.get());

		if (!pSystem->Initialize(startupParams))
		{
			DrxMessageBox("DinrusSystem initialization failed!", "Engine initialization failed!");

			return nullptr;
		}

		pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_DRXSYSTEM_INIT_DONE, 0, 0);
		pSystem->GetISystemEventDispatcher()->RegisterListener(&g_system_event_listener_system,"CSystemEventListener_System");

		// run main loop
		if (!bManualEngineLoop)
		{
			pSystem->RunMainLoop();
			return nullptr;
		}
		return pSystem.release();
	}

#if defined(ENABLE_PROFILING_CODE) && !DRX_PLATFORM_LINUX && !DRX_PLATFORM_ANDROID && !DRX_PLATFORM_APPLE
	DRXSYSTEM_API void DrxInstallPostExceptionHandler(void (* PostExceptionHandlerCallback)())
	{
		return IDebugCallStack::instance()->FileCreationCallback(PostExceptionHandlerCallback);
	}
#endif

};
