// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 
		Interface for squad manager events

	-------------------------------------------------------------------------
	History:
	- 14:12:2011  : Created by Ben Johnson

*************************************************************************/

#ifndef _ISQUADEVENTLISTENER_H_
#define _ISQUADEVENTLISTENER_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/CoreX/Lobby/IDrxLobbyUI.h>

class ISquadEventListener
{
public:
	// Local squad events
	enum ESquadEventType
	{
		eSET_CreatedSquad,
		eSET_MigratedSquad,
		eSET_JoinedSquad,
		eSET_LeftSquad,
	};

public:
	virtual ~ISquadEventListener() {}

	virtual void AddedSquaddie(DrxUserID userId) = 0;
	virtual void RemovedSquaddie(DrxUserID userId) = 0;
	virtual void UpdatedSquaddie(DrxUserID userId) = 0;
	virtual void SquadLeaderChanged(DrxUserID userId) = 0;

	virtual void SquadEvent(ISquadEventListener::ESquadEventType eventType) = 0;
};

#endif // _ISQUADEVENTLISTENER_H_
