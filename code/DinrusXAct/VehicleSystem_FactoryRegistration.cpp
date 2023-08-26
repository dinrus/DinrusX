// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/VehicleSystem.h>

#include <DinrusX/DinrusXAct/Vehicle.h>

#include <DinrusX/DinrusXAct/VehicleDamageBehaviorAISignal.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorDestroy.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorDetachPart.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorEffect.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorGroup.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorHitPassenger.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorImpulse.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorMovementNotification.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorSink.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorSpawnDebris.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorIndicator.h>
#include <DinrusX/DinrusXAct/VehicleDamageBehaviorDisableSeatAction.h>

#include <DinrusX/DinrusXAct/VehiclePartBase.h>
#include <DinrusX/DinrusXAct/VehiclePartAnimated.h>
#include <DinrusX/DinrusXAct/VehiclePartAnimatedJoint.h>
#include <DinrusX/DinrusXAct/VehiclePartLight.h>
#include <DinrusX/DinrusXAct/VehiclePartPulsingLight.h>
#include <DinrusX/DinrusXAct/VehiclePartMassBox.h>
#include <DinrusX/DinrusXAct/VehiclePartStatic.h>
#include <DinrusX/DinrusXAct/VehiclePartSubPart.h>
#include <DinrusX/DinrusXAct/VehiclePartSubPartWheel.h>
#include <DinrusX/DinrusXAct/VehiclePartSuspensionPart.h>
#include <DinrusX/DinrusXAct/VehiclePartTread.h>
#include <DinrusX/DinrusXAct/VehiclePartAttachment.h>
#include <DinrusX/DinrusXAct/VehiclePartEntity.h>
#include <DinrusX/DinrusXAct/VehiclePartEntityDelayedDetach.h>
#include <DinrusX/DinrusXAct/VehiclePartParticleEffect.h>
#include <DinrusX/DinrusXAct/VehiclePartAnimatedChar.h>
#include <DinrusX/DinrusXAct/VehiclePartWaterRipplesGenerator.h>
#include <DinrusX/DinrusXAct/VehiclePartDetachedEntity.h>

#include <DinrusX/DinrusXAct/VehicleSeatSerializer.h>

#include <DinrusX/DinrusXAct/VehicleSeatActionLights.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionMovement.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionPassengerIK.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionRotateTurret.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionSteeringWheel.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionWeapons.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionWeaponsBone.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionAnimation.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionPassiveAnimation.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionOrientatePartToView.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionOrientateBoneToView.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionRotateBone.h>
#include <DinrusX/DinrusXAct/VehicleSeatActionShakeParts.h>

#include <DinrusX/DinrusXAct/VehicleUsableActionEnter.h>
#include <DinrusX/DinrusXAct/VehicleUsableActionFlip.h>

#include <DinrusX/DinrusXAct/VehicleViewActionThirdPerson.h>
#include <DinrusX/DinrusXAct/VehicleViewSteer.h>
#include <DinrusX/DinrusXAct/VehicleViewFirstPerson.h>
#include <DinrusX/DinrusXAct/VehicleViewThirdPerson.h>

#include <DinrusX/DinrusXAct/DinrusAction.h>

