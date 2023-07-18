// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ScriptBind_DialogSystem.cpp
//  Version:     v1.00
//  Created:     02/08/2006 by AlexL
//  Compilers:   Visual Studio.NET
//  Описание: Dialog System ScriptBinding
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __SCRIPTBIND_DIALOGSYSTEM_H__
#define __SCRIPTBIND_DIALOGSYSTEM_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXScript/ScriptHelpers.h>

class CDialogSystem;

class CScriptBind_DialogSystem : public CScriptableBase
{
public:
	CScriptBind_DialogSystem(ISystem* pSystem, CDialogSystem* pDS);
	virtual ~CScriptBind_DialogSystem();

	void         Release() { delete this; };

	virtual void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
	}
private:
	void RegisterGlobals();
	void RegisterMethods();

	int  CreateSession(IFunctionHandler* pH, const char* scriptID);
	int  DeleteSession(IFunctionHandler* pH, int sessionID);
	int  SetActor(IFunctionHandler* pH, int sessionID, int actorID, ScriptHandle entity);
	int  Play(IFunctionHandler* pH, int sessionID);
	int  Stop(IFunctionHandler* pH, int sessionID);
	int  IsEntityInDialog(IFunctionHandler* pH, ScriptHandle entity);

private:
	ISystem*       m_pSystem;
	IEntitySystem* m_pEntitySystem;
	CDialogSystem* m_pDS;
};

#endif //__SCRIPTBIND_DIALOGSYSTEM_H__
