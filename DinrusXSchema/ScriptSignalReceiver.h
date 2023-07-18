// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptSignalReceiver.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>
#include <DinrusX/DinrusXSchema/GUID.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
class CScriptSignalReceiver : public CScriptElementBase<IScriptSignalReceiver>, public CMultiPassSerializer
{
public:

	CScriptSignalReceiver();
	CScriptSignalReceiver(const DrxGUID& guid, const char* szName, EScriptSignalReceiverType type, const DrxGUID& signalGUID);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptSignalReceiver
	virtual EScriptSignalReceiverType GetSignalReceiverType() const override;
	virtual DrxGUID                     GetSignalGUID() const override;
	// ~IScriptSignalReceiver

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Validate(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	void CreateGraph();
	void GoToSignal();

private:

	EScriptSignalReceiverType m_type;
	DrxGUID                     m_signalGUID;
	SScriptUserDocumentation  m_userDocumentation;
};
} // Schematyc