//------------------------------------------------------------------------
void CVehicleSystem::RegisterVehicles(IGameFramework* gameFramework)
{
	LOADING_TIME_PROFILE_SECTION;
	IEntityClassRegistry::SEntityClassDesc serializerClass;
	serializerClass.sName = "VehicleSeatSerializer";
	serializerClass.sScriptFile = "";
	serializerClass.flags = ECLF_INVISIBLE;

	static IGameFramework::CGameObjectExtensionCreator<CVehicleSeatSerializer> createVehicleSeatSerializer;
	CDrxAction::GetDrxAction()->GetIGameObjectSystem()->RegisterExtension(serializerClass.sName, &createVehicleSeatSerializer, &serializerClass);

	// register the detached part entity

	IEntityClassRegistry::SEntityClassDesc detachedPartClass;
	detachedPartClass.sName = "VehiclePartDetached";
	detachedPartClass.sScriptFile = "Scripts/Entities/Vehicles/VehiclePartDetached.lua";
	detachedPartClass.flags = ECLF_INVISIBLE;

	static IGameFramework::CGameObjectExtensionCreator<CVehiclePartDetachedEntity> createVehicleDetachedPartEntity;
	CDrxAction::GetDrxAction()->GetIGameObjectSystem()->RegisterExtension(detachedPartClass.sName, &createVehicleDetachedPartEntity, &detachedPartClass);

	// register all the vehicles

	IDrxPak* pack = gEnv->pDrxPak;
	_finddata_t fd;
	int res;
	intptr_t handle;
	std::set<string> setVehicles;

	string sExt(".xml");
	string sPath("Scripts/Entities/Vehicles/Implementations/Xml/");

	if ((handle = pack->FindFirst(sPath + string("*") + sExt, &fd)) != -1)
	{
		do
		{
			if (XmlNodeRef root = GetISystem()->LoadXmlFromFile(sPath + string(fd.name)))
			{
				const char* name = root->getAttr("name");
				if (name[0])
				{
					// Allow the name to contain relative path, but use only the name part as class name.
					string className(PathUtil::GetFile(name));

					// register only once
					std::pair<std::set<string>::iterator, bool> result = setVehicles.insert(className);
					if (result.second)
					{
						IEntityClassRegistry::SEntityClassDesc vehicleClass;
						vehicleClass.sName = className.c_str();

						char scriptName[1024];

						const char* isOld = root->getAttr("isOld");
						if (!strcmp("1", isOld))
							drx_sprintf(scriptName, "Scripts/Entities/Vehicles/Old/VehiclePool.lua");
						else
							drx_sprintf(scriptName, "Scripts/Entities/Vehicles/VehiclePool.lua");

						bool show = true;
						if (root->getAttr("show", show))
						{
							if (!show && VehicleCVars().v_show_all == 0)
								vehicleClass.flags |= ECLF_INVISIBLE;
						}

						vehicleClass.sScriptFile = scriptName;

						static IGameFramework::CGameObjectExtensionCreator<CVehicle> vehicleCreator;
						CDrxAction::GetDrxAction()->GetIGameObjectSystem()->RegisterExtension(name, &vehicleCreator, &vehicleClass);
						m_classes.insert(TVehicleClassMap::value_type(name, &vehicleCreator));
					}
					else
						DrxLog("Vehicle <%s> already registered", name);
				}
				else
				{
					DrxLog("VehicleSystem: %s is missing 'name' attribute, skipping", fd.name);
				}
			}
			res = pack->FindNext(handle, &fd);
		}
		while (res >= 0);

		pack->FindClose(handle);
	}

#define REGISTER_VEHICLEOBJECT(name, obj)                    \
  REGISTER_FACTORY((IVehicleSystem*)this, name, obj, false); \
  obj::m_objectId = this->AssignVehicleObjectId(name);

	// register other factories

	// vehicle views
	REGISTER_VEHICLEOBJECT("ActionThirdPerson", CVehicleViewActionThirdPerson);
	REGISTER_VEHICLEOBJECT("SteerThirdPerson", CVehicleViewSteer);
	REGISTER_VEHICLEOBJECT("FirstPerson", CVehicleViewFirstPerson);
	REGISTER_VEHICLEOBJECT("ThirdPerson", CVehicleViewThirdPerson);

	// vehicle parts
	REGISTER_VEHICLEOBJECT("Base", CVehiclePartBase);
	REGISTER_VEHICLEOBJECT("Animated", CVehiclePartAnimated);
	REGISTER_VEHICLEOBJECT("AnimatedJoint", CVehiclePartAnimatedJoint);
	REGISTER_VEHICLEOBJECT("SuspensionPart", CVehiclePartSuspensionPart);
	REGISTER_VEHICLEOBJECT("Light", CVehiclePartLight);
	REGISTER_VEHICLEOBJECT("PulsingLight", CVehiclePartPulsingLight);
	REGISTER_VEHICLEOBJECT("MassBox", CVehiclePartMassBox);
	REGISTER_VEHICLEOBJECT("Static", CVehiclePartStatic);
	REGISTER_VEHICLEOBJECT("SubPart", CVehiclePartSubPart);
	REGISTER_VEHICLEOBJECT("SubPartWheel", CVehiclePartSubPartWheel);
	REGISTER_VEHICLEOBJECT("Tread", CVehiclePartTread);
	REGISTER_VEHICLEOBJECT("EntityAttachment", CVehiclePartEntityAttachment);
	REGISTER_VEHICLEOBJECT("Entity", CVehiclePartEntity);
	REGISTER_VEHICLEOBJECT("EntityDelayedDetach", CVehiclePartEntityDelayedDetach);
	REGISTER_VEHICLEOBJECT("ParticleEffect", CVehiclePartParticleEffect);
	REGISTER_VEHICLEOBJECT("AnimatedChar", CVehiclePartAnimatedChar);
	REGISTER_VEHICLEOBJECT("WaterRipplesGenerator", CVehiclePartWaterRipplesGenerator);

	// vehicle damage behaviors
	REGISTER_VEHICLEOBJECT("AISignal", CVehicleDamageBehaviorAISignal);
	REGISTER_VEHICLEOBJECT("Destroy", CVehicleDamageBehaviorDestroy);
	REGISTER_VEHICLEOBJECT("DetachPart", CVehicleDamageBehaviorDetachPart);
	REGISTER_VEHICLEOBJECT("Effect", CVehicleDamageBehaviorEffect);
	REGISTER_VEHICLEOBJECT("Group", CVehicleDamageBehaviorGroup);
	REGISTER_VEHICLEOBJECT("HitPassenger", CVehicleDamageBehaviorHitPassenger);
	REGISTER_VEHICLEOBJECT("Impulse", CVehicleDamageBehaviorImpulse);
	REGISTER_VEHICLEOBJECT("Indicator", CVehicleDamageBehaviorIndicator);
	REGISTER_VEHICLEOBJECT("MovementNotification", CVehicleDamageBehaviorMovementNotification);
	REGISTER_VEHICLEOBJECT("Sink", CVehicleDamageBehaviorSink);
	REGISTER_VEHICLEOBJECT("SpawnDebris", CVehicleDamageBehaviorSpawnDebris);
	REGISTER_VEHICLEOBJECT("DisableSeatAction", CVehicleDamageBehaviorDisableSeatAction);

	// seat actions
	REGISTER_VEHICLEOBJECT("Lights", CVehicleSeatActionLights);
	REGISTER_VEHICLEOBJECT("Movement", CVehicleSeatActionMovement);
	REGISTER_VEHICLEOBJECT("PassengerIK", CVehicleSeatActionPassengerIK);
	REGISTER_VEHICLEOBJECT("RotateTurret", CVehicleSeatActionRotateTurret);
	REGISTER_VEHICLEOBJECT("SteeringWheel", CVehicleSeatActionSteeringWheel);
	REGISTER_VEHICLEOBJECT("Weapons", CVehicleSeatActionWeapons);
	REGISTER_VEHICLEOBJECT("WeaponsBone", CVehicleSeatActionWeaponsBone);
	REGISTER_VEHICLEOBJECT("Animation", CVehicleSeatActionAnimation);
	REGISTER_VEHICLEOBJECT("PassiveAnimation", CVehicleSeatActionPassiveAnimation);
	REGISTER_VEHICLEOBJECT("OrientatePartToView", CVehicleSeatActionOrientatePartToView);
	REGISTER_VEHICLEOBJECT("OrientateBoneToView", CVehicleSeatActionOrientateBoneToView);
	REGISTER_VEHICLEOBJECT("RotateBone", CVehicleSeatActionRotateBone);
	REGISTER_VEHICLEOBJECT("ShakeParts", CVehicleSeatActionShakeParts);

	// actions
	REGISTER_FACTORY((IVehicleSystem*)this, "Enter", CVehicleUsableActionEnter, false);
	REGISTER_FACTORY((IVehicleSystem*)this, "Flip", CVehicleUsableActionFlip, false);
}
