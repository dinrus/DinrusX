// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Audio/IAudioInterfacesCommonData.h>
#include <DinrusX/CoreX/Audio/IListener.h>
#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/IEnvRegistrar.h>

class CPlugin_DrxDefaultEntities;

namespace Drx
{
namespace Audio
{
namespace DefaultComponents
{
class CListenerComponent final : public IEntityComponent
{
protected:

	friend class CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void   Initialize() override;
	virtual void   OnShutDown() override;
	virtual uint64 GetEventMask() const override;
	virtual void   ProcessEvent(const SEntityEvent& event) override;
	virtual void   OnTransformChanged() override;
	// ~IEntityComponent

public:

	CListenerComponent() = default;

	inline static void ReflectType(Schematyc::CTypeDesc<CListenerComponent>& desc)
	{
		desc.SetGUID("BAE91D5C-8CFB-40B2-8397-F5A9EEDB9AC4"_drx_guid);
		desc.SetEditorCategory("Audio");
		desc.SetLabel("Listener");
		desc.SetDescription("Audio Listener from which transformation the audio is recorded.");
		desc.SetIcon("icons:Audio/component_audio_listener.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });
	}

	inline void SetActive(bool const bValue)
	{
		m_bActive = bValue;

		if (!m_bActive)
		{
			gEnv->pEntitySystem->GetAreaUpr()->ExitAllAreas(GetEntityId());
		}
	}

private:

	DrxAudio::IListener*            m_pIListener = nullptr;
	DrxAudio::CObjectTransformation m_previousTransformation;
	bool                            m_bActive = true;
};
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
