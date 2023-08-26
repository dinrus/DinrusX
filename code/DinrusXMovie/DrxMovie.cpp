// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXMovie/StdAfx.h>
#include <DinrusX/DinrusXMovie/DrxMovie.h>
#include <DinrusX/DinrusXMovie/Movie.h>
#include <DinrusX/CoreX/CrtDebugStats.h>

// Included only once per DLL module.
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/IDrxFactory.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

#undef GetClassName

struct CSystemEventListener_Movie : public ISystemEventListener
{
public:
	virtual ~CSystemEventListener_Movie()
	{
		if (gEnv->pSystem)
		{
			gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
		}
	}

	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
	{
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			{
				CLightAnimWrapper::ReconstructCache();
				break;
			}
		}
	}
};

static CSystemEventListener_Movie g_system_event_listener_movie;

class CEngineModule_DinrusXMovie : public IMovieEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IMovieEngineModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusXMovie, "EngineModule_DinrusXMovie", "dce26bee-bdc6-400f-a0e9-b42839f2dd5b"_drx_guid)

	virtual ~CEngineModule_DinrusXMovie()
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(&g_system_event_listener_movie);
		SAFE_RELEASE(gEnv->pMovieSystem);
	}

	virtual const char* GetName() const override { return "DinrusXMovie"; };
	virtual const char* GetCategory() const override { return "DinrusX"; };

	virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;
		pSystem->GetISystemEventDispatcher()->RegisterListener(&g_system_event_listener_movie, "CEngineModule_DinrusXMovie");

		env.pMovieSystem = new CMovieSystem(pSystem);
		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusXMovie)