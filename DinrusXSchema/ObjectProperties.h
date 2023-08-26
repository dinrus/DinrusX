// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/IObjectProperties.h>
#include <DinrusX/DinrusXSchema/IRuntimeClass.h>
#include <DinrusX/DinrusXSchema/ITimerSystem.h>
#include <DinrusX/DinrusXSchema/ClassProperties.h>
#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/Scratchpad.h>

namespace Schematyc
{

// Forward declare classes.
class CAnyConstRef;
class CAnyRef;
class CAnyValue;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CAnyValue)

class CObjectProperties final : public IObjectProperties
{
private:

	struct SComponent
	{
		SComponent();
		SComponent(const char* _szName, const CClassProperties& _properties, EOverridePolicy _overridePolicy);
		SComponent(const SComponent& rhs);

		void Serialize(Serialization::IArchive& archive);

		void Edit();
		void Revert();

		string           name;
		CClassProperties properties;
	};

	typedef std::map<DrxGUID, SComponent>                                        Components;
	typedef std::map<const char*, SComponent&, stl::less_stricmp<const char*>> ComponentsByName;

	struct SVariable
	{
		SVariable();
		SVariable(const char* _szName, const CAnyValuePtr& _pValue, EOverridePolicy _overridePolicy);
		SVariable(const SVariable& rhs);

		void Serialize(Serialization::IArchive& archive);

		void Edit();
		void Revert();

		string          name;
		CAnyValuePtr    pValue;
		EOverridePolicy overridePolicy = EOverridePolicy::Default;
	};

	typedef std::map<DrxGUID, SVariable>                                        Variables;
	typedef std::map<const char*, SVariable&, stl::less_stricmp<const char*>> VariablesByName;

public:

	CObjectProperties();
	CObjectProperties(const CObjectProperties& rhs);

	// IObjectProperties
	virtual IObjectPropertiesPtr    Clone() const override;
	virtual void                    Serialize(Serialization::IArchive& archive) override;
	virtual void                    SerializeVariables(Serialization::IArchive& archive) override;
	virtual const CClassProperties* GetComponentProperties(const DrxGUID& guid) const override;
	virtual CClassProperties*       GetComponentProperties(const DrxGUID& guid) override;
	virtual bool                    ReadVariable(const CAnyRef& value, const DrxGUID& guid) const override;

	virtual void AddComponent(const DrxGUID& guid, const char* szName, const CClassProperties& properties) override;
	virtual void AddVariable(const DrxGUID& guid, const char* szName, const CAnyConstRef& value) override;

	virtual bool                    HasVariables() const override { return m_variables.size() > 0; }
	// ~IObjectProperties

private:

	Components m_components;
	Variables  m_variables;
};

} // Schematyc
