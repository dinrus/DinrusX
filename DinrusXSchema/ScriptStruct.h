// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptStruct.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptParam.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

class CScriptStruct : public CScriptElementBase<IScriptStruct>, public CMultiPassSerializer
{
public:

	CScriptStruct();
	CScriptStruct(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptStruct
	virtual uint32       GetFieldCount() const override;
	virtual const char*  GetFieldName(uint32 fieldIdx) const override;
	virtual CAnyConstPtr GetFieldValue(uint32 fieldIdx) const override;
	// ~IScriptStruct

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	ScriptParams             m_fields;
	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
