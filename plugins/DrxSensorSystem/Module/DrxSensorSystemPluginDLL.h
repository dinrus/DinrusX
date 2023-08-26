// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "../Interface/IDrxSensorSystemPlugin.h"

namespace Drx
{
	namespace SensorSystem
	{
		class CSensorSystem;

		class CDrxSensorSystemPlugin : public IDrxSensorSystemPlugin
		{
			DRXINTERFACE_BEGIN()
				DRXINTERFACE_ADD(IDrxSensorSystemPlugin)
				DRXINTERFACE_ADD(Drx::IEnginePlugin)
			DRXINTERFACE_END()

			DRXGENERATE_SINGLETONCLASS_GUID(CDrxSensorSystemPlugin, "Plugin_DrxSensorSystem", "08a96846-8933-4211-913f-7a64c0bf9822"_drx_guid)

			virtual ~CDrxSensorSystemPlugin() {}

			// Drx::IEnginePlugin
			virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
			virtual void MainUpdate(float frameTime) override;
			// ~Drx::IEnginePlugin

		public:

			// IDrxSensorSystemPlugin
			virtual ISensorSystem& GetSensorSystem() const override;
			// ~IDrxSensorSystemPlugin

		private:

			std::unique_ptr<CSensorSystem> m_pSensorSystem;
		};
	}
}