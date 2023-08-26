// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema2/AggregateTypeId.h>

namespace Schematyc2
{
	bool Serialize(Serialization::IArchive& archive, CAggregateTypeId& value, const char* szName, const char* szLabel);
	bool PatchAggregateTypeIdFromDocVariableTypeInfo(Serialization::IArchive& archive, CAggregateTypeId& value, const char* szName);
}
