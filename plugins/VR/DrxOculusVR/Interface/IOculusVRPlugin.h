// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>

#include "IHmdOculusRiftDevice.h"

namespace DrxVR
{
namespace Oculus {
struct IOculusVRPlugin : public Drx::IEnginePlugin
{
	DRXINTERFACE_DECLARE_GUID(IOculusVRPlugin, "e4b6593f-30ce-4f80-9348-285f842e11c1"_drx_guid);

public:
	virtual IOculusDevice* CreateDevice() = 0;
	virtual IOculusDevice* GetDevice() const = 0;
};

}      // namespace Oculus
}      // namespace DrxVR