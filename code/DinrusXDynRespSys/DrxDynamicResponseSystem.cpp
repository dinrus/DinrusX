// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXDynRespSys/stdafx.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Memory/BucketAllocatorImpl.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

#include <DinrusX/DinrusXDynRespSys/ResponseSystem.h>
#include <DinrusX/DinrusXDynRespSys/VariableCollection.h>

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DinrusDynRespSys : public DRS::IDynamicResponseSystemEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(DRS::IDynamicResponseSystemEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusDynRespSys, "EngineModule_DinrusDynRespSys", "d1ed34dd-a44c-4c17-959a-46df79af5db3"_drx_guid)

	virtual ~CEngineModule_DinrusDynRespSys() override
	{
		SAFE_DELETE(gEnv->pDynamicResponseSystem);
	}

	virtual const char* GetName() const override { return "DinrusDynRespSys"; }
	virtual const char* GetCategory() const override { return "DinrusX"; }

	virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		DrxDRS::CResponseSystem* pResponseSystem = new DrxDRS::CResponseSystem();
		env.pDynamicResponseSystem = pResponseSystem;

		pResponseSystem->CreateVariableCollection(DrxDRS::CVariableCollection::s_globalCollectionName);

		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusDynRespSys)

#include <DinrusX/CoreX/CrtDebugStats.h>
