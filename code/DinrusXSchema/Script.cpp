// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/Script.h>

#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXSchema/ICore.h>
#include <DinrusX/DinrusXSchema/IScriptElement.h>
#include <DinrusX/DinrusXSchema/SerializationToString.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/StringUtils.h>

#include <DinrusX/DinrusXSchema/IScriptRegistry.h>

#include <DinrusX/DinrusXSchema/ScriptSerializers.h>
#include <DinrusX/DinrusXSchema/ScriptRegistry.h>

namespace Schematyc
{
CScript::CScript(const DrxGUID& guid, const char* szFilePath)
	: m_guid(guid)
	, m_filePath(szFilePath)
	, m_timeStamp(gEnv->pTimer->GetAsyncTime())
	, m_pRoot(nullptr)
{}

CScript::CScript(const char* szFilePath)
	: m_guid()
	, m_filePath(szFilePath)
	, m_timeStamp(gEnv->pTimer->GetAsyncTime())
	, m_pRoot(nullptr)
{

}

DrxGUID CScript::GetGUID() const
{
	return m_guid;
}

const char* CScript::SetFilePath(const char* szFilePath)
{
	if (szFilePath)
	{
		stack_string gameFolder = gEnv->pDrxPak->GetGameFolder();
		gameFolder.MakeLower();
		stack_string filePath = szFilePath;
		filePath.MakeLower();

		if (filePath.find(gameFolder.c_str()) != 0)
		{

			filePath = gEnv->pDrxPak->GetGameFolder();
			filePath.append("/");
			filePath.append(szFilePath);

			switch (m_pRoot->GetType())
			{
			case EScriptElementType::Class:
				{
					filePath.append(".schematyc_ent");
					break;
				}
			default:
				{
					filePath.append(".schematyc_lib");
					break;
				}
			}
			filePath.MakeLower();
		}

		m_filePath = filePath.c_str();
	}

	return m_filePath.c_str();
}

const CTimeValue& CScript::GetTimeStamp() const
{
	return m_timeStamp;
}

void CScript::SetRoot(IScriptElement* pRoot)
{
	m_pRoot = pRoot;
}

IScriptElement* CScript::GetRoot()
{
	return m_pRoot;
}

EVisitStatus CScript::VisitElements(const ScriptElementVisitor& visitor)
{
	SCHEMATYC_CORE_ASSERT(visitor);
	if (m_pRoot && visitor)
	{
		return VisitElementsRecursive(visitor, *m_pRoot);
	}
	return EVisitStatus::Stop;
}

EVisitStatus CScript::VisitElementsRecursive(const ScriptElementVisitor& visitor, IScriptElement& element)
{
	EVisitStatus visitStatus = visitor(element);
	if (visitStatus == EVisitStatus::Continue)
	{
		for (IScriptElement* pChild = element.GetFirstChild(); pChild; pChild = pChild->GetNextSibling())
		{
			if (!pChild->GetScript())
			{
				visitStatus = VisitElementsRecursive(visitor, *pChild);
				if (visitStatus == EVisitStatus::Stop)
				{
					break;
				}
			}
		}
	}
	return visitStatus;
}

void CScript::SetNameFromRootRecursive(CStackString& name, IScriptElement& element)
{
	IScriptElement* pParent = element.GetParent();
	if (pParent && (pParent->GetType() != EScriptElementType::Root))
	{
		SetNameFromRootRecursive(name, *pParent);
		name.append("/");
	}
	name.append(element.GetName());
}
}
