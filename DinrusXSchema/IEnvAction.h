// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>

namespace Schematyc
{

// Forward declare classes.
class CAction;
class CActionDesc;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CAction)

struct IEnvAction : public IEnvElementBase<EEnvElementType::Action>
{
	virtual ~IEnvAction() {}

	virtual const CActionDesc& GetDesc() const = 0;
	virtual CActionPtr         CreateFromPool() const = 0;
};

} // Schematyc
