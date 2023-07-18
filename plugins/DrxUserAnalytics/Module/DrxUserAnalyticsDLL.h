// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/UserAnalytics/IDrxUserAnalyticsPlugin.h>

class CUserAnalytics;

class CPlugin_DrxUserAnalytics : public IDrxUserAnalyticsPlugin
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IDrxUserAnalyticsPlugin)
	DRXINTERFACE_ADD(Drx::IEnginePlugin)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CPlugin_DrxUserAnalytics, "Plugin_DrxUserAnalytics", "2284d2bf-677c-4e72-8ace-10f924bdd068"_drx_guid)

	CPlugin_DrxUserAnalytics();
	virtual ~CPlugin_DrxUserAnalytics();

	//! This is called to initialize the new plugin.
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;

public:
	virtual IUserAnalytics* GetIUserAnalytics() const override;

private:
	CUserAnalytics* m_pUserAnalytics;
};
