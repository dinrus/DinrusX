// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXFont/StdAfx.h>
// Included only once per DLL module.
#include <DinrusX/CoreX/Platform/platform_impl.inl>

#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

#include <DinrusX/DinrusXFont/DinrusXFont.h>
#if defined(USE_NULLFONT)
	#include <DinrusX/DinrusXFont/NullFont.h>
#endif

///////////////////////////////////////////////
extern "C" IDrxFont * CreateDrxFontInterface(ISystem * pSystem)
{
	if (gEnv->IsDedicated())
	{
#if defined(USE_NULLFONT)
		return new CDrxNullFont();
#else
		// The NULL font implementation must be present for all platforms
		// supporting running as a pure dedicated server.
		pSystem->GetILog()->LogError("Missing NULL font implementation for dedicated server");
		return NULL;
#endif
	}
	else
	{
#if defined(USE_NULLFONT) && defined(USE_NULLFONT_ALWAYS)
		return new CDrxNullFont();
#else
		return new CDrxFont(pSystem);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxFont : public IFontEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IFontEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxFont, "EngineModule_DrxFont", "6758643f-4321-4957-9b92-0d898d31f434"_drx_guid)

	virtual ~CEngineModule_DrxFont()
	{
		SAFE_RELEASE(gEnv->pDrxFont);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DinrusXFont"; };
	virtual const char* GetCategory() const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;
		env.pDrxFont = CreateDrxFontInterface(pSystem);
		return env.pDrxFont != 0;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxFont)