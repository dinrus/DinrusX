// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptState.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>

namespace Schematyc
{
class CScriptState : public CScriptElementBase<IScriptState>, public CMultiPassSerializer
{
public:

	CScriptState();
	CScriptState(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptState
	virtual DrxGUID GetPartnerGUID() const override;
	// ~IScriptState

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	DrxGUID m_partnerGUID;
};
} // Schematyc
