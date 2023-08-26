// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptModule.h>

#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

namespace Schematyc {

CScriptModule::CScriptModule()
	: CScriptElementBase(EScriptElementFlags::MustOwnScript)
{}

CScriptModule::CScriptModule(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, EScriptElementFlags::MustOwnScript)
{}

void CScriptModule::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptModule::RemapDependencies(IGUIDRemapper& guidRemapper)                                                        {}

void CScriptModule::ProcessEvent(const SScriptEvent& event)                                                               {}

void CScriptModule::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

} // Schematyc
