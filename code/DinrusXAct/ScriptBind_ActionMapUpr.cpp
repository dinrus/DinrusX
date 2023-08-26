// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$

   -------------------------------------------------------------------------
   History:
   - 8:11:2004   16:49 : Created by Márcio Martins

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/ScriptBind_ActionMapUpr.h>
#include <DinrusX/DinrusXAct/ActionMapUpr.h>

//------------------------------------------------------------------------
CScriptBind_ActionMapUpr::CScriptBind_ActionMapUpr(ISystem* pSystem, CActionMapUpr* pActionMapUpr)
	: m_pSystem(pSystem),
	m_pUpr(pActionMapUpr)
{
	Init(gEnv->pScriptSystem, m_pSystem);
	SetGlobalName("ActionMapUpr");

	RegisterGlobals();
	RegisterMethods();
}

//------------------------------------------------------------------------
CScriptBind_ActionMapUpr::~CScriptBind_ActionMapUpr()
{
}

//------------------------------------------------------------------------
void CScriptBind_ActionMapUpr::RegisterGlobals()
{
}

//------------------------------------------------------------------------
void CScriptBind_ActionMapUpr::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_ActionMapUpr::

	SCRIPT_REG_TEMPLFUNC(EnableActionFilter, "name, enable");
	SCRIPT_REG_TEMPLFUNC(EnableActionMap, "name, enable");
	SCRIPT_REG_TEMPLFUNC(EnableActionMapUpr, "enable, resetStateOnDisable");
	SCRIPT_REG_TEMPLFUNC(LoadFromXML, "name");
	SCRIPT_REG_TEMPLFUNC(InitActionMaps, "path");
	SCRIPT_REG_TEMPLFUNC(SetDefaultActionEntity, "id, updateAll");
	SCRIPT_REG_TEMPLFUNC(GetDefaultActionEntity, "");
	SCRIPT_REG_TEMPLFUNC(LoadControllerLayoutFile, "layoutName");
	SCRIPT_REG_TEMPLFUNC(IsFilterEnabled, "filterName");
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::EnableActionFilter(IFunctionHandler* pH, const char* name, bool enable)
{
	m_pUpr->EnableFilter(name, enable);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::EnableActionMap(IFunctionHandler* pH, const char* name, bool enable)
{
	m_pUpr->EnableActionMap(name, enable);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::EnableActionMapUpr(IFunctionHandler* pH, bool enable, bool resetStateOnDisable)
{
	m_pUpr->Enable(enable, resetStateOnDisable);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::LoadFromXML(IFunctionHandler* pH, const char* name)
{
	XmlNodeRef rootNode = m_pSystem->LoadXmlFromFile(name);
	m_pUpr->LoadFromXML(rootNode);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::InitActionMaps(IFunctionHandler* pH, const char* path)
{
	m_pUpr->InitActionMaps(path);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::SetDefaultActionEntity(IFunctionHandler* pH, EntityId id, bool updateAll)
{
	m_pUpr->SetDefaultActionEntity(id, updateAll);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::GetDefaultActionEntity(IFunctionHandler* pH)
{
	return pH->EndFunction(m_pUpr->GetDefaultActionEntity());
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::LoadControllerLayoutFile(IFunctionHandler* pH, const char* layoutName)
{
	return pH->EndFunction(m_pUpr->LoadControllerLayoutFile(layoutName));
}

//------------------------------------------------------------------------
int CScriptBind_ActionMapUpr::IsFilterEnabled(IFunctionHandler* pH, const char* filterName)
{
	return pH->EndFunction(m_pUpr->IsFilterEnabled(filterName));
}
