// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvClass.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>
#include <DinrusX/DinrusXSchema/IScriptClass.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
class CScriptClass : public CScriptElementBase<IScriptClass>, public CMultiPassSerializer
{
public:

	CScriptClass();
	CScriptClass(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptClass
	virtual const char* GetAuthor() const override;
	virtual const char* GetDescription() const override;
	// ~IScriptClass

protected:

	// CMultiPassSerializer
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	SScriptUserDocumentation m_userDocumentation;
};
}
