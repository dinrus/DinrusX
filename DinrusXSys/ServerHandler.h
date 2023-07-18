// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "HandlerBase.h"
#include "SyncLock.h"

struct ServerHandler : public HandlerBase
{
	ServerHandler(const char* bucket, int affinity, int serverTimeout);

	void DoScan();
	bool Sync();

private:
	int m_serverTimeout;
	std::vector<std::unique_ptr<SSyncLock>> m_clientLocks;
	std::vector<std::unique_ptr<SSyncLock>> m_srvLocks;
	CTimeValue                              m_lastScan;
};
