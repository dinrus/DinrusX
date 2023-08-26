// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:  Register the factory templates used to create classes from names
e.g. REGISTER_FACTORY(pFramework, "Player", CPlayer, false);
or   REGISTER_FACTORY(pFramework, "Player", CPlayerG4, false);

Since overriding this function creates template based linker errors,
it's been replaced by a standalone function in its own cpp file.

-------------------------------------------------------------------------
History:
- 17:8:2005   Created by Nick Hesketh - Refactor'd from Game.cpp/h

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/Game.h>
#include <DinrusX/DinrusXGame/Player.h>

//
#include <DinrusX/DinrusXGame/Item.h>
#include <DinrusX/DinrusXGame/Weapon.h>
#include <DinrusX/DinrusXGame/VehicleWeapon.h>
#include <DinrusX/DinrusXGame/VehicleWeaponGuided.h>
#include <DinrusX/DinrusXGame/VehicleWeaponControlled.h>
#include <DinrusX/DinrusXGame/VehicleMountedWeapon.h>
#include <DinrusX/DinrusXGame/Binocular.h>
#include <DinrusX/DinrusXGame/C4.h>
#include <DinrusX/DinrusXGame/DebugGun.h>
#include <DinrusX/DinrusXGame/GunTurret.h>
#include <DinrusX/DinrusXGame/JAW.h>
#include <DinrusX/DinrusXGame/AIGrenade.h>
#include <DinrusX/DinrusXGame/Accessory.h>
#include <DinrusX/DinrusXGame/HandGrenades.h>
#include <DinrusX/DinrusXGame/EnvironmentalWeapon.h>
#include <DinrusX/DinrusXGame/Laser.h>
#include <DinrusX/DinrusXGame/flashlight.h>
#include <DinrusX/DinrusXGame/DoubleMagazine.h>
#include <DinrusX/DinrusXGame/LTAG.h>
#include <DinrusX/DinrusXGame/HeavyMountedWeapon.h>
#include <DinrusX/DinrusXGame/HeavyWeapon.h>
#include <DinrusX/DinrusXGame/PickAndThrowWeapon.h>
#include <DinrusX/DinrusXGame/NoWeapon.h>
#include <DinrusX/DinrusXGame/WeaponMelee.h>
#include <DinrusX/DinrusXGame/UseableTurret.h>
#include <DinrusX/DinrusXGame/CinematicWeapon.h>

#include <DinrusX/DinrusXGame/DummyPlayer.h>

#include <DinrusX/DinrusXGame/ReplayObject.h>
#include <DinrusX/DinrusXGame/ReplayActor.h>

#include <DinrusX/DinrusXGame/MultiplayerEntities/CarryEntity.h>

#include <DinrusX/DinrusXGame/VehicleMovementBase.h>
#include <DinrusX/DinrusXGame/Vehicle/VehicleMovementDummy.h>
#include <DinrusX/DinrusXGame/VehicleActionAutomaticDoor.h>
#include <DinrusX/DinrusXGame/VehicleActionDeployRope.h>
#include <DinrusX/DinrusXGame/VehicleActionEntityAttachment.h>
#include <DinrusX/DinrusXGame/VehicleActionLandingGears.h>
#include <DinrusX/DinrusXGame/VehicleActionAutoTarget.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorBurn.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorCameraShake.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorExplosion.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorTire.h>
#include <DinrusX/DinrusXGame/VehicleDamageBehaviorAudioFeedback.h>
#include <DinrusX/DinrusXGame/VehicleMovementStdWheeled.h>
#include <DinrusX/DinrusXGame/VehicleMovementStdTank.h>
#include <DinrusX/DinrusXGame/VehicleMovementArcadeWheeled.h>
//#include <DinrusX/DinrusXGame/VehicleMovementHelicopterArcade.h>
#include <DinrusX/DinrusXGame/VehicleMovementHelicopter.h>
#include <DinrusX/DinrusXGame/VehicleMovementStdBoat.h>
#include <DinrusX/DinrusXGame/VehicleMovementTank.h>
#include <DinrusX/DinrusXGame/VehicleMovementMPVTOL.h>

#include <DinrusX/DinrusXGame/ScriptControlledPhysics.h>

