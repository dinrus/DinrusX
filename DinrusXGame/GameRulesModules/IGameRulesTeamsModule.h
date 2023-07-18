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

#ifndef _GameRulesTeamsModule_h_
#define _GameRulesTeamsModule_h_

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/DinrusXNetwork/SerializeFwd.h>
#include <DinrusX/DinrusXGame/IGameObject.h>

class IGameRulesTeamsModule
{
public:
	virtual ~IGameRulesTeamsModule() {}

	virtual void	Init(XmlNodeRef xml) = 0;
	virtual void	PostInit() = 0;

	virtual void	RequestChangeTeam(EntityId playerId, int teamId, bool onlyIfUnassigned) = 0;

	virtual int		GetAutoAssignTeamId(EntityId playerId)	= 0;

	virtual bool	CanTeamModifyWeapons(int teamId)				= 0;
};

#endif // _GameRulesTeamsModule_h_