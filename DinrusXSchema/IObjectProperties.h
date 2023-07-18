// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IObjectProperties;
// Forward declare classes.
class CAnyRef;
class CAnyConstRef;
class CClassProperties;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(IObjectProperties)

struct IObjectProperties
{
	virtual ~IObjectProperties() {}

	virtual IObjectPropertiesPtr    Clone() const = 0;
	virtual void                    Serialize(Serialization::IArchive& archive) = 0;
	virtual void                    SerializeVariables(Serialization::IArchive& archive) = 0;
	virtual const CClassProperties* GetComponentProperties(const DrxGUID& guid) const = 0;
	virtual CClassProperties*       GetComponentProperties(const DrxGUID& guid) = 0;
	virtual bool                    ReadVariable(const CAnyRef& value, const DrxGUID& guid) const = 0;

	virtual void                    AddComponent(const DrxGUID& guid, const char* szName, const CClassProperties& properties) = 0;
	virtual void                    AddVariable(const DrxGUID& guid, const char* szName, const CAnyConstRef& value) = 0;

	virtual bool                    HasVariables() const = 0;
};

} // Schematyc
