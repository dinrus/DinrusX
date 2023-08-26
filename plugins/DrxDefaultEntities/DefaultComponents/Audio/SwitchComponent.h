// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

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
struct SSwitchWithStateSerializeHelper
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(SSwitchWithStateSerializeHelper const& other) const { return m_switchName == other.m_switchName && m_switchStateName == other.m_switchStateName; }

	DrxAudio::ControlId     m_switchId = DrxAudio::InvalidControlId;
	DrxAudio::SwitchStateId m_switchStateId = DrxAudio::InvalidSwitchStateId;
	string                  m_switchName;
	string                  m_switchStateName;
};

class CSwitchComponent final : public IEntityComponent
{
protected:

	friend CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void   Initialize() override;
	virtual void   OnShutDown() override {}
	virtual uint64 GetEventMask() const override;
	virtual void   ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

public:

	CSwitchComponent() = default;

	static void     ReflectType(Schematyc::CTypeDesc<CSwitchComponent>& desc);

	void Set(SSwitchWithStateSerializeHelper const& switchAndState);

protected:

	IEntityAudioComponent* m_pIEntityAudioComponent = nullptr;

	// Properties exposed to UI
	SSwitchWithStateSerializeHelper m_switch;
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<SSwitchWithStateSerializeHelper>& desc)
{
	desc.SetGUID("9DB56B33-57FE-4E97-BED2-F0BBD3012967"_drx_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void SSwitchWithStateSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioSwitch<string>(m_switchName), "switchName", "SwitchName");
	archive(Serialization::AudioSwitchState<string>(m_switchStateName), "stateName", "StateName");

	if (archive.isInput())
	{
		m_switchId = DrxAudio::StringToId(m_switchName.c_str());
		m_switchStateId = DrxAudio::StringToId(m_switchStateName.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
