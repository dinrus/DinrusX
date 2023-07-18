// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "DrxSensorSystemPluginDLL.h"

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

#include "SensorSystem.h"

namespace Drx
{
	namespace SensorSystem
	{
		bool CDrxSensorSystemPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
		{
			m_pSensorSystem.reset(new CSensorSystem());
			EnableUpdate(EUpdateStep::MainUpdate, true);
			return true;
		}

		ISensorSystem& CDrxSensorSystemPlugin::GetSensorSystem() const
		{
			DRX_ASSERT(m_pSensorSystem);
			return *m_pSensorSystem.get();
		}

		void CDrxSensorSystemPlugin::MainUpdate(float frameTime)
		{
			m_pSensorSystem->Update();
		}

		DRXREGISTER_SINGLETON_CLASS(CDrxSensorSystemPlugin)
	}
}

<DinrusX/CoreX/CrtDebugStats.h>
