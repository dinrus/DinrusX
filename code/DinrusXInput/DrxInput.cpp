// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXInput/StdAfx.h>

// Included only once per DLL module.
#include <DinrusX/CoreX/Platform/platform_impl.inl>

#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

#include <DinrusX/DinrusXInput/BaseInput.h>

#include <DinrusX/DinrusXInput/Synergy/SynergyContext.h>
#include <DinrusX/DinrusXInput/Synergy/SynergyKeyboard.h>
#include <DinrusX/DinrusXInput/Synergy/SynergyMouse.h>

#if DRX_PLATFORM_WINDOWS
	#ifndef _LIB
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxInput : public IInputEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IInputEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxInput, "EngineModule_DrxInput", "3cc05160-71bb-44f6-ae52-5949f30277f9"_drx_guid)

	virtual ~CEngineModule_DrxInput() {}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DinrusXInput"; };
	virtual const char* GetCategory() const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;

		IInput* pInput = 0;

		//Specific input systems only make sense in 'normal' mode when renderer is on
		if (gEnv->pRenderer)
		{
#if defined(USE_DXINPUT)
			pInput = new CDXInput(pSystem, (HWND) initParams.hWnd);
#elif defined(USE_DURANGOINPUT)
			pInput = new CDurangoInput(pSystem);
#elif defined(USE_LINUXINPUT)
			pInput = new CLinuxInput(pSystem);
#elif defined(USE_ORBIS_INPUT)
			pInput = new COrbisInput(pSystem);
#else
			pInput = new CBaseInput();
#endif
		}
		else
		{
			pInput = new CBaseInput();
		}

		if (!pInput->Init())
		{
			delete pInput;
			return false;
		}

#ifdef USE_SYNERGY_INPUT
		const char* pServer = g_pInputCVars->i_synergyServer->GetString();
		if (pServer && pServer[0] != '\0')
		{
			_smart_ptr<CSynergyContext> pContext = new CSynergyContext(g_pInputCVars->i_synergyScreenName->GetString(), pServer);

			if (!gEnv->pThreadUpr->SpawnThread(pContext.get(), "Synergy"))
			{
				DrxFatalError("Error spawning \"Synergy\" thread.");
			}

			pInput->AddInputDevice(new CSynergyKeyboard(*pInput, pContext));
			pInput->AddInputDevice(new CSynergyMouse(*pInput, pContext));
		}
#endif

		env.pInput = pInput;

		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxInput)

#include <DinrusX/CoreX/CrtDebugStats.h>
