// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{

// Forward declare classes.
class CClassProperties;

struct IScriptActionInstance : public IScriptElementBase<EScriptElementType::ActionInstance>
{
	virtual ~IScriptActionInstance() {}

	virtual DrxGUID                   GetActionTypeGUID() const = 0;
	virtual DrxGUID                   GetComponentInstanceGUID() const = 0;
	virtual const CClassProperties& GetProperties() const = 0;
};

} // Schematyc
