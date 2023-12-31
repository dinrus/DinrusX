// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __PMTUDISCOVERY_H__
#define __PMTUDISCOVERY_H__

#pragma once

#include <DinrusX/DinrusXSys/TimeValue.h>
#include <DinrusX/DinrusXNetwork/Network.h>

class CPMTUDiscovery
{
public:
	CPMTUDiscovery();

	uint16 GetMaxPacketSize(CTimeValue curTime);
	void   SentPacket(CTimeValue curTime, uint32 seq, uint16 sz);
	void   AckedPacket(CTimeValue curTime, uint32 seq, bool ack);
	void   FragmentedPacket(CTimeValue curTime);
	void   GetMemoryStatistics(IDrxSizer* pSizer, bool countingThis = false)
	{
		SIZER_COMPONENT_NAME(pSizer, "CPMTUDiscovery");

		if (countingThis)
			pSizer->Add(*this);
		pSizer->AddContainer(m_experiments);
	}

private:
	uint16     m_pmtu;
	uint16     m_peak;
	uint16     m_last;
	CTimeValue m_lastExperiment;
	CTimeValue m_lastExperimentRequest;
	bool       m_bInExperiment;

	struct SExperiment
	{
		uint16     sz;
		CTimeValue when;

		SExperiment(uint16 s = 0) : sz(s), when(g_time) {}
	};
	typedef std::map<uint32, SExperiment> TExperimentMap;
	TExperimentMap m_experiments;
};

#endif
