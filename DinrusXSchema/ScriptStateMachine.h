// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptStateMachine.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>

namespace Schematyc
{
class CScriptStateMachine : public CScriptElementBase<IScriptStateMachine>, public CMultiPassSerializer
{
public:

	CScriptStateMachine();
	CScriptStateMachine(const DrxGUID& guid, const char* szName, EScriptStateMachineLifetime lifetime, const DrxGUID& contextGUID, const DrxGUID& partnerGUID);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptStateMachine
	virtual EScriptStateMachineLifetime GetLifetime() const override;
	virtual DrxGUID                       GetContextGUID() const override;
	virtual DrxGUID                       GetPartnerGUID() const override;
	// ~IScriptStateMachine

protected:

	// CMultiPassSerializer
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	void CreateTransitionGraph();
	void RefreshTransitionGraph();

private:

	EScriptStateMachineLifetime m_lifetime;
	DrxGUID                       m_contextGUID;
	DrxGUID                       m_partnerGUID;
	DrxGUID                       m_transitionGraphGUID;
};
} // Schematyc
