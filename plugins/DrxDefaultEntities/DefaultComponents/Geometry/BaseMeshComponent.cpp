// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#include "StdAfx.h"
#include "BaseMeshComponent.h"
<DinrusX/CoreX/Serialization/Enum.h>

namespace Drx
{
namespace DefaultComponents
{
	YASLI_ENUM_BEGIN_NESTED(SPhysicsParameters, EWeightType, "WeightType")
	YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Mass, "Mass")
	YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Density, "Density")
	YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Immovable, "Immovable")
	YASLI_ENUM_END()
}
}
