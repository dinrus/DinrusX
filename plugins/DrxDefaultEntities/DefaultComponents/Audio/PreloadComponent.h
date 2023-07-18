// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Audio/IAudioInterfacesCommonData.h>
<DinrusX/CoreX/Serialization/Forward.h>
<DinrusX/DinrusXSchema/TypeDesc.h>
<DinrusX/DinrusXSchema/IEnvRegistrar.h>

class CPlugin_DrxDefaultEntities;

namespace Drx
{
namespace Audio
{
namespace DefaultComponents
{
struct SPreloadSerializeHelper
{
	void Serialize(Serialization::IArchive& archive);
	bool operator==(SPreloadSerializeHelper const& other) const { return m_name == other.m_name; }

	DrxAudio::PreloadRequestId m_id = DrxAudio::InvalidPreloadRequestId;
	string                     m_name;
};

class CPreloadComponent final : public IEntityComponent
{
protected:

	friend CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void   Initialize() override                      {}
	virtual void   OnShutDown() override;
	virtual uint64 GetEventMask() const override              { return 0; }
	virtual void   ProcessEvent(const SEntityEvent& event) override {}
	// ~IEntityComponent

	// Properties exposed to UI
	SPreloadSerializeHelper m_preload;

public:

	CPreloadComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CPreloadComponent>& desc);

	void        Load();
	void        Unload();

private:

	bool m_bLoaded = false;
};

//////////////////////////////////////////////////////////////////////////
inline void ReflectType(Schematyc::CTypeDesc<SPreloadSerializeHelper>& desc)
{
	desc.SetGUID("C9353DDE-5F53-482F-AFEF-7D6A500ACDD9"_drx_guid);
}

//////////////////////////////////////////////////////////////////////////
inline void SPreloadSerializeHelper::Serialize(Serialization::IArchive& archive)
{
	archive(Serialization::AudioPreloadRequest<string>(m_name), "preloadName", "^");

	if (archive.isInput())
	{
		m_id = DrxAudio::StringToId(m_name.c_str());
	}
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
