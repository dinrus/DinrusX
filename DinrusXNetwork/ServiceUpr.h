// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SERVICEMANAGER_H__
#define __SERVICEMANAGER_H__

#pragma once

#include <DinrusX/DinrusXNetwork/INetwork.h>
#include <DinrusX/DinrusXNetwork/INetworkService.h>
#include <DinrusX/DinrusXNetwork/NetTimer.h>

class CServiceUpr
{
public:
	CServiceUpr();
	~CServiceUpr();

	INetworkServicePtr GetService(const string& name);

	void               CreateExtension(bool server, IContextViewExtensionAdder* adder);

	void               GetMemoryStatistics(IDrxSizer* pSizer)
	{
		SIZER_COMPONENT_NAME(pSizer, "CServiceUpr");

		pSizer->Add(*this);
		pSizer->AddContainer(m_services);
		for (TServices::const_iterator it = m_services.begin(); it != m_services.end(); ++it)
		{
			pSizer->AddString(it->first);
			it->second->GetMemoryStatistics(pSizer);
		}
	}

private:
	NetTimerId m_timer;
	typedef std::map<string, INetworkServicePtr> TServices;
	TServices  m_services;

	INetworkServicePtr CreateService(const string& name);
	static void        TimerCallback(NetTimerId id, void* pUser, CTimeValue tm);
};

#endif
