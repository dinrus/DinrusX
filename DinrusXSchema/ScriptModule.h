// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptModule.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>

namespace Schematyc
{
class CScriptModule : public CScriptElementBase<IScriptModule>, public CMultiPassSerializer
{
public:

	CScriptModule();
	CScriptModule(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

protected:
	// CMultiPassSerializer
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override { CMultiPassSerializer::Save(archive, context); }
	// ~CMultiPassSerializer
};
} // Schematyc
