// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>

namespace Drx
{
	namespace SensorSystem
	{
		struct ISensorSystem;

		struct IDrxSensorSystemPlugin : public Drx::IEnginePlugin
		{
			DRXINTERFACE_DECLARE_GUID(IDrxSensorSystemPlugin, "43678bb8-48cd-4bb8-ae11-8f6b0a347d9a"_drx_guid);

		public:

			virtual ISensorSystem& GetSensorSystem() const = 0;
		};
	}
}