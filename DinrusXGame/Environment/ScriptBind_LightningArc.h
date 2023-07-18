// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _SCRIPTBIND_LIGHTNING_ARC_H_
#define _SCRIPTBIND_LIGHTNING_ARC_H_

#pragma once

#include <DrxScriptSystem/IScriptSystem.h>
#include <DrxScriptSystem/ScriptHelpers.h>


class CLightningArc;



class CScriptBind_LightningArc: public CScriptableBase
{
public:
	CScriptBind_LightningArc(ISystem* pSystem);

	void AttachTo(CLightningArc* pLightingArc);

	int TriggerSpark(IFunctionHandler* pFunction);
	int Enable(IFunctionHandler* pFunction, bool enable);
	int ReadLuaParameters(IFunctionHandler* pFunction);
};



#endif
