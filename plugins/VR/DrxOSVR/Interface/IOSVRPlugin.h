// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>

#include "IHmdOSVRDevice.h"

namespace DrxVR
{
namespace Osvr {
struct IOsvrPlugin : public Drx::IEnginePlugin
{
	DRXINTERFACE_DECLARE_GUID(IOsvrPlugin, "47512e92-6a94-404d-a2c9-a14f0633eebe"_drx_guid);

public:
	virtual IOsvrDevice* CreateDevice() = 0;
	virtual IOsvrDevice* GetDevice() const = 0;
};

}      // namespace Osvr
}      // namespace DrxVR