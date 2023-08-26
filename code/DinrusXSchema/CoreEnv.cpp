// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/CoreEnv.h>
#include <DinrusX/DinrusXSchema/IEnvRegistrar.h>
#include <DinrusX/DinrusXSchema/EnvModule.h>
#include <DinrusX/DinrusXSchema/CoreEnvSignals.h>

namespace Schematyc
{
void RegisterCoreEnvPackage(IEnvRegistrar& registrar)
{
	RegisterCoreEnvSignals(registrar);

	registrar.RootScope().Register(SCHEMATYC_MAKE_ENV_MODULE(g_stdModuleGUID, "Standard"));
	{
		CEnvRegistrationScope scope = registrar.Scope(g_stdModuleGUID);
		scope.Register(SCHEMATYC_MAKE_ENV_MODULE(g_logModuleGUID, "Log"));
		scope.Register(SCHEMATYC_MAKE_ENV_MODULE(g_mathModuleGUID, "Math"));
		scope.Register(SCHEMATYC_MAKE_ENV_MODULE(g_resourceModuleGUID, "Resource"));

		DrxInvokeStaticCallbacks<Schematyc::IEnvRegistrar&>(registrar);
		//DrxInvokeStaticCallbacks<int>(0);
	}
}
} // Schematyc
