// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAnimation/stdafx.h>
#include <DinrusX/DinrusXAnimation/AnimationBase.h>

#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXAnimation/IFacialAnimation.h>
#include <DinrusX/DinrusXAnimation/CharacterUpr.h>
#include <DinrusX/DinrusXAnimation/AnimEventLoader.h>
#include <DinrusX/DinrusXAnimation/SkeletonAnim.h>
#include <DinrusX/DinrusXAnimation/AttachmentSkin.h>

#include <DinrusX/CoreX/Platform/platform_impl.inl> // Must be included once in the module.
#include <DinrusX/DinrusXSys/IEngineModule.h>

//////////////////////////////////////////////////////////////////////////
struct CSystemEventListener_Animation : public ISystemEventListener
{
public:
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
	{
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
			break;
		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			{
				if (Memory::CPoolFrameLocal* pMemoryPool = CharacterInstanceProcessing::GetMemoryPool())
				{
					pMemoryPool->ReleaseBuckets();
				}

				if (!gEnv->IsEditor())
				{
					delete g_pCharacterUpr;
					g_pCharacterUpr = NULL;
					gEnv->pCharacterUpr = NULL;
				}

				break;
			}
		case ESYSTEM_EVENT_3D_POST_RENDERING_START:
			{
				if (!g_pCharacterUpr)
				{
					g_pCharacterUpr = new CharacterUpr;
					gEnv->pCharacterUpr = g_pCharacterUpr;
				}
				AnimEventLoader::SetPreLoadParticleEffects(false);
				break;
			}
		case ESYSTEM_EVENT_3D_POST_RENDERING_END:
			{
				if (Memory::CPoolFrameLocal* pMemoryPool = CharacterInstanceProcessing::GetMemoryPool())
				{
					pMemoryPool->ReleaseBuckets();
				}

				SAFE_DELETE(g_pCharacterUpr);
				gEnv->pCharacterUpr = NULL;
				AnimEventLoader::SetPreLoadParticleEffects(true);
				break;
			}
		case ESYSTEM_EVENT_LEVEL_LOAD_START:
			{
				if (!g_pCharacterUpr)
				{
					g_pCharacterUpr = new CharacterUpr;
					gEnv->pCharacterUpr = g_pCharacterUpr;
					gEnv->pCharacterUpr->PostInit();
				}
				break;
			}
		}
	}
};
static CSystemEventListener_Animation g_system_event_listener_anim;

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DinrusXAnimation : public IAnimationEngineModule
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(Drx::IDefaultModule)
		DRXINTERFACE_ADD(IAnimationEngineModule)
	DRXINTERFACE_END()
	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DinrusXAnimation, "EngineModule_DinrusXAnimation", "9c73d2cd-142c-4256-a8f0-706d80cd7ad2"_drx_guid)

	virtual ~CEngineModule_DinrusXAnimation()
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(&g_system_event_listener_anim);
		SAFE_RELEASE(gEnv->pCharacterUpr);
	}

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName()  const override { return "DinrusXAnimation"; };
	virtual const char* GetCategory()  const override { return "DinrusX"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		ISystem* pSystem = env.pSystem;

		if (!CharacterInstanceProcessing::InitializeMemoryPool())
			return false;

		g_pISystem = pSystem;
		g_InitInterfaces();

		if (!g_controllerHeap.IsInitialised())
			g_controllerHeap.Init(Console::GetInst().ca_MemoryDefragPoolSize);

		pSystem->GetISystemEventDispatcher()->RegisterListener(&g_system_event_listener_anim, "CSystemEventListener_Animation");

		g_pCharacterUpr = NULL;
		env.pCharacterUpr = NULL;
		if (gEnv->IsEditor())
		{
			g_pCharacterUpr = new CharacterUpr;
			gEnv->pCharacterUpr = g_pCharacterUpr;
		}

		return true;
	}
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DinrusXAnimation)

// cached interfaces - valid during the whole session, when the character manager is alive; then get erased
ISystem* g_pISystem = NULL;
ITimer* g_pITimer = NULL;                     //module implemented in DinrusSystem
ILog* g_pILog = NULL;                         //module implemented in DinrusSystem
IConsole* g_pIConsole = NULL;                 //module implemented in DinrusSystem
IDrxPak* g_pIPak = NULL;                      //module implemented in DinrusSystem
IStreamEngine* g_pIStreamEngine = NULL;       //module implemented in DinrusSystem

IRenderer* g_pIRenderer = NULL;
IPhysicalWorld* g_pIPhysicalWorld = NULL;
I3DEngine* g_pI3DEngine = NULL;               //Need just for loading of chunks. Should be part of DinrusSystem

f32 g_AverageFrameTime = 0;
CAnimation g_DefaultAnim;
CharacterUpr* g_pCharacterUpr;
QuatT g_IdentityQuatT = QuatT(IDENTITY);
int32 g_nRenderThreadUpdateID = 0;
DynArray<string> g_DataMismatch;
SParametricSamplerInternal* g_parametricPool = NULL;
bool* g_usedParametrics = NULL;
int32 g_totalParametrics = 0;
uint32 g_DefaultTransitionInterpolationType = (uint32)CA_Interpolation_Type::QuadraticInOut;
AABB g_IdentityAABB = AABB(ZERO, ZERO);

CControllerDefragHeap g_controllerHeap;

ILINE void g_LogToFile(const char* szFormat, ...)
{
	char szBuffer[2 * 1024];
	va_list args;
	va_start(args, szFormat);
	drx_vsprintf(szBuffer, szFormat, args);
	va_end(args);
	g_pILog->LogToFile("%s", szBuffer);
}

f32 g_fCurrTime = 0;
bool g_bProfilerOn = false;

AnimStatisticsInfo g_AnimStatisticsInfo;

// TypeInfo implementations for DinrusXAnimation
#ifndef _LIB
	#include <DinrusX/CoreX/Common_TypeInfo.h>
	#include <DinrusX/DinrusX3dEng/CGFContent_info.h>
#endif
