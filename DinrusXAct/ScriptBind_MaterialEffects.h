// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ScriptBind_MaterialEffects.cpp
//  Version:     v1.00
//  Created:     03/22/2007 by MichaelR
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: MaterialEffects ScriptBind
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __SCRIPTBIND_MaterialEffects_H__
#define __SCRIPTBIND_MaterialEffects_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXScript/ScriptHelpers.h>

class CMaterialEffects;

class CScriptBind_MaterialEffects : public CScriptableBase
{
public:
	CScriptBind_MaterialEffects(ISystem* pSystem, CMaterialEffects* pDS);
	virtual ~CScriptBind_MaterialEffects();

	void         Release() { delete this; };

	virtual void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
	}
private:
	void RegisterGlobals();
	void RegisterMethods();

	int  GetEffectId(IFunctionHandler* pH, const char* customName, int surfaceIndex2);
	int  GetEffectIdByLibName(IFunctionHandler* pH, const char* LibName, const char* FGFXName);
	int  PrintEffectIdByMatIndex(IFunctionHandler* pH, int materialIndex1, int materialIndex2);
	int  ExecuteEffect(IFunctionHandler* pH, int effectId, SmartScriptTable paramsTable);

private:
	ISystem*          m_pSystem;
	CMaterialEffects* m_pMFX;
};

#endif //__SCRIPTBIND_MaterialEffects_H__
