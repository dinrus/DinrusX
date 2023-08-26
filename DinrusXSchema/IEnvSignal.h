// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>

namespace Schematyc
{

// Forward declare classes.
class CClassDesc;

struct IEnvSignal : public IEnvElementBase<EEnvElementType::Signal>
{
	virtual ~IEnvSignal() {}

	virtual const CClassDesc& GetDesc() const = 0;
};

} // Schematyc
