// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

class CDrxPerceptionSystemPlugin : public IDrxPerceptionSystemPlugin
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(IDrxPerceptionSystemPlugin)
		DRXINTERFACE_ADD(Drx::IEnginePlugin)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CDrxPerceptionSystemPlugin, "Plugin_DrxPerceptionSystem", "a4ee2509-3468-4bad-8472-dc66d91186c6"_drx_guid)

	PLUGIN_FLOWNODE_REGISTER
	PLUGIN_FLOWNODE_UNREGISTER

	CDrxPerceptionSystemPlugin() : m_pPerceptionUpr(nullptr) {}
	virtual ~CDrxPerceptionSystemPlugin() {}

	// Drx::IEnginePlugin
	virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Drx::IEnginePlugin

public:

	// IDrxPerceptionSystemPlugin
	virtual IPerceptionUpr& GetPerceptionUpr() const override;
	// ~IDrxPerceptionSystemPlugin

private:
	std::unique_ptr<CPerceptionUpr> m_pPerceptionUpr;
};

//-----------------------------------------------------------------------------------------------------------
bool CDrxPerceptionSystemPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	m_pPerceptionUpr.reset(new CPerceptionUpr);
	return true;
}

//-----------------------------------------------------------------------------------------------------------
IPerceptionUpr& CDrxPerceptionSystemPlugin::GetPerceptionUpr() const
{
	DRX_ASSERT(m_pPerceptionUpr);
	return *m_pPerceptionUpr.get();
}

//-----------------------------------------------------------------------------------------------------------
DRXREGISTER_SINGLETON_CLASS(CDrxPerceptionSystemPlugin)
