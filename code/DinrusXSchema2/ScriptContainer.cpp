// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>
#include <DinrusX/DinrusXSchema2/ScriptContainer.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema2/GUIDRemapper.h>
#include <DinrusX/DinrusXSchema2/Deprecated/DocUtils.h>
#include <DinrusX/DinrusXSchema2/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema2/ISerializationContext.h>
#include <DinrusX/DinrusXSchema2/SerializationUtils.h>

namespace Schematyc2
{
	//////////////////////////////////////////////////////////////////////////
	CScriptContainer::CScriptContainer(IScriptFile& file, const SGUID& guid, const SGUID& scopeGUID, const char* szName, const SGUID& typeGUID)
		: CScriptElementBase(EScriptElementType::Container, file)
		, m_guid(guid)
		, m_scopeGUID(scopeGUID)
		, m_name(szName)
		, m_typeGUID(typeGUID)
	{}

	//////////////////////////////////////////////////////////////////////////
	EAccessor CScriptContainer::GetAccessor() const
	{
		return EAccessor::Private;
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptContainer::GetGUID() const
	{
		return m_guid;
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptContainer::GetScopeGUID() const
	{
		return m_scopeGUID;
	}

	//////////////////////////////////////////////////////////////////////////
	bool CScriptContainer::SetName(const char* szName)
	{
		m_name = szName;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	const char* CScriptContainer::GetName() const
	{
		return m_name.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	void CScriptContainer::EnumerateDependencies(const ScriptDependancyEnumerator& enumerator) const {}

	//////////////////////////////////////////////////////////////////////////
	void CScriptContainer::Refresh(const SScriptRefreshParams& params) {}

	//////////////////////////////////////////////////////////////////////////
	void CScriptContainer::Serialize(Serialization::IArchive& archive)
	{
		LOADING_TIME_PROFILE_SECTION;
		
		CScriptElementBase::Serialize(archive);

		SerializationContext::SetValidatorLink(archive, SValidatorLink(m_guid)); // #SchematycTODO : Can we set this from CScriptElementBase?
		switch(SerializationContext::GetPass(archive))
		{
		case ESerializationPass::PreLoad:
		case ESerializationPass::Save:
			{
				archive(m_guid, "guid");
				archive(m_scopeGUID, "scope_guid");
				archive(m_name, "name");
				archive(m_typeGUID, "type_guid");
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void CScriptContainer::RemapGUIDs(IGUIDRemapper& guidRemapper)
	{
		m_guid      = guidRemapper.Remap(m_guid);
		m_scopeGUID = guidRemapper.Remap(m_scopeGUID);
		m_typeGUID  = guidRemapper.Remap(m_typeGUID);
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptContainer::GetTypeGUID() const
	{
		return m_typeGUID;
	}
}
