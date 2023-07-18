// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   StatsAgent.h
//  Version:     v1.00
//  Created:     06/10/2009 by Steve Barnett.
//  Description: This the declaration for CStatsAgent
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __STATS_AGENT_H__
#define __STATS_AGENT_H__

#pragma once

class ICmdLineArg;

class CStatsAgent
{
public:
	static void CreatePipe( const ICmdLineArg* pPipeName );
	static void ClosePipe( void );
	static void Update( void );

	static void SetDelayMessages(bool enable);
	static void SetDelayTimeout(const int timeout);
private:
	static bool s_delayMessages;
	static int s_iDelayMessagesCounter;
	CStatsAgent( void ) {} // Prevent instantiation
};

#endif
