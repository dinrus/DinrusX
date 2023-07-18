// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

#include <DinrusX/DinrusXSchema2/GUID.h>

namespace Schematyc2
{
	enum class EEnvElementType
	{
		Function
	};

	struct IEnvElement
	{
		virtual ~IEnvElement() {}

		virtual EEnvElementType GetElementType() const = 0;
		virtual SGUID GetGUID() const = 0;
	};
}
