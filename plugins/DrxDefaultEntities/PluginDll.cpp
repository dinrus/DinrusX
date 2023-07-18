// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "PluginDll.h"

#include "DefaultComponents/AI/PathfindingComponent.h"
#include "DefaultComponents/Audio/AreaComponent.h"
#include "DefaultComponents/Audio/EnvironmentComponent.h"
#include "DefaultComponents/Audio/ListenerComponent.h"
#include "DefaultComponents/Audio/OcclusionComponent.h"
#include "DefaultComponents/Audio/ParameterComponent.h"
#include "DefaultComponents/Audio/PreloadComponent.h"
#include "DefaultComponents/Audio/SwitchComponent.h"
#include "DefaultComponents/Audio/TriggerComponent.h"
#include "DefaultComponents/Cameras/CameraComponent.h"
#include "DefaultComponents/Cameras/VirtualReality/RoomscaleCamera.h"
#include "DefaultComponents/Constraints/LineConstraint.h"
#include "DefaultComponents/Constraints/PlaneConstraint.h"
#include "DefaultComponents/Constraints/PointConstraint.h"
#include "DefaultComponents/Constraints/BreakableJoint.h"
#include "DefaultComponents/Debug/DebugDrawComponent.h"
#include "DefaultComponents/Effects/DecalComponent.h"
#include "DefaultComponents/Effects/FogComponent.h"
#include "DefaultComponents/Effects/WaterRippleComponent.h"
#include "DefaultComponents/Effects/RainComponent.h"
#include "DefaultComponents/Effects/ParticleComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"
#include "DefaultComponents/Geometry/AlembicComponent.h"
#include "DefaultComponents/Geometry/AnimatedMeshComponent.h"
#include "DefaultComponents/Geometry/StaticMeshComponent.h"
#include "DefaultComponents/Input/InputComponent.h"
#include "DefaultComponents/Lights/EnvironmentProbeComponent.h"
#include "DefaultComponents/Lights/PointLightComponent.h"
#include "DefaultComponents/Lights/ProjectorLightComponent.h"
#include "DefaultComponents/Physics/BoxPrimitiveComponent.h"
#include "DefaultComponents/Physics/CapsulePrimitiveComponent.h"
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include "DefaultComponents/Physics/CylinderPrimitiveComponent.h"
#include "DefaultComponents/Physics/PhysicsPrimitiveComponent.h"
#include "DefaultComponents/Physics/RigidBodyComponent.h"
#include "DefaultComponents/Physics/SampleRigidbodyActorComponent.h"
#include "DefaultComponents/Physics/SpherePrimitiveComponent.h"
#include "DefaultComponents/Physics/AreaComponent.h"
#include "DefaultComponents/Physics/ThrusterComponent.h"
#include "DefaultComponents/Physics/Vehicles/VehicleComponent.h"
#include "DefaultComponents/Physics/Vehicles/WheelComponent.h"
#include "DefaultComponents/Physics/VirtualReality/VirtualRealityInteractionComponent.h"
#include "DefaultComponents/Utilities/ChildEntityComponent.h"
#include "DefaultComponents/Cameras/CameraUpr.h"

<DinrusX/DinrusXEntity/IEntityClass.h>

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

<DinrusX/CoreX/StaticInstanceList.h>

IEntityRegistrator* IEntityRegistrator::g_pFirst = nullptr;
IEntityRegistrator* IEntityRegistrator::g_pLast = nullptr;



CPlugin_DrxDefaultEntities::~CPlugin_DrxDefaultEntities()
{
	if (gEnv->pSchematyc != nullptr)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CPlugin_DrxDefaultEntities::GetCID());
	}

	if (ISystem* pSystem = GetISystem())
	{
		pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	}
}

bool CPlugin_DrxDefaultEntities::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	m_pCameraUpr = stl::make_unique<CCameraUpr>();

	env.pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CDrxPluginUpr");

	return true;
}

