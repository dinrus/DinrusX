// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __NETDEBUGINTERNETSIMULATOR_H__
#define __NETDEBUGINTERNETSIMULATOR_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Config.h>

#if ENABLE_NET_DEBUG_INFO

	#include <DinrusX/DinrusXNetwork/NetDebugInfo.h>

class CNetDebugInternetSimulator : public CNetDebugInfoSection
{
public:
	CNetDebugInternetSimulator();
	virtual ~CNetDebugInternetSimulator();

	virtual void Tick(const SNetworkProfilingStats* const pProfilingStats);
	virtual void Draw(const SNetworkProfilingStats* const pProfilingStats);

private:

	SAccumulator<eBS_Lag> m_lagAverage;

	float                 m_packetLossRate;
	uint32                m_packetSends;
	uint32                m_packetDrops;
	uint32                m_packetLagMin;
	uint32                m_packetLagMax;
};

#endif // ENABLE_NET_DEBUG_INFO

#endif
