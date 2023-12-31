// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptInterfaceFunction.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/IScriptView.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

namespace Schematyc
{
CScriptInterfaceFunction::CScriptInterfaceFunction()
	: CScriptElementBase()
{}

CScriptInterfaceFunction::CScriptInterfaceFunction(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName)
{}

void CScriptInterfaceFunction::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	ScriptParam::EnumerateDependencies(m_inputs, enumerator, type);
	ScriptParam::EnumerateDependencies(m_outputs, enumerator, type);
}

void CScriptInterfaceFunction::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	ScriptParam::RemapDependencies(m_inputs, guidRemapper);
	ScriptParam::RemapDependencies(m_outputs, guidRemapper);
}

void CScriptInterfaceFunction::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);

	switch (event.id)
	{
	case EScriptEventId::EditorAdd:
	case EScriptEventId::EditorPaste:
		{
			m_userDocumentation.SetCurrentUserAsAuthor();
			break;
		}
	}
}

void CScriptInterfaceFunction::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

const char* CScriptInterfaceFunction::GetAuthor() const
{
	return m_userDocumentation.author.c_str();
}

const char* CScriptInterfaceFunction::GetDescription() const
{
	return m_userDocumentation.description.c_str();
}

uint32 CScriptInterfaceFunction::GetInputCount() const
{
	return m_inputs.size();
}

const char* CScriptInterfaceFunction::GetInputName(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].name.c_str() : "";
}

CAnyConstPtr CScriptInterfaceFunction::GetInputValue(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].data.GetValue() : CAnyConstPtr();
}

uint32 CScriptInterfaceFunction::GetOutputCount() const
{
	return m_outputs.size();
}

const char* CScriptInterfaceFunction::GetOutputName(uint32 outputIdx) const
{
	return outputIdx < m_outputs.size() ? m_outputs[outputIdx].name.c_str() : "";
}

CAnyConstPtr CScriptInterfaceFunction::GetOutputValue(uint32 outputIdx) const
{
	return outputIdx < m_outputs.size() ? m_outputs[outputIdx].data.GetValue() : CAnyConstPtr();
}

void CScriptInterfaceFunction::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs");
	archive(m_outputs, "outputs");
}
void CScriptInterfaceFunction::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs");
	archive(m_outputs, "outputs");
	archive(m_userDocumentation, "userDocumentation");
}

void CScriptInterfaceFunction::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs");
	archive(m_outputs, "outputs");
	archive(m_userDocumentation, "userDocumentation");
}

void CScriptInterfaceFunction::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	{
		ScriptVariableData::CScopedSerializationConfig serializationConfig(archive);

		const DrxGUID guid = GetGUID();
		serializationConfig.DeclareEnvDataTypes(guid);
		serializationConfig.DeclareScriptEnums(guid);
		serializationConfig.DeclareScriptStructs(guid);

		archive(m_inputs, "inputs", "Inputs");
		archive(m_outputs, "outputs", "Outputs");
	}

	archive(m_userDocumentation, "userDocumentation", "Documentation");
}
} // Schematyc
