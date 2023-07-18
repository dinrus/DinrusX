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
struct SParameterSerializeHelper
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(SParameterSerializeHelper const& other) const { return m_name == other.m_name; }

	DrxAudio::ControlId m_id = DrxAudio::InvalidControlId;
	string              m_name;
};

class CParameterComponent final : public IEntityComponent
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

	CParameterComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CParameterComponent>& desc);

	void        Set(float const value);

protected:

	IEntityAudioComponent* m_pIEntityAudioComponent = nullptr;

	// Properties exposed to UI
	SParameterSerializeHelper                        m_parameter;
	Schematyc::Range<-100000, 100000, -10000, 10000> m_value = 0.0f;
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<SParameterSerializeHelper>& desc)
{
	desc.SetGUID("5287D8F9-7638-41BB-BFDD-2F5B47DEEA07"_drx_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void SParameterSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioRTPC<string>(m_name), "parameterName", "^");

	if (archive.isInput())
	{
		m_id = DrxAudio::StringToId(m_name.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
