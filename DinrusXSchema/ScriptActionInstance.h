// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptActionInstance.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>
#include <DinrusX/DinrusXSchema/ClassProperties.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>

namespace Schematyc
{

class CScriptActionInstance : public CScriptElementBase<IScriptActionInstance>, public CMultiPassSerializer
{
public:

	CScriptActionInstance();
	CScriptActionInstance(const DrxGUID& guid, const char* szName, const DrxGUID& actionTypeGUID, const DrxGUID& componentInstanceGUID);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptActionInstance
	virtual DrxGUID                   GetActionTypeGUID() const override;
	virtual DrxGUID                   GetComponentInstanceGUID() const override;
	virtual const CClassProperties& GetProperties() const override;
	// ~IScriptActionInstance

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	void RefreshProperties();
	void SerializeProperties(Serialization::IArchive& archive);

private:

	DrxGUID            m_actionTypeGUID;
	DrxGUID            m_componentInstanceGUID;
	CClassProperties m_properties;
};

} // Schematyc
