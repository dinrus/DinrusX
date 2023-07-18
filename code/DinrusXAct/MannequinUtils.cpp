// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/MannequinUtils.h>

#include <DinrusX/DinrusXAct/IDrxMannequin.h>

const char* mannequin::FindProcClipTypeName(const IProceduralClipFactory::THash& typeNameHash)
{
	if (gEnv && gEnv->pGameFramework)
	{
		const IProceduralClipFactory& proceduralClipFactory = gEnv->pGameFramework->GetMannequinInterface().GetProceduralClipFactory();
		const char* const typeName = proceduralClipFactory.FindTypeName(typeNameHash);
		return typeName;
	}
	return NULL;
}
