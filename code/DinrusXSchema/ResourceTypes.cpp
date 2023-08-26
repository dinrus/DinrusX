// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>

#include <DinrusX/DinrusXSchema/ResourceTypes.h>

#include <DinrusX/DinrusXSchema/CoreAPI.h>
#include <DinrusX/DinrusXSchema/CoreEnv.h>

namespace Schematyc
{
void RegisterResourceTypes(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_resourceModuleGUID);
	{
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(MaterialFileName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(GeomFileName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(SkinName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(CharacterFileName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ParticleEffectName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioEnvironmentName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioPreloadRequestName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioRtpcName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioSwitchName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioSwitchStateName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(AudioTriggerName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(DialogName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(EntityClassName));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ForceFeedbackId));
	}
}
} // namespace Schematyc

DRX_STATIC_AUTO_REGISTER_FUNCTION(&Schematyc::RegisterResourceTypes)
