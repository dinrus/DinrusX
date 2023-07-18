// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSys/IUserAnalytics.h>

#if !defined(_RELEASE) && DRX_PLATFORM_WINDOWS
#include <DinrusX/DinrusXSys/IDrxPluginUpr.h>

struct IDrxUserAnalyticsPlugin;

class CUserAnalyticsSystem : public IUserAnalyticsSystem, public Drx::IPluginUpr::IEventListener
{
public:
	CUserAnalyticsSystem();
	~CUserAnalyticsSystem();

	virtual void TriggerEvent(const char* szEventName, UserAnalytics::Attributes* attributes) override;

	void         Initialize();
	void         RegisterCVars();

private:
	virtual void OnPluginEvent(const DrxClassID& pluginClassId, Drx::IPluginUpr::IEventListener::EEvent event) override;

	IDrxUserAnalyticsPlugin* m_pUserAnalyticsPlugin;
	IUserAnalytics*          m_pUserAnalytics;

	static int               m_enableUserAnalyticsCollect;
	static int               m_enableUserAnalyticsLogging;
};
#else
class CUserAnalyticsSystem : public IUserAnalyticsSystem
{
public:
	CUserAnalyticsSystem() {};
	~CUserAnalyticsSystem() {};

	virtual void TriggerEvent(const char* szEventName, UserAnalytics::Attributes* attributes) override {}

	void         Initialize()                                                                          {}
	void         RegisterCVars()                                                                       {}
};
#endif
