// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAudio/StdAfx.h>
#include <DinrusX/DinrusXAudio/AudioImpl.h>
#include <DinrusX/DinrusXAudio/AudioImplCVars.h>
#include <Logger.h>
#include <DinrusX/CoreX/Audio/IAudioSystem.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

#if DRX_PLATFORM_DURANGO
	#include <apu.h>
	#include <shapexmacontext.h>
#endif // DRX_PLATFORM_DURANGO

namespace DrxAudio
{
namespace Impl
{
namespace Fmod
{
// Define global objects.
CCVars g_cvars;

#if defined(PROVIDE_FMOD_IMPL_SECONDARY_POOL)
MemoryPoolReferenced g_audioImplMemoryPoolSecondary;
#endif // PROVIDE_AUDIO_IMPL_SECONDARY_POOL

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxAudioImplFmod : public DrxAudio::IImplModule
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(Drx::IDefaultModule)
	DRXINTERFACE_ADD(DrxAudio::IImplModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxAudioImplFmod, "EngineModule_AudioImpl", "aa6a039a-0ce5-bbab-33e0-aad69f3136f4"_drx_guid);

	CEngineModule_DrxAudioImplFmod();

	//////////////////////////////////////////////////////////////////////////
	virtual char const* GetName()  const override     { return "DrxAudioImplFmod"; }
	virtual char const* GetCategory()  const override { return "DrxAudio"; }

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
#if defined(PROVIDE_FMOD_IMPL_SECONDARY_POOL)
		size_t secondarySize = 0;
		void* pSecondaryMemory = nullptr;

	#if DRX_PLATFORM_DURANGO
		MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Fmod Implementation Audio Pool Secondary");
		secondarySize = g_cvars.m_secondaryMemoryPoolSize << 10;

		APU_ADDRESS temp;
		HRESULT const result = ApuAlloc(&pSecondaryMemory, &temp, secondarySize, SHAPE_XMA_INPUT_BUFFER_ALIGNMENT);
		DRX_ASSERT(result == S_OK);
	#endif  // DRX_PLATFORM_DURANGO

		g_audioImplMemoryPoolSecondary.InitMem(secondarySize, (uint8*)pSecondaryMemory);
#endif    // PROVIDE_AUDIO_IMPL_SECONDARY_POOL

		gEnv->pAudioSystem->AddRequestListener(&CEngineModule_DrxAudioImplFmod::OnEvent, nullptr, ESystemEvents::ImplSet);
		SRequestUserData const data(ERequestFlags::ExecuteBlocking | ERequestFlags::CallbackOnExternalOrCallingThread);
		gEnv->pAudioSystem->SetImpl(new CImpl, data);
		gEnv->pAudioSystem->RemoveRequestListener(&CEngineModule_DrxAudioImplFmod::OnEvent, nullptr);

		if (m_bSuccess)
		{
			Drx::Audio::Log(ELogType::Always, "DrxAudioImplFmod loaded");
		}
		else
		{
			Drx::Audio::Log(ELogType::Error, "DrxAudioImplFmod failed to load");
		}

		return m_bSuccess;
	}

	//////////////////////////////////////////////////////////////////////////
	static void OnEvent(SRequestInfo const* const pRequestInfo)
	{
		m_bSuccess = pRequestInfo->requestResult == ERequestResult::Success;
	}

	static bool m_bSuccess;
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxAudioImplFmod)
bool CEngineModule_DrxAudioImplFmod::m_bSuccess = false;

CEngineModule_DrxAudioImplFmod::CEngineModule_DrxAudioImplFmod()
{
	g_cvars.RegisterVariables();
}
} // namespace Fmod
} // namespace Impl
} // namespace DrxAudio
#include <DinrusX/CoreX/CrtDebugStats.h>
