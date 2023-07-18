// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptInterfaceFunction.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptParam.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
// Forward declare classes.
class CAnyConstPtr;

class CScriptInterfaceFunction : public CScriptElementBase<IScriptInterfaceFunction>, public CMultiPassSerializer
{
public:

	CScriptInterfaceFunction();
	CScriptInterfaceFunction(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

	// IScriptInterfaceFunction
	virtual const char*  GetAuthor() const override;
	virtual const char*  GetDescription() const override;
	virtual uint32       GetInputCount() const override;
	virtual const char*  GetInputName(uint32 inputIdx) const override;
	virtual CAnyConstPtr GetInputValue(uint32 inputIdx) const override;
	virtual uint32       GetOutputCount() const override;
	virtual const char*  GetOutputName(uint32 outputIdx) const override;
	virtual CAnyConstPtr GetOutputValue(uint32 outputIdx) const override;
	// ~IScriptInterfaceFunction

protected:

	// CMultiPassSerializer
	virtual void LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Load(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Save(Serialization::IArchive& archive, const ISerializationContext& context) override;
	virtual void Edit(Serialization::IArchive& archive, const ISerializationContext& context) override;
	// ~CMultiPassSerializer

private:

	ScriptParams             m_inputs;
	ScriptParams             m_outputs;
	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
