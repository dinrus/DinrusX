// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>
#include <DinrusX/DinrusXSchema2/ScriptInclude.h>

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
	CScriptInclude::CScriptInclude(IScriptFile& file, const SGUID& guid, const SGUID& scopeGUID, const char* szFileName, const SGUID& refGUID)
		: CScriptElementBase(EScriptElementType::Include, file)
		, m_guid(guid)
		, m_scopeGUID(scopeGUID)
		, m_fileName(szFileName)
		, m_refGUID(refGUID)
	{}

	//////////////////////////////////////////////////////////////////////////
	EAccessor CScriptInclude::GetAccessor() const
	{
		return EAccessor::Private;
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptInclude::GetGUID() const
	{
		return m_guid;
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptInclude::GetScopeGUID() const
	{
		return m_scopeGUID;
	}

	//////////////////////////////////////////////////////////////////////////
	bool CScriptInclude::SetName(const char* szName)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	const char* CScriptInclude::GetName() const
	{
		return m_fileName.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	void CScriptInclude::EnumerateDependencies(const ScriptDependancyEnumerator& enumerator) const {}

	//////////////////////////////////////////////////////////////////////////
	void CScriptInclude::Refresh(const SScriptRefreshParams& params) {}

	//////////////////////////////////////////////////////////////////////////
	void CScriptInclude::Serialize(Serialization::IArchive& archive)
	{
		LOADING_TIME_PROFILE_SECTION;
		
		CScriptElementBase::Serialize(archive);

		SerializationContext::SetValidatorLink(archive, SValidatorLink(m_guid)); // #SchematycTODO : Can we set this from CScriptElementBase?
		switch(SerializationContext::GetPass(archive))
		{
		case ESerializationPass::PreLoad:
			{
				archive(m_guid, "guid");
				archive(m_scopeGUID, "scope_guid");
				archive(m_fileName, "fileName");
				archive(m_refGUID, "refGUID");

				////////////////////////////////////////////////////////////////////////////////////////////////////
				// Temporary fix to ensure files load in the correct order.
				// Ideally include dependencies should be resolved by the doc manager.
				IScriptRegistry&   scriptRegistry = gEnv->pSchematyc2->GetScriptRegistry();
				const IScriptFile* pRefFile = scriptRegistry.GetFile(m_refGUID);
				if(!pRefFile)
				{
					pRefFile = scriptRegistry.LoadFile(m_fileName.c_str());
					if(pRefFile)
					{
						m_refGUID = pRefFile->GetGUID();
					}
				}
				//////////////////////////////////////////////////
				break;
			}
		case ESerializationPass::Save:
			{
				archive(m_guid, "guid");
				archive(m_scopeGUID, "scope_guid");
				archive(m_fileName, "fileName");
				archive(m_refGUID, "refGUID");
				break;
			}
		case ESerializationPass::Edit:
			{
				archive(m_fileName, "fileName", "!FileName");
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void CScriptInclude::RemapGUIDs(IGUIDRemapper& guidRemapper)
	{
		m_guid      = guidRemapper.Remap(m_guid);
		m_scopeGUID = guidRemapper.Remap(m_scopeGUID);
		m_refGUID   = guidRemapper.Remap(m_refGUID);
	}

	//////////////////////////////////////////////////////////////////////////
	const char* CScriptInclude::GetFileName() const
	{
		return m_fileName.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	SGUID CScriptInclude::GetRefGUID() const
	{
		return m_refGUID;
	}
}
