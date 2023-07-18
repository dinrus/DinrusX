// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptSignal.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptParam.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

class CScriptSignal : public CScriptElementBase<IScriptSignal>, public CMultiPassSerializer
{
public:

	CScriptSignal();
	CScriptSignal(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptSignal
	virtual const char*  GetAuthor() const override;
	virtual const char*  GetDescription() const override;
	virtual uint32       GetInputCount() const override;
	virtual DrxGUID        GetInputGUID(uint32 inputIdx) const override;
	virtual const char*  GetInputName(uint32 inputIdx) const override;
	virtual SElementId   GetInputTypeId(uint32 inputIdx) const override;
	virtual CAnyConstPtr GetInputData(uint32 inputIdx) const override;
	// ~IScriptSignal

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	ScriptParams             m_inputs;
	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
