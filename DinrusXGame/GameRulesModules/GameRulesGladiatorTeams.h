// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 

	-------------------------------------------------------------------------
	History:
	- 09:05:2011  : Created by Colin Gulliver

*************************************************************************/

#ifndef _GameRulesGladiatorTeams_h_
#define _GameRulesGladiatorTeams_h_

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/DinrusXGame/GameRulesStandardTwoTeams.h>
#include <DinrusX/CoreX/String/DrxFixedString.h>

class CGameRulesGladiatorTeams : public CGameRulesStandardTwoTeams
{
public:
	virtual int		GetAutoAssignTeamId(EntityId playerId) { return 0; }		// Defer team selection for objectives module
	virtual void	RequestChangeTeam(EntityId playerId, int teamId, bool onlyIfUnassigned) {};
};

#endif // _GameRulesGladiatorTeams_h_
