// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptStruct.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{
CScriptStruct::CScriptStruct()
	: CScriptElementBase(EScriptElementFlags::CanOwnScript)
{}

CScriptStruct::CScriptStruct(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::CanOwnScript)
{}

void CScriptStruct::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	ScriptParam::EnumerateDependencies(m_fields, enumerator, type);
}

void CScriptStruct::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	ScriptParam::RemapDependencies(m_fields, guidRemapper);
}

void CScriptStruct::ProcessEvent(const SScriptEvent& event)
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

void CScriptStruct::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

uint32 CScriptStruct::GetFieldCount() const
{
	return m_fields.size();
}

const char* CScriptStruct::GetFieldName(uint32 fieldIdx) const
{
	return fieldIdx < m_fields.size() ? m_fields[fieldIdx].name : "";
}

CAnyConstPtr CScriptStruct::GetFieldValue(uint32 fieldIdx) const
{
	return fieldIdx < m_fields.size() ? m_fields[fieldIdx].data.GetValue() : CAnyConstPtr();
}

void CScriptStruct::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_fields, "fields");
}

void CScriptStruct::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_fields, "fields");
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptStruct::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_fields, "fields");
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}

void CScriptStruct::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	{
		ScriptVariableData::CScopedSerializationConfig serializationConfig(archive);

		const DrxGUID guid = GetGUID();
		serializationConfig.DeclareEnvDataTypes(guid);
		serializationConfig.DeclareScriptEnums(guid);

		archive(m_fields, "fields", "Fields");
	}

	archive(m_userDocumentation, "userDocumentation", "Documentation");
}
} // Schematyc
