// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>
#include <DinrusX/DinrusXSchema2/ScriptRoot.h>

namespace Schematyc2
{
	CScriptRoot::CScriptRoot(IScriptFile& file)
		: CScriptElementBase(EScriptElementType::Root, file)
	{}

	EAccessor CScriptRoot::GetAccessor() const
	{
		return EAccessor::Private;
	}

	SGUID CScriptRoot::GetGUID() const
	{
		return SGUID();
	}

	SGUID CScriptRoot::GetScopeGUID() const
	{
		return SGUID();
	}

	bool CScriptRoot::SetName(const char* szName)
	{
		return false;
	}

	const char* CScriptRoot::GetName() const
	{
		return "Root";
	}

	void CScriptRoot::EnumerateDependencies(const ScriptDependancyEnumerator& enumerator) const {}

	void CScriptRoot::Refresh(const SScriptRefreshParams& params) {}

	void CScriptRoot::Serialize(Serialization::IArchive& archive) {}

	void CScriptRoot::RemapGUIDs(IGUIDRemapper& guidRemapper) {}
}
