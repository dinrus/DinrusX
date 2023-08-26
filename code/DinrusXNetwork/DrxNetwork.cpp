// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
//	File: drxnetwork.cpp
//  Описание: dll entry point
//
//	History:
//	-July 25,2001:Created by Alberto Demichelis
//
//////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/Network.h>
// Included only once per DLL module.
#include <DinrusX/CoreX/Platform/platform_impl.inl>

#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

//////////////////////////////////////////////////////////////////////////

DRXNETWORK_API INetwork* CreateNetwork(ISystem* pSystem, int ncpu)
{
	CNetwork* pNetwork = new CNetwork;

	if (!pNetwork->Init(ncpu))
	{
		pNetwork->Release();
		return NULL;
	}
	return pNetwork;
}

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DinrusXNetwork : public INetworkEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(INetworkEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusXNetwork, "EngineModule_DinrusXNetwork", "7dc5c3b8-bb37-4063-a29a-c2d6dd718e0f"_drx_guid)

	virtual ~CEngineModule_DinrusXNetwork()
	{
		SAFE_RELEASE(gEnv->pNetwork);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override { return "DinrusXNetwork"; };
	virtual const char* GetCategory()  const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;

		CNetwork* pNetwork = new CNetwork;

		int ncpu = env.pi.numCoresAvailableToProcess;
		if (!pNetwork->Init(ncpu))
		{
			pNetwork->Release();
			return false;
		}
		env.pNetwork = pNetwork;

		DrxLogAlways("[Network Version]: "
#if defined(_RELEASE)
		             "RELEASE "
#elif defined(_PROFILE)
		             "PROFILE "
#else
		             "DEBUG "
#endif

#if defined(PURE_CLIENT)
		             "PURE CLIENT"
#elif (DEDICATED_SERVER)
		             "DEDICATED SERVER"
#else
		             "DEVELOPMENT BUILD"
#endif
		             );

#if defined(DEDICATED_SERVER) && defined(PURE_CLIENT)
		DrxFatalError("[Network]: Invalid build configuration - aborting");
#endif

		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusXNetwork);

#include <DinrusX/CoreX/CrtDebugStats.h>
