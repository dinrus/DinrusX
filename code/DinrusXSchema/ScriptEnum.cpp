// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptEnum.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

namespace Schematyc
{
CScriptEnum::CScriptEnum()
	: CScriptElementBase(EScriptElementFlags::CanOwnScript)
{}

CScriptEnum::CScriptEnum(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::CanOwnScript)
{}

void CScriptEnum::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptEnum::RemapDependencies(IGUIDRemapper& guidRemapper) {}

void CScriptEnum::ProcessEvent(const SScriptEvent& event)
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

void CScriptEnum::Serialize(Serialization::IArchive& archive)
{
	LOADING_TIME_PROFILE_SECTION;

	CScriptElementBase::Serialize(archive);

	switch (SerializationContext::GetPass(archive))
	{
	case ESerializationPass::LoadDependencies:
	case ESerializationPass::Save:
		{
			archive(m_constants, "constants", "Constants");
			archive(m_userDocumentation, "userDocumentation", "Documentation");
			break;
		}
	case ESerializationPass::Edit:
		{
			archive(m_constants, "constants", "Constants");
			archive(m_userDocumentation, "userDocumentation", "Documentation");
			break;
		}
	}

	CScriptElementBase::SerializeExtensions(archive);
}

uint32 CScriptEnum::GetConstantCount() const
{
	return m_constants.size();
}

uint32 CScriptEnum::FindConstant(const char* szConstant) const
{
	SCHEMATYC_CORE_ASSERT(szConstant);
	if (szConstant)
	{
		for (Constants::const_iterator itConstant = m_constants.begin(), itEndConstant = m_constants.end(); itConstant != itEndConstant; ++itConstant)
		{
			if (strcmp(itConstant->c_str(), szConstant) == 0)
			{
				return static_cast<uint32>(itConstant - m_constants.begin());
			}
		}
	}
	return InvalidIdx;
}

const char* CScriptEnum::GetConstant(uint32 constantIdx) const
{
	return constantIdx < m_constants.size() ? m_constants[constantIdx].c_str() : "";
}
} // Schematyc
