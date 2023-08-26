// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ScriptBind_MaterialEffects.cpp
//  Version:     v1.00
//  Created:     03/22/2007 by MichaelR
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: MaterialEffects ScriptBind. MaterialEffects should be used
//               from C++ if possible. Use this ScriptBind for legacy stuff.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/ScriptBind_MaterialEffects.h>
#include <DinrusX/DinrusXAct/MaterialEffects.h>

//------------------------------------------------------------------------
CScriptBind_MaterialEffects::CScriptBind_MaterialEffects(ISystem* pSystem, CMaterialEffects* pMFX)
{
	m_pSystem = pSystem;
	m_pMFX = pMFX;
	assert(m_pMFX != 0);

	Init(gEnv->pScriptSystem, m_pSystem);
	SetGlobalName("MaterialEffects");

	RegisterGlobals();
	RegisterMethods();
}

//------------------------------------------------------------------------
CScriptBind_MaterialEffects::~CScriptBind_MaterialEffects()
{
}

//------------------------------------------------------------------------
void CScriptBind_MaterialEffects::RegisterGlobals()
{
	gEnv->pScriptSystem->SetGlobalValue("MFX_INVALID_EFFECTID", InvalidEffectId);
}

//------------------------------------------------------------------------
void CScriptBind_MaterialEffects::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_MaterialEffects::

	SCRIPT_REG_TEMPLFUNC(GetEffectId, "customName, surfaceIndex2");
	SCRIPT_REG_TEMPLFUNC(GetEffectIdByLibName, "LibName, FGFXName");
	SCRIPT_REG_TEMPLFUNC(PrintEffectIdByMatIndex, "MatName1, MatName2");
	SCRIPT_REG_TEMPLFUNC(ExecuteEffect, "effectId, paramsTable");
}

//------------------------------------------------------------------------
int CScriptBind_MaterialEffects::GetEffectId(IFunctionHandler* pH, const char* customName, int surfaceIndex2)
{
	TMFXEffectId effectId = m_pMFX->GetEffectId(customName, surfaceIndex2);

	return pH->EndFunction(effectId);
}

//------------------------------------------------------------------------
int CScriptBind_MaterialEffects::GetEffectIdByLibName(IFunctionHandler* pH, const char* LibName, const char* FGFXName)
{
	TMFXEffectId effectId = m_pMFX->GetEffectIdByName(LibName, FGFXName);

	return pH->EndFunction(effectId);
}

//------------------------------------------------------------------------
int CScriptBind_MaterialEffects::PrintEffectIdByMatIndex(IFunctionHandler* pH, int materialIndex1, int materialIndex2)
{
	TMFXEffectId effectId = m_pMFX->InternalGetEffectId(materialIndex1, materialIndex2);

	DrxLogAlways("Requested MaterialEffect ID: %u", effectId);

	return pH->EndFunction(effectId);
}

//------------------------------------------------------------------------
int CScriptBind_MaterialEffects::ExecuteEffect(IFunctionHandler* pH, int effectId, SmartScriptTable paramsTable)
{
	if (effectId == InvalidEffectId)
		return pH->EndFunction(false);

	// minimalistic implementation.. extend if you need it
	SMFXRunTimeEffectParams params;
	paramsTable->GetValue("pos", params.pos);
	paramsTable->GetValue("normal", params.normal);
	paramsTable->GetValue("scale", params.scale);
	paramsTable->GetValue("angle", params.angle);

	bool res = m_pMFX->ExecuteEffect(effectId, params);

	return pH->EndFunction(res);
}
