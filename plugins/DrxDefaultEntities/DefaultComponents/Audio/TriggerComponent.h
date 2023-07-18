// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Audio/IAudioInterfacesCommonData.h>
<DinrusX/CoreX/Serialization/Forward.h>
<DinrusX/DinrusXSchema/TypeDesc.h>
<DinrusX/DinrusXSchema/IEnvRegistrar.h>
<DinrusX/CoreX/String/DrxString.h>

class CPlugin_DrxDefaultEntities;

namespace Drx
{
namespace Audio
{
namespace DefaultComponents
{
struct STriggerSerializeHelper
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(STriggerSerializeHelper const& other) const { return m_name == other.m_name; }

	DrxAudio::ControlId m_id = DrxAudio::InvalidControlId;
	string              m_name;
};

class CTriggerComponent final : public IEntityComponent
{
protected:

	friend CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void                    Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void                    ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

public:

	CTriggerComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CTriggerComponent>& desc);

	void        Play();
	void        Stop();
	void        DetermineActivityRadius();
	void        GetActivityRadius(float& radius);

	struct SFinishedSignal
	{
		SFinishedSignal() = default;
	};

protected:

	DrxAudio::AuxObjectId  m_auxObjectId = DrxAudio::InvalidAuxObjectId;
	IEntityAudioComponent* m_pIEntityAudioComponent = nullptr;
	bool                   m_autoPlay = true;
	uint32                 m_numActiveTriggerInstances = 0;
	float                  m_activityRadius = 0.0f;

	// Properties exposed to UI
	STriggerSerializeHelper m_playTrigger;
	STriggerSerializeHelper m_stopTrigger;

#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
	Serialization::FunctorActionButton<std::function<void()>> m_playButton;
	Serialization::FunctorActionButton<std::function<void()>> m_stopButton;
	DrxAudio::ControlId m_previousPlayTriggerId = DrxAudio::InvalidControlId;
	DrxAudio::ControlId m_previousStopTriggerId = DrxAudio::InvalidControlId;
#endif  // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<STriggerSerializeHelper>& desc)
{
	desc.SetGUID("C5DE4974-ECAB-4D6F-A93D-02C1F5C55C31"_drx_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void STriggerSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioTrigger<string>(m_name), "triggerName", "^");

	if (archive.isInput())
	{
		m_id = DrxAudio::StringToId(m_name.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
