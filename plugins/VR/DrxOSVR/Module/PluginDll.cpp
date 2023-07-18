// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "PluginDll.h"

#include "OsvrResources.h"
#include "OsvrDevice.h"

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

<DinrusX/DinrusXSys/VR/IHMDUpr.h>

namespace DrxVR
{
namespace Osvr {

CPlugin_Osvr::~CPlugin_Osvr()
{
	DrxVR::Osvr::Resources::Shutdown();

	GetISystem()->GetISystemEventDispatcher()->RemoveListener(this);
}

bool CPlugin_Osvr::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	GetISystem()->GetISystemEventDispatcher()->RegisterListener(this, "CPlugin_Osvr");

	return true;
}

void CPlugin_Osvr::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_PRE_RENDERER_INIT:
	{
		// Initialize resources to make sure we query available VR devices
		DrxVR::Osvr::Resources::Init();

		if (auto *pDevice = GetDevice())
		{
			gEnv->pSystem->GetHmdUpr()->RegisterDevice(GetName(), *pDevice);
		}
	}
	break;
	}
}

IOsvrDevice* CPlugin_Osvr::CreateDevice()
{
	return GetDevice();
}

IOsvrDevice* CPlugin_Osvr::GetDevice() const
{
	return DrxVR::Osvr::Resources::GetAssociatedDevice();
}

DRXREGISTER_SINGLETON_CLASS(CPlugin_Osvr)
}      // namespace Osvr
}      // namespace DrxVR

<DinrusX/CoreX/CrtDebugStats.h>