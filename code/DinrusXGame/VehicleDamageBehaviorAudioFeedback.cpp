// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorAudioFeedback.h>

#include <DinrusX/DinrusXGame/Game.h>
#include <DinrusX/DinrusXGame/Audio/GameAudio.h>
#include <DinrusX/DinrusXGame/Audio/AudioSignalPlayer.h>
#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/DinrusXGame/Utility/DesignerWarning.h>

//------------------------------------------------------------------------
CVehicleDamageBehaviorAudioFeedback::CVehicleDamageBehaviorAudioFeedback()
	: m_vehicleId(0)
	, m_firstPersonSignal(INVALID_AUDIOSIGNAL_ID)
	, m_thirdPersonSignal(INVALID_AUDIOSIGNAL_ID)
{
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorAudioFeedback::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	if (gEnv->IsClient())
	{
		m_vehicleId = pVehicle->GetEntityId();

		pVehicle->RegisterVehicleEventListener(this, "DamageBehaviorAudioFeedback");

		CVehicleParams sound = table.findChild("Sound");
		DesignerWarning(sound.haveAttr("firstPersonSignal") && sound.haveAttr("thirdPersonSignal"), "Expected name and target attributes");

		m_firstPersonSignal = g_pGame->GetGameAudio()->GetSignalID(sound.getAttr("firstPersonSignal"));
		m_thirdPersonSignal = g_pGame->GetGameAudio()->GetSignalID(sound.getAttr("thirdPersonSignal"));
	}

	return true;
}

//------------------------------------------------------------------------
CVehicleDamageBehaviorAudioFeedback::~CVehicleDamageBehaviorAudioFeedback()
{
	if (gEnv->IsClient())
	{
		IVehicle* pVehicle = gEnv->pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(m_vehicleId);
		if(pVehicle)
		{
			pVehicle->UnregisterVehicleEventListener(this);
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorAudioFeedback::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	DRX_ASSERT(gEnv->IsClient());

	if (event == eVE_Hit)
	{
		TAudioSignalID signal = GetSignal();
		CAudioSignalPlayer player;
		player.SetSignal(signal);
		player.SetParam(m_vehicleId, "damage", params.fParam);
		player.SetParam(m_vehicleId, "hittype", (float) params.iParam);
		//player.Play(m_vehicleId);
		REINST("vehicle audio");

#if !defined(_RELEASE)
		static ICVar* pDebugVehicle = gEnv->pConsole->GetCVar("v_debugVehicle");
		if(pDebugVehicle)
		{
			IVehicle* pVehicle = gEnv->pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(m_vehicleId);
			if(pVehicle)
			{
				const char* debugVehicle = pDebugVehicle->GetString();
				const char* entityName = pVehicle->GetEntity()->GetName();
				if(strcmpi(debugVehicle, entityName) == 0 || strcmpi(debugVehicle, "*") == 0)
				{
					DrxLog("CVehicleDamageBehaviorAudioFeedback signal %d, damage %.2f, hittype (%s) %d", signal, params.fParam, g_pGame->GetGameRules()->GetHitType(params.iParam, "unknown"), params.iParam);
				}
			}
		}
#endif
	}
}

//------------------------------------------------------------------------
TAudioSignalID CVehicleDamageBehaviorAudioFeedback::GetSignal() const
{
	DRX_ASSERT(gEnv->IsClient());

	IActor* pLocalActor = gEnv->pGame->GetIGameFramework()->GetClientActor();
	if(pLocalActor)
	{
		IVehicle* pVehicle = pLocalActor->GetLinkedVehicle();
		if(pVehicle && pVehicle->GetEntityId() == m_vehicleId)
		{
			IVehicleSeat* pSeat = pVehicle->GetSeatForPassenger(pLocalActor->GetEntityId());
			IVehicleView* pView = pSeat ? pSeat->GetCurrentView() : NULL;
			if (pView && !pView->IsThirdPerson())
			{
				return m_firstPersonSignal;
			}
		}
	}

	return m_thirdPersonSignal;
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorAudioFeedback);
