// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Script Bind for CActionMapUpr

   -------------------------------------------------------------------------
   History:
   - 8:11:2004   16:48 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_ACTIONMAPMANAGER_H__
#define __SCRIPTBIND_ACTIONMAPMANAGER_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXScript/ScriptHelpers.h>

class CActionMapUpr;

class CScriptBind_ActionMapUpr :
	public CScriptableBase
{
public:
	CScriptBind_ActionMapUpr(ISystem* pSystem, CActionMapUpr* pActionMapUpr);
	virtual ~CScriptBind_ActionMapUpr();

	void         Release() { delete this; };

	virtual void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
	}

	//! <code>ActionMapUpr.EnableActionFilter( name, enable )</code>
	//!		<param name="name">Filter name.</param>
	//!		<param name="enable">True to enable the filter, false otherwise.</param>
	//! <description>Enables a filter for the actions.</description>
	int EnableActionFilter(IFunctionHandler* pH, const char* name, bool enable);

	//! <code>ActionMapUpr.EnableActionMap( name, enable )</code>
	//!		<param name="name">Action map name.</param>
	//!		<param name="enable">True to enable the filter, false otherwise.</param>
	//! <description>Enables an action map.</description>
	int EnableActionMap(IFunctionHandler* pH, const char* name, bool enable);

	//! <code>ActionMapUpr.EnableActionMapUpr( enable, resetStateOnDisable )</code>
	//!		<param name="enable">Enables/Disables ActionMapUpr.</param>
	//!		<param name="resetStateOnDisable">Resets the different Action states when ActionMapUpr gets disabled.</param>
	//! <description>Enables/Disables ActionMapUpr.</description>
	int EnableActionMapUpr(IFunctionHandler* pH, bool enable, bool resetStateOnDisable);

	//! <code>ActionMapUpr.LoadFromXML( name )</code>
	//!		<param name="name">XML file name.</param>
	//! <description>Loads information from an XML file.</description>
	int LoadFromXML(IFunctionHandler* pH, const char* name);

	//! <code>ActionMapUpr.InitActionMaps( path )</code>
	//!		<param name="path">XML file path.</param>
	//! <description>Initializes the action maps and filters found in given file</description>
	int InitActionMaps(IFunctionHandler* pH, const char* path);

	//! <code>ActionMapUpr.SetDefaultActionEntity( id, updateAll )</code>
	//!		<param name="id">EntityId of the new default action entity.</param>
	//!		<param name="updateAll">Updates all existing action map assignments.</param>
	//! <description>Sets the new default entity.</description>
	int SetDefaultActionEntity(IFunctionHandler* pH, EntityId id, bool updateAll);

	//! <code>ActionMapUpr.GetDefaultActionEntity()</code>
	//! <description>Gets the currently set default action entity.</description>
	int GetDefaultActionEntity(IFunctionHandler* pH);

	//! <code>ActionMapUpr.LoadControllerLayoutFile( layoutName )</code>
	//!		<param name="layoutName">layout name.</param>
	//! <description>Loads the given controller layout into the action manager.</description>
	int LoadControllerLayoutFile(IFunctionHandler* pH, const char* layoutName);

	//! <code>ActionMapUpr.IsFilterEnabled( filterName )</code>
	//!		<param name="filterName">filter name.</param>
	//! <description>Checks if a filter is currently enabled.</description>
	int IsFilterEnabled(IFunctionHandler* pH, const char* filterName);

private:
	void RegisterGlobals();
	void RegisterMethods();

	ISystem*           m_pSystem;
	CActionMapUpr* m_pUpr;
};

#endif //__SCRIPTBIND_ACTIONMAPMANAGER_H__
