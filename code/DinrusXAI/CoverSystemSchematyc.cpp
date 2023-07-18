// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/CoverSystemSchematyc.h>

#include <DinrusX/DinrusXAI/CoverUserComponent.h>

namespace CoverSystemSchematyc
{
	void Register(Schematyc::IEnvRegistrar& registrar, Schematyc::CEnvRegistrationScope& parentScope)
	{
		//Register Components
		CEntityAICoverUserComponent::Register(registrar);
		
		const DrxGUID CoverSystemGUID = "736C4B57-6DEE-405C-9C06-58659FAB999A"_drx_guid;

		parentScope.Register(SCHEMATYC_MAKE_ENV_MODULE(CoverSystemGUID, "Covers"));
		Schematyc::CEnvRegistrationScope coversScope = registrar.Scope(CoverSystemGUID);

		// Nothing registered now
	}
}