#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesModulesUpr.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesTeamsModule.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardTwoTeams.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesGladiatorTeams.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesStateModule.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardState.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardVictoryConditionsTeam.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardVictoryConditionsPlayer.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjectiveVictoryConditionsTeam.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjectiveVictoryConditionsIndividualScore.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesExtractionVictoryConditions.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesSurvivorOneVictoryConditions.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardSetup.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardScoring.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesAssistScoring.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardPlayerStats.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesSpawningModule.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesSpawningBase.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPSpawning.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPSpawningWithLives.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPWaveSpawning.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPDamageHandling.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPActorAction.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesMPSpectator.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesSPDamageHandling.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjective_Predator.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStandardRounds.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesStatsRecording.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjective_PowerStruggle.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjective_Extraction.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesSimpleEntityBasedObjective.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesObjective_CTF.h>

#include <DinrusX/DinrusXGame/Environment/Tornado.h>
#include <DinrusX/DinrusXGame/Environment/Shake.h>
#include <DinrusX/DinrusXGame/Environment/Rain.h>
#include <DinrusX/DinrusXGame/Environment/Snow.h>
#include <DinrusX/DinrusXGame/Environment/InteractiveObject.h>
#include <DinrusX/DinrusXGame/Environment/DeflectorShield.h>
#include <DinrusX/DinrusXGame/Environment/DangerousRigidBody.h>
#include <DinrusX/DinrusXGame/Environment/Ledge.h>
#include <DinrusX/DinrusXGame/Environment/WaterPuddle.h>
#include <DinrusX/DinrusXGame/Environment/SmartMine.h>
#include <DinrusX/DinrusXGame/Environment/MineField.h>
#include <DinrusX/DinrusXGame/Environment/DoorPanel.h>
#include <DinrusX/DinrusXGame/Environment/VicinityDependentObjectMover.h>
#include <DinrusX/DinrusXGame/Environment/WaterRipplesGenerator.h>
#include <DinrusX/DinrusXGame/Environment/LightningArc.h>

#include <DinrusX/DinrusXGame/AI/AICorpse.h>

#include <DinrusX/DinrusXGame/Turret/Turret/Turret.h>
#include <DinrusX/DinrusXGame/MPPath.h>

#include <DinrusX/DinrusXAct/IItemSystem.h>
#include <DinrusX/DinrusXAct/IVehicleSystem.h>
#include <IGameRulesSystem.h>
#include <DinrusX/CoreX/Game/IGameVolumes.h>

#include <DinrusX/DinrusXGame/GameCVars.h>

#define HIDE_FROM_EDITOR(className)																																				\
  { IEntityClass *pItemClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(className);\
  pItemClass->SetFlags(pItemClass->GetFlags() | ECLF_INVISIBLE); }																				\

#define REGISTER_EDITOR_VOLUME_CLASS(frameWork, className)                                          \
{	                                                                                                  \
	IGameVolumes* pGameVolumes = frameWork->GetIGameVolumesUpr();                                 \
	IGameVolumesEdit* pGameVolumesEdit = pGameVolumes ? pGameVolumes->GetEditorInterface() : NULL;    \
	if (pGameVolumesEdit != NULL)                                                                     \
	{                                                                                                 \
		pGameVolumesEdit->RegisterEntityClass( className );                                             \
	}                                                                                                 \
} 

#define REGISTER_GAME_OBJECT(framework, name, script)\
	{\
	IEntityClassRegistry::SEntityClassDesc clsDesc;\
	clsDesc.sName = #name;\
	clsDesc.sScriptFile = script;\
struct C##name##Creator : public IGameObjectExtensionCreatorBase\
		{\
		IGameObjectExtensionPtr Create()\
			{\
			return ComponentCreate_DeleteWithRelease<C##name>();\
			}\
			void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
			{\
			C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
			}\
		};\
		static C##name##Creator _creator;\
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);\
	}

#define REGISTER_GAME_OBJECT_WITH_IMPL(framework, name, impl, script)\
	{\
	IEntityClassRegistry::SEntityClassDesc clsDesc;\
	clsDesc.sName = #name;\
	clsDesc.sScriptFile = script;\
struct C##name##Creator : public IGameObjectExtensionCreatorBase\
		{\
		IGameObjectExtensionPtr Create()\
			{\
			return ComponentCreate_DeleteWithRelease<C##impl>();\
			}\
			void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
			{\
			C##impl::GetGameObjectExtensionRMIData( ppRMI, nCount );\
			}\
		};\
		static C##name##Creator _creator;\
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);\
	}

#define REGISTER_GAME_OBJECT_EXTENSION(framework, name)\
	{\
struct C##name##Creator : public IGameObjectExtensionCreatorBase\
		{\
		IGameObjectExtensionPtr Create()\
			{\
			return ComponentCreate_DeleteWithRelease<C##name>();\
			}\
			void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
			{\
			C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
			}\
		};\
		static C##name##Creator _creator;\
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, NULL);\
	}

