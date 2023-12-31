// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptTimer.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>
#include <DinrusX/DinrusXSchema/ITimerSystem.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
class CScriptTimer : public CScriptElementBase<IScriptTimer>, public CMultiPassSerializer
{
public:

	CScriptTimer();
	CScriptTimer(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptTimer
	virtual const char*  GetAuthor() const override;
	virtual const char*  GetDescription() const override;
	virtual STimerParams GetParams() const override;
	// ~IScriptTimer

protected:

	// CMultiPassSerializer
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	void SerializeParams(Serialization::IArchive& archive);
	void ValidateDuration(STimerDuration& duration, Serialization::IArchive* pArchive, bool bApplyCorrections) const;

private:

	STimerParams             m_params;
	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
