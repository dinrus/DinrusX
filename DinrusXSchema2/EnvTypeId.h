// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

#include <DinrusX/DinrusXSchema2/TemplateUtils_StringHashWrapper.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_TypeUtils.h>

namespace Schematyc2
{
	typedef TemplateUtils::CStringHashWrapper<TemplateUtils::CFastStringHash, TemplateUtils::CEmptyStringConversion, TemplateUtils::CRawPtrStringStorage> EnvTypeId;

	template <typename TYPE> inline EnvTypeId GetEnvTypeId()
	{
		static const EnvTypeId envTypeId(TemplateUtils::GetTypeName<TYPE>());
		return envTypeId;
	}
}
