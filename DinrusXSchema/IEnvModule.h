// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>

namespace Schematyc
{

struct IEnvModule : public IEnvElementBase<EEnvElementType::Module>
{
	virtual ~IEnvModule() {}
};

} // Schematyc
