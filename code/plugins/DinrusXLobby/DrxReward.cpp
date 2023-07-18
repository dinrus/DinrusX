// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxReward.h"

CDrxReward::CDrxReward(CDrxLobby* pLobby)
	: m_pLobby(pLobby)
{
	assert(pLobby);
	m_status = eCRS_Uninitialised;
}

CDrxReward::~CDrxReward(void)
{
}

EDrxRewardError CDrxReward::Award(uint32 playerID, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg)
{
	// Default implementation
	return eCRE_Failed;
}

void CDrxReward::Tick(CTimeValue tv)
{
}
// [EOF]
