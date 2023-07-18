// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "HandlerBase.h"
#include "SyncLock.h"

struct ClientHandler : public HandlerBase
{
	ClientHandler(const char* bucket, int affinity, int clientTimeout);

	void Reset();
	bool ServerIsValid();
	bool Sync();

private:
	int                        m_clientTimeout;
	std::unique_ptr<SSyncLock> m_clientLock;
	std::unique_ptr<SSyncLock> m_srvLock;
};
