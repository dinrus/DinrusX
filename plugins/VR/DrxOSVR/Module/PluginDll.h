// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "../Interface/IOsvrPlugin.h"

namespace DrxVR
{
namespace Osvr {
class CPlugin_Osvr : public IOsvrPlugin, public ISystemEventListener
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IOsvrPlugin)
	DRXINTERFACE_ADD(Drx::IEnginePlugin)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CPlugin_Osvr, "Plugin_OSVR", "655d3252-2a6d-4d09-afe8-2386d4566054"_drx_guid)

	virtual ~CPlugin_Osvr();

	//! This is called to initialize the new plugin.
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;

public:
	virtual IOsvrDevice* CreateDevice() override;
	virtual IOsvrDevice* GetDevice() const override;

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener
};

}      // namespace Osvr
}      // namespace DrxVR