// Register the factory templates used to create classes from names. Called via CGame::Init()
void InitGameFactory(IGameFramework *pFramework)
{
	assert(pFramework);

	REGISTER_FACTORY(pFramework, "Player", CPlayer, false);
	REGISTER_FACTORY(pFramework, "PlayerHeavy", CPlayer, false);
	
	REGISTER_FACTORY(pFramework, "DamageTestEnt", CPlayer, true);

#if (USE_DEDICATED_INPUT)
	REGISTER_FACTORY(pFramework, "DummyPlayer", CDummyPlayer, true);
#endif

	//REGISTER_FACTORY(pFramework, "Civilian", CPlayer, true);

	// Null AI for AI pool
	REGISTER_FACTORY(pFramework, "NullAI", CPlayer, true);

	// Characters	
	REGISTER_FACTORY(pFramework, "Characters/Human", CPlayer, true);
	
	// Items
	REGISTER_FACTORY(pFramework, "Item", CItem, false);
	REGISTER_FACTORY(pFramework, "Accessory", CAccessory, false);
	REGISTER_FACTORY(pFramework, "Laser", CLaser, false);
	REGISTER_FACTORY(pFramework, "FlashLight", CFlashLight, false);
	REGISTER_FACTORY(pFramework, "DoubleMagazine", CDoubleMagazine, false);
	REGISTER_FACTORY(pFramework, "HandGrenades", CHandGrenades, false);

	// Weapons
	REGISTER_FACTORY(pFramework, "Weapon", CWeapon, false);
	REGISTER_FACTORY(pFramework, "VehicleWeapon", CVehicleWeapon, false);
	REGISTER_FACTORY(pFramework, "VehicleWeaponGuided", CVehicleWeaponGuided, false);
	REGISTER_FACTORY(pFramework, "VehicleWeaponControlled", CVehicleWeaponControlled, false);
	REGISTER_FACTORY(pFramework, "VehicleWeaponPulseC", CVehicleWeaponPulseC, false);
	REGISTER_FACTORY(pFramework, "VehicleMountedWeapon", CVehicleMountedWeapon, false);
	REGISTER_FACTORY(pFramework, "Binocular", CBinocular, false);
	REGISTER_FACTORY(pFramework, "C4", CC4, false);
	REGISTER_FACTORY(pFramework, "DebugGun", CDebugGun, false);
	REGISTER_FACTORY(pFramework, "GunTurret", CGunTurret, false);
	REGISTER_FACTORY(pFramework, "JAW", CJaw, false);
	REGISTER_FACTORY(pFramework, "AIGrenade", CAIGrenade, false);
	REGISTER_FACTORY(pFramework, "AISmokeGrenades", CAIGrenade, false);
	REGISTER_FACTORY(pFramework, "AIEMPGrenade", CAIGrenade, false);
	REGISTER_FACTORY(pFramework, "LTAG", CLTag, false);
	REGISTER_FACTORY(pFramework, "PickAndThrowWeapon", CPickAndThrowWeapon, false);
	REGISTER_FACTORY(pFramework, "NoWeapon", CNoWeapon, false);
	REGISTER_FACTORY(pFramework, "HeavyMountedWeapon", CHeavyMountedWeapon, false);
	REGISTER_FACTORY(pFramework, "HeavyWeapon", CHeavyWeapon, false);
	REGISTER_FACTORY(pFramework, "WeaponMelee", CWeaponMelee, false);
	REGISTER_FACTORY(pFramework, "UseableTurret", CUseableTurret, false);
	REGISTER_FACTORY(pFramework, "CinematicWeapon", CCinematicWeapon, false);
	
	// vehicle objects
	IVehicleSystem* pVehicleSystem = pFramework->GetIVehicleSystem();

#define REGISTER_VEHICLEOBJECT(name, obj) \
	REGISTER_FACTORY((IVehicleSystem*)pVehicleSystem, name, obj, false); \
	obj::m_objectId = pVehicleSystem->AssignVehicleObjectId(name);

	// damage behaviours
	REGISTER_VEHICLEOBJECT("Burn", CVehicleDamageBehaviorBurn);
	REGISTER_VEHICLEOBJECT("CameraShake", CVehicleDamageBehaviorCameraShake);
	REGISTER_VEHICLEOBJECT("Explosion", CVehicleDamageBehaviorExplosion);
	REGISTER_VEHICLEOBJECT("BlowTire", CVehicleDamageBehaviorBlowTire);
	REGISTER_VEHICLEOBJECT("AudioFeedback", CVehicleDamageBehaviorAudioFeedback);

	// actions
	REGISTER_VEHICLEOBJECT("AutomaticDoor", CVehicleActionAutomaticDoor);
	REGISTER_VEHICLEOBJECT("EntityAttachment", CVehicleActionEntityAttachment);
	REGISTER_VEHICLEOBJECT("LandingGears", CVehicleActionLandingGears);
	REGISTER_VEHICLEOBJECT("AutoAimTarget", CVehicleActionAutoTarget);

	//seat actions
	REGISTER_VEHICLEOBJECT("DeployRope", CVehicleActionDeployRope);

	// vehicle movements
	REGISTER_FACTORY(pVehicleSystem, "DummyMovement", CVehicleMovementDummy, false);
	//REGISTER_FACTORY(pVehicleSystem, "HelicopterArcade", CVehicleMovementHelicopterArcade, false);
	REGISTER_FACTORY(pVehicleSystem, "Helicopter", CVehicleMovementHelicopter, false);
	REGISTER_FACTORY(pVehicleSystem, "StdBoat", CVehicleMovementStdBoat, false);
	REGISTER_FACTORY(pVehicleSystem, "StdWheeled", CVehicleMovementStdWheeled, false);
	REGISTER_FACTORY(pVehicleSystem, "StdTank", CVehicleMovementStdTank, false);
	REGISTER_FACTORY(pVehicleSystem, "ArcadeWheeled", CVehicleMovementArcadeWheeled, false);
	REGISTER_FACTORY(pVehicleSystem, "Tank", CVehicleMovementTank, false);
	REGISTER_FACTORY(pVehicleSystem, "MPVTOL", CVehicleMovementMPVTOL, false);


	// Custom GameObjects
	REGISTER_GAME_OBJECT(pFramework, Tornado, "Scripts/Entities/Environment/Tornado.lua");
	REGISTER_GAME_OBJECT(pFramework, Shake, "Scripts/Entities/Environment/Shake.lua");
	REGISTER_GAME_OBJECT(pFramework, Rain, "Scripts/Entities/Environment/Rain.lua");
	REGISTER_GAME_OBJECT(pFramework, Snow, "Scripts/Entities/Environment/Snow.lua");
	REGISTER_GAME_OBJECT(pFramework, InteractiveObjectEx, "Scripts/Entities/PlayerInteractive/InteractiveObjectEx.lua");
	REGISTER_GAME_OBJECT(pFramework, DeployableBarrier, "Scripts/Entities/PlayerInteractive/DeployableBarrier.lua");
	REGISTER_GAME_OBJECT(pFramework, ReplayObject, "");
	REGISTER_GAME_OBJECT(pFramework, ReplayActor, "");
	REGISTER_GAME_OBJECT(pFramework, DeflectorShield, "Scripts/Entities/Others/DeflectorShield.lua");
	HIDE_FROM_EDITOR("DeflectorShield");
	REGISTER_GAME_OBJECT(pFramework, EnvironmentalWeapon, "Scripts/Entities/Multiplayer/EnvironmentWeapon_Rooted.lua");
	REGISTER_GAME_OBJECT(pFramework, DangerousRigidBody, "Scripts/Entities/Multiplayer/DangerousRigidBody.lua");
	REGISTER_GAME_OBJECT(pFramework, AICorpse, "");
	HIDE_FROM_EDITOR("ReplayObject");
	HIDE_FROM_EDITOR("ReplayActor");
	HIDE_FROM_EDITOR("AICorpse");
	HIDE_FROM_EDITOR("NullAI");

	//////////////////////////////////////////////////////////////////////////
	/// Shape/Volume objects
	REGISTER_GAME_OBJECT(pFramework, MPPath, "Scripts/Entities/Multiplayer/MPPath.lua");
	HIDE_FROM_EDITOR("MPPath");
	REGISTER_EDITOR_VOLUME_CLASS( pFramework, "MPPath" );

	REGISTER_GAME_OBJECT(pFramework, LedgeObject, "Scripts/Entities/ContextualNavigation/LedgeObject.lua");
	HIDE_FROM_EDITOR("LedgeObject");
	REGISTER_EDITOR_VOLUME_CLASS( pFramework, "LedgeObject" );
	REGISTER_GAME_OBJECT(pFramework, LedgeObjectStatic, "Scripts/Entities/ContextualNavigation/LedgeObjectStatic.lua");
	HIDE_FROM_EDITOR("LedgeObjectStatic");
	REGISTER_EDITOR_VOLUME_CLASS( pFramework, "LedgeObjectStatic" );

	REGISTER_GAME_OBJECT(pFramework, WaterPuddle, "Scripts/Entities/Environment/WaterPuddle.lua");
	HIDE_FROM_EDITOR("WaterPuddle");
	REGISTER_EDITOR_VOLUME_CLASS(pFramework, "WaterPuddle");
	//////////////////////////////////////////////////////////////////////////


	REGISTER_GAME_OBJECT(pFramework, SmartMine, "Scripts/Entities/Environment/SmartMine.lua");
	REGISTER_GAME_OBJECT(pFramework, MineField, "Scripts/Entities/Environment/MineField.lua");
	REGISTER_GAME_OBJECT(pFramework, DoorPanel, "Scripts/Entities/Environment/DoorPanel.lua");
	REGISTER_GAME_OBJECT(pFramework, VicinityDependentObjectMover, "Scripts/Entities/Environment/VicinityDependentObjectMover.lua");
	REGISTER_GAME_OBJECT(pFramework, WaterRipplesGenerator, "Scripts/Entities/Environment/WaterRipplesGenerator.lua");
	REGISTER_GAME_OBJECT(pFramework, LightningArc, "Scripts/Entities/Environment/LightningArc.lua");

	REGISTER_GAME_OBJECT_WITH_IMPL(pFramework, CTFFlag, CarryEntity, "Scripts/Entities/Multiplayer/CTFFlag.lua");
	
	REGISTER_GAME_OBJECT_WITH_IMPL(pFramework, Turret, Turret, "Scripts/Entities/Turret/Turret.lua");
	
	REGISTER_GAME_OBJECT_EXTENSION(pFramework, ScriptControlledPhysics);

	HIDE_FROM_EDITOR("CTFFlag");
	IEntityClassRegistry::SEntityClassDesc stdClass;
	stdClass.flags |= ECLF_INVISIBLE|ECLF_DEFAULT;
	stdClass.sName = "Corpse";
	gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

	//GameRules
	REGISTER_FACTORY(pFramework, "GameRules", CGameRules, false);

	IGameRulesModulesUpr *pGameRulesModulesUpr = CGameRulesModulesUpr::GetInstance();

	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardTwoTeams", CGameRulesStandardTwoTeams, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "GladiatorTeams", CGameRulesGladiatorTeams, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardState", CGameRulesStandardState, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardVictoryConditionsTeam", CGameRulesStandardVictoryConditionsTeam, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardVictoryConditionsPlayer", CGameRulesStandardVictoryConditionsPlayer, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "ObjectiveVictoryConditionsTeam", CGameRulesObjectiveVictoryConditionsTeam, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "ObjectiveVictoryConditionsIndiv", CGameRulesObjectiveVictoryConditionsIndividualScore, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "ExtractionVictoryConditions", CGameRulesExtractionVictoryConditions, false);
