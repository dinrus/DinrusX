// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvElement.h>
#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IObject;
struct IObjectPreviewer;
// Forward declare classes.
class CAnyConstPtr;

struct IEnvClass : public IEnvElementBase<EEnvElementType::Class>
{
	virtual ~IEnvClass() {}

	virtual uint32            GetInterfaceCount() const = 0;
	virtual DrxGUID             GetInterfaceGUID(uint32 interfaceIdx) const = 0;
	virtual uint32            GetComponentCount() const = 0;
	virtual DrxGUID             GetComponentTypeGUID(uint32 componentIdx) const = 0;
	virtual CAnyConstPtr      GetProperties() const = 0;
	virtual IObjectPreviewer* GetPreviewer() const = 0;
};

} // Schematyc
