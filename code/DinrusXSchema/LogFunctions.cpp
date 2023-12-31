// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>

#include <DinrusX/DinrusXSchema/CoreAPI.h>

#include <DinrusX/DinrusXSchema/CoreEnv.h>

namespace Schematyc
{
namespace Log
{
void Comment(const CSharedString& message, const SLogStreamName& streamName)
{
	SCHEMATYC_COMMENT(gEnv->pSchematyc->GetLog().GetStreamId(streamName.value.c_str()), message.c_str());
}

void Warning(const CSharedString& message, const SLogStreamName& streamName)
{
	SCHEMATYC_WARNING(gEnv->pSchematyc->GetLog().GetStreamId(streamName.value.c_str()), message.c_str());
}

void Error(const CSharedString& message, const SLogStreamName& streamName)
{
	SCHEMATYC_ERROR(gEnv->pSchematyc->GetLog().GetStreamId(streamName.value.c_str()), message.c_str());
}

static void RegisterFunctions(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_logModuleGUID);
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Comment, "71c2cf50-8bbb-4d35-ba71-f6ffa895141d"_drx_guid, "Comment");
		pFunction->SetDescription("Send comment to log");
		pFunction->BindInput(1, 'msg', "Message");
		pFunction->BindInput(2, 'str', "Stream");
		scope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Warning, "8f9dd16b-b1e7-40fd-af24-93b2eb887a8a"_drx_guid, "Warning");
		pFunction->SetDescription("Send warning to log");
		pFunction->BindInput(1, 'msg', "Message");
		pFunction->BindInput(2, 'str', "Stream");
		scope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Error, "feb709b5-3ebe-4186-81e9-ec8d49f30397"_drx_guid, "Error");
		pFunction->SetDescription("Send error to log");
		pFunction->BindInput(1, 'msg', "Message");
		pFunction->BindInput(2, 'str', "Stream");
		scope.Register(pFunction);
	}
}
} // Log
} // Schematyc

DRX_STATIC_AUTO_REGISTER_FUNCTION(&Schematyc::Log::RegisterFunctions)
