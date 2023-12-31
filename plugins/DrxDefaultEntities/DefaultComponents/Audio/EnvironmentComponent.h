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
struct SEnvironmentSerializeHelper
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(SEnvironmentSerializeHelper const& other) const { return m_name == other.m_name; }

	DrxAudio::EnvironmentId m_id = DrxAudio::InvalidEnvironmentId;
	string                  m_name;
};

class CEnvironmentComponent final : public IEntityComponent
{
protected:

	friend CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void   Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void   ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// Properties exposed to UI
	SEnvironmentSerializeHelper m_environment;
	float                       m_fadeDistance = 0.0f;

public:

	CEnvironmentComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CEnvironmentComponent>& desc);
	void        SetActive(bool const bValue);

private:

	IEntityAudioComponent* m_pIEntityAudioComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<SEnvironmentSerializeHelper>& desc)
{
	desc.SetGUID("AA1DBA00-A1D6-4A77-843C-612A7BE1DECD"_drx_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void SEnvironmentSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioEnvironment<string>(m_name), "environmentName", "^");

	if (archive.isInput())
	{
		m_id = DrxAudio::StringToId(m_name.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
