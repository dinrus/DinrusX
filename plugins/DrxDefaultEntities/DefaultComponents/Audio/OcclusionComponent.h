// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Audio/IAudioSystem.h>
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
class COcclusionComponent final : public IEntityComponent
{
protected:

	friend CPlugin_DrxDefaultEntities;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void   Initialize() override;
	virtual void   OnShutDown() override;
	virtual uint64 GetEventMask() const override;
	virtual void   ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

public:

	DrxAudio::EOcclusionType m_occlusionType = DrxAudio::EOcclusionType::None;

public:

	COcclusionComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<COcclusionComponent>& desc);
	void        SetOcclusionType(DrxAudio::EOcclusionType const occlusionType);

private:

	IEntityAudioComponent* m_pIEntityAudioComponent = nullptr;
};
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
