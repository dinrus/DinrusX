// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptSignal.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{
CScriptSignal::CScriptSignal()
	: CScriptElementBase(EScriptElementFlags::CanOwnScript)
{}

CScriptSignal::CScriptSignal(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::CanOwnScript)
{}

void CScriptSignal::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	ScriptParam::EnumerateDependencies(m_inputs, enumerator, type);
}

void CScriptSignal::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	ScriptParam::RemapDependencies(m_inputs, guidRemapper);
}

void CScriptSignal::ProcessEvent(const SScriptEvent& event)
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

void CScriptSignal::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

const char* CScriptSignal::GetAuthor() const
{
	return m_userDocumentation.author.c_str();
}

const char* CScriptSignal::GetDescription() const
{
	return m_userDocumentation.description.c_str();
}

uint32 CScriptSignal::GetInputCount() const
{
	return m_inputs.size();
}

DrxGUID CScriptSignal::GetInputGUID(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].guid : DrxGUID();
}

const char* CScriptSignal::GetInputName(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].name.c_str() : "";
}

SElementId CScriptSignal::GetInputTypeId(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].data.GetTypeId() : SElementId();
}

CAnyConstPtr CScriptSignal::GetInputData(uint32 inputIdx) const
{
	return inputIdx < m_inputs.size() ? m_inputs[inputIdx].data.GetValue() : CAnyConstPtr();
}

void CScriptSignal::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs", "Inputs");
}

void CScriptSignal::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs", "Inputs");
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptSignal::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_inputs, "inputs");
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptSignal::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	{
		ScriptVariableData::CScopedSerializationConfig serializationConfig(archive);

		const DrxGUID guid = GetGUID();
		serializationConfig.DeclareEnvDataTypes(guid);
		serializationConfig.DeclareScriptEnums(guid);
		serializationConfig.DeclareScriptStructs(guid);

		archive(m_inputs, "inputs", "Inputs");
	}

	archive(m_userDocumentation, "userDocumentation", "Documentation");
}
} // Schematyc
