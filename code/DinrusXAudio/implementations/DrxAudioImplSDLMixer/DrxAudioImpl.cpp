// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAudio/StdAfx.h>
#include <DinrusX/DinrusXAudio/AudioImpl.h>
#include <DinrusX/DinrusXAudio/AudioImplCVars.h>
#include <Logger.h>
#include <DinrusX/CoreX/Audio/IAudioSystem.h>
#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/IEngineModule.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

namespace DrxAudio
{
namespace Impl
{
namespace SDL_mixer
{
// Define global objects.
CCVars g_cvars;

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxAudioImplSDLMixer : public IImplModule
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(Drx::IDefaultModule)
	DRXINTERFACE_ADD(IImplModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxAudioImplSDLMixer, "EngineModule_AudioImpl", "8030c0d1-905b-4031-a378-5a8b53125f3f"_drx_guid)

	CEngineModule_DrxAudioImplSDLMixer();

	//////////////////////////////////////////////////////////////////////////
	virtual char const* GetName() const override     { return "DrxAudioImplSDLMixer"; }
	virtual char const* GetCategory() const override { return "DrxAudio"; }

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		gEnv->pAudioSystem->AddRequestListener(&CEngineModule_DrxAudioImplSDLMixer::OnEvent, nullptr, ESystemEvents::ImplSet);
		SRequestUserData const data(ERequestFlags::ExecuteBlocking | ERequestFlags::CallbackOnExternalOrCallingThread);
		gEnv->pAudioSystem->SetImpl(new CImpl, data);
		gEnv->pAudioSystem->RemoveRequestListener(&CEngineModule_DrxAudioImplSDLMixer::OnEvent, nullptr);

		if (m_bSuccess)
		{
			Drx::Audio::Log(ELogType::Always, "DrxAudioImplSDLMixer loaded");
		}
		else
		{
			Drx::Audio::Log(ELogType::Error, "DrxAudioImplSDLMixer failed to load");
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

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxAudioImplSDLMixer)
bool CEngineModule_DrxAudioImplSDLMixer::m_bSuccess = false;

CEngineModule_DrxAudioImplSDLMixer::CEngineModule_DrxAudioImplSDLMixer()
{
	g_cvars.RegisterVariables();
}
} // namespace SDL_mixer
} // namespace Impl
} // namespace DrxAudio
#include <DinrusX/CoreX/CrtDebugStats.h>
