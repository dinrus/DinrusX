// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

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
namespace Wwise
{
// Define global objects.
CCVars g_cvars;

#if defined(PROVIDE_WWISE_IMPL_SECONDARY_POOL)
MemoryPoolReferenced g_audioImplMemoryPoolSecondary;
#endif // PROVIDE_AUDIO_IMPL_SECONDARY_POOL

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxAudioImplWwise : public DrxAudio::IImplModule
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(Drx::IDefaultModule)
	DRXINTERFACE_ADD(DrxAudio::IImplModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxAudioImplWwise, "EngineModule_AudioImpl", "b4971e5d-d024-42c5-b34a-9ac0b4abfffd"_drx_guid)

	CEngineModule_DrxAudioImplWwise();

	//////////////////////////////////////////////////////////////////////////
	virtual const char* GetName() const override     { return "DrxAudioImplWwise"; }
	virtual const char* GetCategory() const override { return "DrxAudio"; }

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
#if defined(PROVIDE_WWISE_IMPL_SECONDARY_POOL)
		size_t secondarySize = 0;
		void* pSecondaryMemory = nullptr;

	#if DRX_PLATFORM_DURANGO
		MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Wwise Implementation Audio Pool Secondary");
		secondarySize = g_cvars.m_secondaryMemoryPoolSize << 10;

		APU_ADDRESS temp;
		HRESULT const result = ApuAlloc(&pSecondaryMemory, &temp, secondarySize, SHAPE_XMA_INPUT_BUFFER_ALIGNMENT);
		DRX_ASSERT(result == S_OK);
	#endif  // DRX_PLATFORM_DURANGO

		g_audioImplMemoryPoolSecondary.InitMem(secondarySize, (uint8*)pSecondaryMemory);
#endif    // PROVIDE_AUDIO_IMPL_SECONDARY_POOL

		gEnv->pAudioSystem->AddRequestListener(&CEngineModule_DrxAudioImplWwise::OnAudioEvent, nullptr, ESystemEvents::ImplSet);
		SRequestUserData const data(ERequestFlags::ExecuteBlocking | ERequestFlags::CallbackOnExternalOrCallingThread);
		gEnv->pAudioSystem->SetImpl(new CImpl, data);
		gEnv->pAudioSystem->RemoveRequestListener(&CEngineModule_DrxAudioImplWwise::OnAudioEvent, nullptr);

		if (m_bSuccess)
		{
			Drx::Audio::Log(ELogType::Always, "DrxAudioImplWwise loaded");
		}
		else
		{
			Drx::Audio::Log(ELogType::Error, "DrxAudioImplWwise failed to load");
		}

		return m_bSuccess;
	}

	//////////////////////////////////////////////////////////////////////////
	static void OnAudioEvent(SRequestInfo const* const pAudioRequestInfo)
	{
		m_bSuccess = pAudioRequestInfo->requestResult == ERequestResult::Success;
	}

	static bool m_bSuccess;
};

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxAudioImplWwise)
bool CEngineModule_DrxAudioImplWwise::m_bSuccess = false;

CEngineModule_DrxAudioImplWwise::CEngineModule_DrxAudioImplWwise()
{
	g_cvars.RegisterVariables();
}
} // namespace Wwise
} // namespace Impl
} // namespace DrxAudio
#include <DinrusX/CoreX/CrtDebugStats.h>
