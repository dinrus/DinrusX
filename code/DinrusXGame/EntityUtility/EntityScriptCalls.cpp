// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------

Description: Util functions relating to script entity operations.

-------------------------------------------------------------------------
History:
- 22:02:2011: Created by Benito G.R.

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/EntityScriptCalls.h>

bool EntityScripts::CallScriptFunction(IEntity* pEntity, IScriptTable *pScriptTable, const char *functionName)
{
	bool result = false;

	if ((pEntity != NULL) && (pScriptTable != NULL))
	{
		IScriptSystem *pScriptSystem = pScriptTable->GetScriptSystem();
		if (pScriptTable->GetValueType(functionName) == svtFunction)
		{
			pScriptSystem->BeginCall(pScriptTable, functionName); 
			pScriptSystem->PushFuncParam(pEntity->GetScriptTable());
			pScriptSystem->EndCall(result);
		}
	}

	return result;
}