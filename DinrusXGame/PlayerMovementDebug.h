// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Player movement debug for non-release builds
-------------------------------------------------------------------------
History:
- 16:07:2010   Extracted from CPlayer code by Benito Gangoso Rodriguez

*************************************************************************/

#ifndef _PLAYER_MOVEMENT_DEBUG_H_
#define _PLAYER_MOVEMENT_DEBUG_H_

struct IDebugHistoryUpr;

#if !defined(_RELEASE)
	#define PLAYER_MOVEMENT_DEBUG_ENABLED
#endif

#ifdef PLAYER_MOVEMENT_DEBUG_ENABLED

class CPlayerDebugMovement
{
public:
	CPlayerDebugMovement();
	~CPlayerDebugMovement();

	void DebugGraph_AddValue(const char* id, float value) const;
	void Debug(const IEntity* pPlayerEntity);

	void LogFallDamage(const IEntity* pPlayerEntity, const float velocityFraction, const float impactVelocity, const float damage);
	void LogFallDamageNone(const IEntity* pPlayerEntity, const float impactVelocity);
	void LogVelocityStats(const IEntity* pPlayerEntity, const pe_status_living& livStat, const float fallSpeed, const float impactVelocity);

	void GetInternalMemoryUsage(IDrxSizer * pSizer) const;

private:
	 IDebugHistoryUpr* m_pDebugHistoryUpr;
};


#endif

#endif