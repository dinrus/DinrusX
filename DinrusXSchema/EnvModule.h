// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/EnvElementBase.h>
#include <DinrusX/DinrusXSchema/IEnvModule.h>

#define SCHEMATYC_MAKE_ENV_MODULE(guid, name) Schematyc::EnvModule::MakeShared(guid, name, SCHEMATYC_SOURCE_FILE_INFO)

namespace Schematyc
{

class CEnvModule : public CEnvElementBase<IEnvModule>
{
public:

	inline CEnvModule(const DrxGUID& guid, const char* szName, const SSourceFileInfo& sourceFileInfo)
		: CEnvElementBase(guid, szName, sourceFileInfo)
	{}

	// IEnvElement

	virtual bool IsValidScope(IEnvElement& scope) const override
	{
		switch (scope.GetType())
		{
		case EEnvElementType::Root:
		case EEnvElementType::Module:
			{
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	// ~IEnvElement
};

namespace EnvModule
{

inline std::shared_ptr<CEnvModule> MakeShared(const DrxGUID& guid, const char* szName, const SSourceFileInfo& sourceFileInfo)
{
	return std::make_shared<CEnvModule>(guid, szName, sourceFileInfo);
}

} // EnvModule
} // Schematyc
