// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>

#include "IHmdOpenVRDevice.h"

namespace DrxVR
{
namespace OpenVR {
struct IOpenVRPlugin : public Drx::IEnginePlugin
{
	DRXINTERFACE_DECLARE_GUID(IOpenVRPlugin, "cd1389a9-b375-47f9-bc45-d382d18b21b1"_drx_guid);

public:
	virtual IOpenVRDevice* CreateDevice() = 0;
	virtual IOpenVRDevice* GetDevice() const = 0;
};

}      // namespace OpenVR
}      // namespace DrxVR