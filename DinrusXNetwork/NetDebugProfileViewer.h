// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __NETDEBUGPROFILEVIEWER_H__
#define __NETDEBUGPROFILEVIEWER_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Config.h>

#if ENABLE_NET_DEBUG_INFO
	#if NET_PROFILE_ENABLE

		#include <DinrusX/DinrusXNetwork/NetDebugInfo.h>

class CNetDebugProfileViewer : public CNetDebugInfoSection
{
public:
	CNetDebugProfileViewer(uint32 type);
	virtual ~CNetDebugProfileViewer();

	virtual void Tick(const SNetworkProfilingStats* const pProfilingStats);
	virtual void Draw(const SNetworkProfilingStats* const pProfilingStats);

private:

	void DrawProfileEntry(const SNetProfileStackEntry* entry, int depth, int* line);
	void DrawTree(const SNetProfileStackEntry* root, int depth, int* line);

};

	#endif
#endif // ENABLE_NET_DEBUG_INFO

#endif
