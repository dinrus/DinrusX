// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.


#include "StdAfx.h"
#include "ScriptBind_PerceptionUpr.h"
#include "PerceptionUpr.h"

//-----------------------------------------------------------------------------------------------------------
CScriptBind_PerceptionUpr::CScriptBind_PerceptionUpr(ISystem* pSystem) : m_pScriptSystem(nullptr)
{
	m_pSystem = pSystem;
	CScriptableBase::Init(pSystem->GetIScriptSystem(), pSystem);
	SetGlobalName("Perception");

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_PerceptionUpr::

	SCRIPT_REG_TEMPLFUNC(SetPriorityObjectType, "type");

	SCRIPT_REG_FUNC(SoundStimulus);

#undef SCRIPT_REG_CLASSNAME
}

//-----------------------------------------------------------------------------------------------------------
int CScriptBind_PerceptionUpr::SetPriorityObjectType(IFunctionHandler* pH, int type)
{
	if (type < 0)
	{
		return pH->EndFunction();
	}
	CPerceptionUpr::GetInstance()->SetPriorityObjectType(uint16(type));
	return pH->EndFunction();
}

//-----------------------------------------------------------------------------------------------------------
int CScriptBind_PerceptionUpr::SoundStimulus(IFunctionHandler* pH)
{
	int type = 0;
	float radius = 0;
	Vec3 pos(0, 0, 0);
	ScriptHandle hdl(0);
	pH->GetParams(pos, radius, type);
	// can be called from scripts without owner
	if (pH->GetParamCount() > 3)
		pH->GetParam(4, hdl);

	SAIStimulus stim(AISTIM_SOUND, type, (EntityId)hdl.n, 0, pos, ZERO, radius);
	CPerceptionUpr::GetInstance()->RegisterStimulus(stim);

	return pH->EndFunction();
}

