// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/CoreAPI.h>
#include <DinrusX/DinrusXSchema/CoreEnv.h>

namespace Schematyc
{

void RegisterBasicTypes(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_stdModuleGUID);
	{
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(bool));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(int32));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(uint32));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(CSharedString));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ObjectId));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ExplicitEntityId));
	}
}

} // Schematyc

DRX_STATIC_AUTO_REGISTER_FUNCTION(&Schematyc::RegisterBasicTypes)
