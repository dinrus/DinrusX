// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#include "IDefaultComponentsPlugin.h"

class CPlugin_DrxDefaultEntities final : public IPlugin_DrxDefaultEntities
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IPlugin_DrxDefaultEntities)
	DRXINTERFACE_ADD(Drx::IEnginePlugin)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CPlugin_DrxDefaultEntities, "Plugin_DrxDefaultEntities", "{CB9E7C85-3289-41B6-983A-6A076ABA6351}"_drx_guid)

	PLUGIN_FLOWNODE_REGISTER
	PLUGIN_FLOWNODE_UNREGISTER

	virtual ~CPlugin_DrxDefaultEntities();

	void RegisterComponents(Schematyc::IEnvRegistrar& registrar);

public:
	// Drx::IEnginePlugin
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Drx::IEnginePlugin

	virtual ICameraUpr* GetICameraUpr() override
	{
		return m_pCameraUpr.get();
	}

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

private:
	std::unique_ptr<ICameraUpr> m_pCameraUpr;
};

struct IEntityRegistrator
{
	IEntityRegistrator()
		: m_pNext(nullptr)
	{
		if (g_pFirst == nullptr)
		{
			g_pFirst = this;
			g_pLast = this;
		}
		else
		{
			g_pLast->m_pNext = this;
			g_pLast = g_pLast->m_pNext;
		}
	}

	virtual void Register() = 0;

public:
	IEntityRegistrator *m_pNext;

	static IEntityRegistrator *g_pFirst;
	static IEntityRegistrator *g_pLast;
};