#if SURVIVOR_ONE_ENABLED
	REGISTER_FACTORY(pGameRulesModulesUpr, "SurvivorOneVictoryConditions", CGameRulesSurvivorOneVictoryConditions, false);
#endif
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardSetup", CGameRulesStandardSetup, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardScoring", CGameRulesStandardScoring, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "AssistScoring", CGameRulesAssistScoring, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardPlayerStats", CGameRulesStandardPlayerStats, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "SpawningBase", CGameRulesSpawningBase, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPRSSpawning", CGameRulesRSSpawning, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardStatsRecording", CGameRulesStatsRecording, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPSpawningWithLives", CGameRulesMPSpawningWithLives, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPWaveSpawning", CGameRulesMPWaveSpawning, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPDamageHandling", CGameRulesMPDamageHandling, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPActorAction", CGameRulesMPActorAction, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "MPSpectator", CGameRulesMPSpectator, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "SPDamageHandling", CGameRulesSPDamageHandling, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "Objective_Predator", CGameRulesObjective_Predator, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "StandardRounds", CGameRulesStandardRounds, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "Objective_PowerStruggle", CGameRulesObjective_PowerStruggle, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "Objective_Extraction", CGameRulesObjective_Extraction, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "Objective_SimpleEntityBased", CGameRulesSimpleEntityBasedObjective, false);
	REGISTER_FACTORY(pGameRulesModulesUpr, "Objective_CTF", CGameRulesObjective_CTF, false);

	pGameRulesModulesUpr->Init();
}
