// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptInterfaceTask.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/IEnvInterface.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

namespace Schematyc
{
CScriptInterfaceTask::CScriptInterfaceTask() {}

CScriptInterfaceTask::CScriptInterfaceTask(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName)
{}

void CScriptInterfaceTask::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptInterfaceTask::RemapDependencies(IGUIDRemapper& guidRemapper)                                                        {}

void CScriptInterfaceTask::ProcessEvent(const SScriptEvent& event)
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

void CScriptInterfaceTask::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

const char* CScriptInterfaceTask::GetAuthor() const
{
	return m_userDocumentation.author.c_str();
}

const char* CScriptInterfaceTask::GetDescription() const
{
	return m_userDocumentation.description.c_str();
}

void CScriptInterfaceTask::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation");
}

void CScriptInterfaceTask::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation");
}

void CScriptInterfaceTask::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation", "Documentation");
}
} // Schematyc
