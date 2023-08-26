// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>

class CEngineModule_DrxAction : public IGameFrameworkEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IGameFrameworkEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxAction, "EngineModule_DinrusXAction", "943C708C-F469-48AA-B573-D6A0FCE7B9CD"_drx_guid)

	virtual ~CEngineModule_DrxAction()
	{
		SAFE_DELETE(gEnv->pGameFramework);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DinrusAction"; }
	virtual const char* GetCategory() const override { return "DinrusX"; }

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		// Special case: cast away the const to maintain legacy compatibility for the game framework
		env.pGameFramework = new CDrxAction(const_cast<SSystemInitParams&>(initParams));
		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxAction)