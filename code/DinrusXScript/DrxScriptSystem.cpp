// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXScript/StdAfx.h>
#include <DinrusX/DinrusXScript/ScriptSystem.h>

// Included only once per DLL module.
#include <DinrusX/CoreX/Platform/platform_impl.inl>

#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DinrusXScriptSys : public IScriptSystemEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IScriptSystemEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusXScriptSys, "EngineModule_DinrusXScriptSys", "d032b164-4978-4f82-a99e-7dc6b6338c5c"_drx_guid)

	virtual ~CEngineModule_DinrusXScriptSys()
	{
		SAFE_DELETE(gEnv->pScriptSystem);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DinrusXScriptSys"; };
	virtual const char* GetCategory() const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;

		CScriptSystem* pScriptSystem = new CScriptSystem;

		bool bStdLibs = true;
		if (!pScriptSystem->Init(pSystem, bStdLibs, 1024))
		{
			pScriptSystem->Release();
			return false;
		}

		env.pScriptSystem = pScriptSystem;
		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusXScriptSys)

#if DRX_PLATFORM_WINDOWS && !defined(_LIB)
HANDLE gDLLHandle = NULL;
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
	gDLLHandle = hModule;
	return TRUE;
}
#endif

#include <DinrusX/CoreX/CrtDebugStats.h>
