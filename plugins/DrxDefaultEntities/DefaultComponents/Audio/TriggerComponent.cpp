// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "TriggerComponent.h"
<DinrusX/CoreX/Audio/IAudioSystem.h>

namespace Drx
{
namespace Audio
{
namespace DefaultComponents
{
//////////////////////////////////////////////////////////////////////////
static void ReflectType(Schematyc::CTypeDesc<CTriggerComponent::SFinishedSignal>& desc)
{
	desc.SetGUID("A16A29CB-8E39-42C0-88C2-33FED1680545"_drx_guid);
	desc.SetLabel("Finished");
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTriggerComponent::Play, "B7FDCC03-6312-4795-8D00-D63F3381BFBC"_drx_guid, "Play");
		pFunction->SetDescription("Executes the PlayTrigger");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTriggerComponent::Stop, "11AB550A-A6C9-422E-8D87-8333E845C23D"_drx_guid, "Stop");
		pFunction->SetDescription("Stops the PlayTrigger if no StopTrigger assigned otherwise executes the StopTrigger.");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTriggerComponent::GetActivityRadius, "F70A0C3D-50EC-4858-A470-2855FFEC25E2"_drx_guid, "GetActivityRadius");
		pFunction->SetDescription("Retrieves the activity radius of this component.");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		pFunction->BindOutput(1, 'radi', "Radius");
		componentScope.Register(pFunction);
	}

	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CTriggerComponent::SFinishedSignal));
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::ReflectType(Schematyc::CTypeDesc<CTriggerComponent>& desc)
{
	desc.SetGUID("672F0641-004E-4300-B4F7-764B70CC4DA0"_drx_guid);
	desc.SetEditorCategory("Audio");
	desc.SetLabel("Trigger");
	desc.SetDescription("Allows for execution of an audio trigger at provided transformation.");
	desc.SetIcon("icons:Audio/component_trigger.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });

	desc.AddMember(&CTriggerComponent::m_autoPlay, 'auto', "autoPlay", "AutoPlay", "Automatically executes PlayTrigger when enabled.", true);

	desc.AddMember(&CTriggerComponent::m_playTrigger, 'tri1', "playTrigger", "PlayTrigger", "This trigger gets executed when Play is called.", STriggerSerializeHelper());
	desc.AddMember(&CTriggerComponent::m_stopTrigger, 'tri2', "stopTrigger", "StopTrigger", "This trigger gets executed when Stop is called.", STriggerSerializeHelper());

#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
	desc.AddMember(&CTriggerComponent::m_playButton, 'btn1', "play", "Play", "Executes the PlayTrigger.", Serialization::FunctorActionButton<std::function<void()>>());
	desc.AddMember(&CTriggerComponent::m_stopButton, 'btn2', "stop", "Stop", "Stops the PlayTrigger if no StopTrigger assigned otherwise executes the StopTrigger.", Serialization::FunctorActionButton<std::function<void()>>());
#endif  // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::Initialize()
{
	m_pIEntityAudioComponent = m_pEntity->GetComponent<IEntityAudioComponent>();

	if (m_pIEntityAudioComponent != nullptr)
	{
		if (m_auxObjectId != DrxAudio::InvalidAuxObjectId && m_auxObjectId != DrxAudio::DefaultAuxObjectId)
		{
			// Workaround for scenarios where 'Initialize' is called twice without a 'OnShutDown' in between.
			m_pIEntityAudioComponent->RemoveAudioAuxObject(m_auxObjectId);
		}

		m_auxObjectId = m_pIEntityAudioComponent->CreateAudioAuxObject();
	}
	else
	{
		m_pIEntityAudioComponent = m_pEntity->CreateComponent<IEntityAudioComponent>();
		DRX_ASSERT(m_auxObjectId == DrxAudio::InvalidAuxObjectId);
		m_auxObjectId = DrxAudio::DefaultAuxObjectId;
	}

	// Only play in editor. Launcher is handled via ENTITY_EVENT_START_GAME.
	if (m_autoPlay && gEnv->IsEditor())
	{
		Play();
	}

	DetermineActivityRadius();

#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
	m_playButton = Serialization::ActionButton(std::function<void()>([this]() { Play(); }));
	m_stopButton = Serialization::ActionButton(std::function<void()>([this]() { Stop(); }));

	m_previousPlayTriggerId = m_playTrigger.m_id;
	m_previousStopTriggerId = m_stopTrigger.m_id;
#endif  // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
}

//////////////////////////////////////////////////////////////////////////
uint64 CTriggerComponent::GetEventMask() const
{
	uint64 mask = ENTITY_EVENT_BIT(ENTITY_EVENT_AUDIO_TRIGGER_STARTED) | ENTITY_EVENT_BIT(ENTITY_EVENT_AUDIO_TRIGGER_ENDED) | ENTITY_EVENT_BIT(ENTITY_EVENT_START_GAME) | ENTITY_EVENT_BIT(ENTITY_EVENT_DONE);

#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
	mask |= ENTITY_EVENT_BIT(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED) | ENTITY_EVENT_BIT(ENTITY_EVENT_RESET);
#endif  // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE

	return mask;
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::ProcessEvent(const SEntityEvent& event)
{
	if (m_pIEntityAudioComponent != nullptr)
	{
		switch (event.event)
		{
		case ENTITY_EVENT_AUDIO_TRIGGER_STARTED:
			++m_numActiveTriggerInstances;
			break;
		case ENTITY_EVENT_AUDIO_TRIGGER_ENDED:
			DRX_ASSERT(m_numActiveTriggerInstances > 0);
			--m_numActiveTriggerInstances;

			if (m_autoPlay && m_numActiveTriggerInstances == 0)
			{
				DrxAudio::SRequestInfo const* const pRequestInfo = reinterpret_cast<DrxAudio::SRequestInfo const* const>(event.nParam[0]);

				if (m_pIEntityAudioComponent->GetAuxObjectIdFromAudioObject(pRequestInfo->pAudioObject) == m_auxObjectId)
				{
					Schematyc::IObject* const pSchematycObject = GetEntity()->GetSchematycObject();

					if (pSchematycObject != nullptr)
					{
						pSchematycObject->ProcessSignal(SFinishedSignal(), GetGUID());
					}
				}
			}
			break;
		case ENTITY_EVENT_START_GAME:
			// Only play in launcher. Editor is handled in Initialize()
			if (m_autoPlay && !gEnv->IsEditor())
			{
				Play();
			}
			break;
		case ENTITY_EVENT_DONE:
			if (m_autoPlay)
			{
				Stop();
			}

			if (m_pIEntityAudioComponent != nullptr && m_auxObjectId != DrxAudio::InvalidAuxObjectId && m_auxObjectId != DrxAudio::DefaultAuxObjectId)
			{
				m_pIEntityAudioComponent->RemoveAudioAuxObject(m_auxObjectId);
			}

			m_auxObjectId = DrxAudio::InvalidAuxObjectId;
			break;
#if defined(INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE)
		case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
			{
				if (m_previousPlayTriggerId != m_playTrigger.m_id)
				{
					if (m_previousPlayTriggerId != DrxAudio::InvalidControlId)
					{
						m_pIEntityAudioComponent->StopTrigger(m_previousPlayTriggerId, m_auxObjectId);
					}

					if (m_autoPlay)
					{
						Play();
					}

					DetermineActivityRadius();
				}
				else if (m_previousStopTriggerId != m_stopTrigger.m_id)
				{
					if (m_previousStopTriggerId != DrxAudio::InvalidControlId)
					{
						m_pIEntityAudioComponent->StopTrigger(m_previousStopTriggerId, m_auxObjectId);
					}

					if (m_autoPlay)
					{
						Stop();
					}

					DetermineActivityRadius();
				}

				m_previousPlayTriggerId = m_playTrigger.m_id;
				m_previousStopTriggerId = m_stopTrigger.m_id;
			}
			break;
		case ENTITY_EVENT_RESET:
			{
				if (m_autoPlay && (m_numActiveTriggerInstances == 0))
				{
					Play();
				}
				else if (!m_autoPlay && (m_numActiveTriggerInstances > 0))
				{
					Stop();
				}
			}
			break;
#endif      // INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::Play()
{
	if (m_pIEntityAudioComponent != nullptr && m_playTrigger.m_id != DrxAudio::InvalidControlId)
	{
		DrxAudio::SRequestUserData const userData(DrxAudio::ERequestFlags::DoneCallbackOnExternalThread | DrxAudio::ERequestFlags::CallbackOnExternalOrCallingThread, this);
		m_pIEntityAudioComponent->ExecuteTrigger(m_playTrigger.m_id, m_auxObjectId, userData);
	}
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::Stop()
{
	if (m_pIEntityAudioComponent != nullptr)
	{
		if (m_stopTrigger.m_id != DrxAudio::InvalidControlId)
		{
			DrxAudio::SRequestUserData const userData(DrxAudio::ERequestFlags::DoneCallbackOnExternalThread | DrxAudio::ERequestFlags::CallbackOnExternalOrCallingThread, this);
			m_pIEntityAudioComponent->ExecuteTrigger(m_stopTrigger.m_id, m_auxObjectId, userData);
		}
		else if (m_playTrigger.m_id != DrxAudio::InvalidControlId)
		{
			m_pIEntityAudioComponent->StopTrigger(m_playTrigger.m_id, m_auxObjectId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::DetermineActivityRadius()
{
	DrxAudio::STriggerData playTriggerData;
	DrxAudio::STriggerData stopTriggerData;

	if (m_playTrigger.m_id != DrxAudio::InvalidControlId)
	{
		gEnv->pAudioSystem->GetTriggerData(m_playTrigger.m_id, playTriggerData);
	}

	if (m_stopTrigger.m_id != DrxAudio::InvalidControlId)
	{
		gEnv->pAudioSystem->GetTriggerData(m_stopTrigger.m_id, stopTriggerData);
	}

	m_activityRadius = std::max(playTriggerData.radius, stopTriggerData.radius);
}

//////////////////////////////////////////////////////////////////////////
void CTriggerComponent::GetActivityRadius(float& radius)
{
	radius = m_activityRadius;
}
} // namespace DefaultComponents
} // namespace Audio
} // namespace Drx