void CPlugin_DrxDefaultEntities::RegisterComponents(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CPathfindingComponent));
			Drx::DefaultComponents::CPathfindingComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CAreaComponent));
			Drx::Audio::DefaultComponents::CAreaComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CEnvironmentComponent));
			Drx::Audio::DefaultComponents::CEnvironmentComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CListenerComponent));
			Drx::Audio::DefaultComponents::CListenerComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::COcclusionComponent));
			Drx::Audio::DefaultComponents::COcclusionComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CParameterComponent));
			Drx::Audio::DefaultComponents::CParameterComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CPreloadComponent));
			Drx::Audio::DefaultComponents::CPreloadComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CSwitchComponent));
			Drx::Audio::DefaultComponents::CSwitchComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::Audio::DefaultComponents::CTriggerComponent));
			Drx::Audio::DefaultComponents::CTriggerComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CCameraComponent));
			Drx::DefaultComponents::CCameraComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::VirtualReality::CRoomscaleCameraComponent));
			Drx::DefaultComponents::VirtualReality::CRoomscaleCameraComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CLineConstraintComponent));
			Drx::DefaultComponents::CLineConstraintComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CPlaneConstraintComponent));
			Drx::DefaultComponents::CPlaneConstraintComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CPointConstraintComponent));
			Drx::DefaultComponents::CPointConstraintComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CBreakableJointComponent));
			Drx::DefaultComponents::CBreakableJointComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CDebugDrawComponent));
			Drx::DefaultComponents::CDebugDrawComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CDecalComponent));
			Drx::DefaultComponents::CDecalComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CFogComponent));
			Drx::DefaultComponents::CFogComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CParticleComponent));
			Drx::DefaultComponents::CParticleComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CAdvancedAnimationComponent));
			Drx::DefaultComponents::CAdvancedAnimationComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CAlembicComponent));
			Drx::DefaultComponents::CAlembicComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CAnimatedMeshComponent));
			Drx::DefaultComponents::CAnimatedMeshComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CStaticMeshComponent));
			Drx::DefaultComponents::CStaticMeshComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CInputComponent));
			Drx::DefaultComponents::CInputComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CEnvironmentProbeComponent));
			Drx::DefaultComponents::CEnvironmentProbeComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CPointLightComponent));
			Drx::DefaultComponents::CPointLightComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CProjectorLightComponent));
			Drx::DefaultComponents::CProjectorLightComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CBoxPrimitiveComponent));
			Drx::DefaultComponents::CBoxPrimitiveComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CCapsulePrimitiveComponent));
			Drx::DefaultComponents::CCapsulePrimitiveComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CCharacterControllerComponent));
			Drx::DefaultComponents::CCharacterControllerComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CCylinderPrimitiveComponent));
			Drx::DefaultComponents::CCylinderPrimitiveComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CRigidBodyComponent));
			Drx::DefaultComponents::CRigidBodyComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CSampleActorComponent));
			Drx::DefaultComponents::CSampleActorComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CSpherePrimitiveComponent));
			Drx::DefaultComponents::CSpherePrimitiveComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CWaterRippleComponent));
			Drx::DefaultComponents::CWaterRippleComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CRainComponent));
			Drx::DefaultComponents::CRainComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CAreaComponent));
			Drx::DefaultComponents::CAreaComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CThrusterComponent));
			Drx::DefaultComponents::CThrusterComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CVehiclePhysicsComponent));
			Drx::DefaultComponents::CVehiclePhysicsComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CWheelComponent));
			Drx::DefaultComponents::CWheelComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::VirtualReality::CInteractionComponent));
			Drx::DefaultComponents::VirtualReality::CInteractionComponent::Register(componentScope);
		}
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Drx::DefaultComponents::CChildEntityComponent));
			Drx::DefaultComponents::CChildEntityComponent::Register(componentScope);
		}
	}
}

void CPlugin_DrxDefaultEntities::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			// Register legacy components introduced with 5.3 and below
			if (IEntityRegistrator::g_pFirst != nullptr)
			{
				do
				{
					IEntityRegistrator::g_pFirst->Register();

					IEntityRegistrator::g_pFirst = IEntityRegistrator::g_pFirst->m_pNext;
				}
				while (IEntityRegistrator::g_pFirst != nullptr);
			}

			// Register dummy entities
			IEntityClassRegistry::SEntityClassDesc stdClass;
			stdClass.flags |= ECLF_INVISIBLE;

			stdClass.sName = "AreaBox";
			gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

			stdClass.sName = "AreaSphere";
			gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

			stdClass.sName = "AreaSolid";
			gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

			stdClass.sName = "ClipVolume";
			gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

			stdClass.sName = "AreaShape";
			gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(stdClass);

			if (gEnv->pSchematyc != nullptr)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CPlugin_DrxDefaultEntities::GetCID(),
						"EntityComponents",
						"DinrusPro",
						"DRXENGINE Default Entity Components",
						[this](Schematyc::IEnvRegistrar& registrar) { RegisterComponents(registrar); }
						)
				);
			}
		}
		break;
	}
}

DRXREGISTER_SINGLETON_CLASS(CPlugin_DrxDefaultEntities)

<DinrusX/CoreX/CrtDebugStats.h>
