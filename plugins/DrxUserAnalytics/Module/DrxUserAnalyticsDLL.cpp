// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxUserAnalyticsDLL.h"
#include "UserAnalytics.h"

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

CPlugin_DrxUserAnalytics::CPlugin_DrxUserAnalytics()
	: m_pUserAnalytics(nullptr)
{
}

CPlugin_DrxUserAnalytics::~CPlugin_DrxUserAnalytics()
{
	SAFE_DELETE(m_pUserAnalytics);
}

bool CPlugin_DrxUserAnalytics::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	m_pUserAnalytics = new CUserAnalytics();

	return (m_pUserAnalytics != nullptr);
}

IUserAnalytics* CPlugin_DrxUserAnalytics::GetIUserAnalytics() const
{
	return m_pUserAnalytics;
}

DRXREGISTER_SINGLETON_CLASS(CPlugin_DrxUserAnalytics)

<DinrusX/CoreX/CrtDebugStats.h>
