// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 

	-------------------------------------------------------------------------
	History:
	- 02:09:2009  : Created by Colin Gulliver

*************************************************************************/

#ifndef _IGameRulesModulesUpr_h_
#define _IGameRulesModulesUpr_h_

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/CoreX/Game/IGameFramework.h>
#include <DinrusX/DinrusXGame/GameRulesModules/GameRulesModulesRegistration.h>

class IGameRulesModulesUpr
{
public:
	virtual ~IGameRulesModulesUpr() {}

// Register as a factory for each module type
#define GAMERULES_MODULE_LIST_FUNC(type, name, lowerCase, useInEditor) DECLARE_GAMEOBJECT_FACTORY(type)
	GAMERULES_MODULE_TYPES_LIST
#undef GAMERULES_MODULE_LIST_FUNC

	virtual void Init() = 0;

	virtual int GetRulesCount() = 0;

	virtual const char* GetRules(int index) = 0;
};

#endif // _IGameRulesModulesUpr_h_
