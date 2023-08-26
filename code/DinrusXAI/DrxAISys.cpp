// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// DinrusXAISys.cpp : Определяет точку входа в приложение DLL.
//

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/DrxAISys.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXAI/CAISystem.h>
#include <DinrusX/DinrusXAI/AILog.h>
#include <DinrusX/DinrusXSys/ISystem.h>

#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

CAISystem* g_pAISystem;

/*
   //////////////////////////////////////////////////////////////////////////
   // Pointer to Global ISystem.
   static ISystem* gISystem = 0;
   ISystem* GetISystem()
   {
   return gISystem;
   }
 */

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DinrusXAISys : public IAIEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IAIEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusXAISys, "EngineModule_DinrusXAI", "6b8e79a7-8400-4f44-97db-7614428ad251"_drx_guid)

	virtual ~CEngineModule_DinrusXAISys()
	{
		DrxUnregisterFlowNodes();
		SAFE_RELEASE(gEnv->pAISystem);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName()  const override { return "DinrusXAI"; };
	virtual const char* GetCategory()  const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;

		AIInitLog(pSystem);

		g_pAISystem = new CAISystem(pSystem);
		env.pAISystem = g_pAISystem;

		DrxRegisterFlowNodes();

		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusXAISys)

//////////////////////////////////////////////////////////////////////////
#include <DinrusX/CoreX/CrtDebugStats.h>

#ifndef _LIB
	#include <DinrusX/CoreX/Common_TypeInfo.h>
#endif

#include <DinrusX/CoreX/TypeInfo_impl.h>
#include <DinrusX/DinrusXAI/AutoTypeStructs_info.h>
