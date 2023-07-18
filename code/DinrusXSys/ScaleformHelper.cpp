// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#ifdef INCLUDE_SCALEFORM_SDK
	#include "ScaleformHelper.h"

// Engine module wrapper for Scaleform Helper.
// Note: This can either be loaded from inside DinrusSystem (if compiled in directly) or loaded from a DLL (if pre-compiled version is shipped).
// The only responsibility of the module is to set gEnv->pScaleformHelper, through which all DRXENGINE specific Scaleform interaction runs.
	#include <DinrusX/DinrusXSys/IEngineModule.h>
	#include <DinrusX/CoreX/Extension/ClassWeaver.h>

class CEngineModule_ScaleformHelper : public IScaleformHelperEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IScaleformHelperEngineModule)
	DRXINTERFACE_END()
	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_ScaleformHelper, "EngineModule_ScaleformHelper", "3d38f12a-521d-43cf-ca18-fd1fa7ea5020"_drx_guid)

	virtual ~CEngineModule_ScaleformHelper() {}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DrxScaleformHelper"; };

	#if DRX_IS_SCALEFORM_HELPER
	virtual const char* GetCategory() const override { return "DrxExtensions"; };
	#else
	virtual const char* GetCategory() const override { return "DinrusX"; };
	#endif

	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		if (env.pRenderer)
		{
			env.pScaleformHelper = new CScaleformHelper();	
		}

		return true;
	}
};
DRXREGISTER_SINGLETON_CLASS(CEngineModule_ScaleformHelper)
#endif

// If we are a stand-alone module, this will implement the module entry-points
#ifdef DRX_IS_SCALEFORM_HELPER
	#include <DinrusX/CoreX/Platform/platform_impl.inl>
#endif
