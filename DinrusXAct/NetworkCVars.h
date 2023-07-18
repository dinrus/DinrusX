// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __NETWORKCVARS_H__
#define __NETWORKCVARS_H__

#pragma once

#include <DinrusX/DinrusXSys/IConsole.h>

class CNetworkCVars
{
public:
	int   BreakageLog;
	float VoiceVolume;
	float PhysSyncPingSmooth;
	float PhysSyncLagSmooth;
	int   PhysDebug;
	int   BreakTimeoutFrames;
	float BreakMaxWorldSize;
	float BreakWorldOffsetX;
	float BreakWorldOffsetY;
	int   sv_LoadAllLayersForResList;

	static ILINE CNetworkCVars& Get()
	{
		DRX_ASSERT(s_pThis);
		return *s_pThis;
	}

private:
	friend class CDrxAction; // Our only creator

	CNetworkCVars(); // singleton stuff
	~CNetworkCVars();
	CNetworkCVars(const CNetworkCVars&);
	CNetworkCVars& operator=(const CNetworkCVars&);

	static CNetworkCVars* s_pThis;
};

#endif
