// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

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
namespace PortAudio
{
// Define global objects.
CCVars g_cvars;

//////////////////////////////////////////////////////////////////////////
class CEngineModule_DrxAudioImplPortAudio : public IImplModule
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(Drx::IDefaultModule)
	DRXINTERFACE_ADD(IImplModule)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CEngineModule_DrxAudioImplPortAudio, "EngineModule_AudioImpl", "c0c05842-ff77-4a61-96de-43a684515dc8"_drx_guid);

	CEngineModule_DrxAudioImplPortAudio();

	//////////////////////////////////////////////////////////////////////////
	virtual char const* GetName()  const override    { return "DrxAudioImplPortAudio"; }
	virtual char const* GetCategory() const override { return "DrxAudio"; }

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		gEnv->pAudioSystem->AddRequestListener(&CEngineModule_DrxAudioImplPortAudio::OnEvent, nullptr, ESystemEvents::ImplSet);
		SRequestUserData const data(ERequestFlags::ExecuteBlocking | ERequestFlags::CallbackOnExternalOrCallingThread);
		gEnv->pAudioSystem->SetImpl(new CImpl, data);
		gEnv->pAudioSystem->RemoveRequestListener(&CEngineModule_DrxAudioImplPortAudio::OnEvent, nullptr);

		if (m_bSuccess)
		{
			Drx::Audio::Log(ELogType::Always, "DrxAudioImplPortAudio loaded");
		}
		else
		{
			Drx::Audio::Log(ELogType::Error, "DrxAudioImplPortAudio failed to load");
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

DRXREGISTER_SINGLETON_CLASS(CEngineModule_DrxAudioImplPortAudio)
bool CEngineModule_DrxAudioImplPortAudio::m_bSuccess = false;

CEngineModule_DrxAudioImplPortAudio::CEngineModule_DrxAudioImplPortAudio()
{
	g_cvars.RegisterVariables();
}
} // namespace PortAudio
} // namespace Impl
} // namespace DrxAudio
#include <DinrusX/CoreX/CrtDebugStats.h>
