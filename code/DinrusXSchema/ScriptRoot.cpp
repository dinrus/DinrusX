// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptRoot.h>

namespace Schematyc
{
CScriptRoot::CScriptRoot()
	: CScriptElementBase(DrxGUID(), "Root", EScriptElementFlags::FixedName)
{}

void CScriptRoot::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptRoot::RemapDependencies(IGUIDRemapper& guidRemapper)                                                        {}

void CScriptRoot::ProcessEvent(const SScriptEvent& event)                                                               {}

void CScriptRoot::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}
} // Schematyc
