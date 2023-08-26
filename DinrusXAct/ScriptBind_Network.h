// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Binding of network functions into script

   -------------------------------------------------------------------------
   History:
   - 24:11:2004   11:30 : Created by Craig Tiller

*************************************************************************/
#ifndef __SCRIPTBIND_NETWORK_H__
#define __SCRIPTBIND_NETWORK_H__

#pragma once

#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXScript/ScriptHelpers.h>

class CGameContext;
class CDrxAction;

class CScriptBind_Network :
	public CScriptableBase
{
public:
	CScriptBind_Network(ISystem* pSystem, CDrxAction* pFW);
	virtual ~CScriptBind_Network();

	void Release() { delete this; };

	//! <code>Network.Expose()</code>
	int Expose(IFunctionHandler* pFH);

	//! <code>Network.DelegateAuthority(ent, channel)</code>
	//!		<param name="ent">Entity identifier.</param>
	//!		<param name="channel">Network channel.</param>
	//! <description>Delegate authority for an object to some client.</description>
	int          DelegateAuthority(IFunctionHandler* pFH, ScriptHandle ent, int channel);

	virtual void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
	}
private:
	void RegisterGlobals();
	void RegisterMethods();

	ISystem*    m_pSystem;
	CDrxAction* m_pFW;
};

#endif
