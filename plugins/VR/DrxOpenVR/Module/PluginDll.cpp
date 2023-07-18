// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "PluginDll.h"

#include "OpenVRResources.h"
#include "OpenVRDevice.h"

// Included only once per DLL module.
<DinrusX/CoreX/Platform/platform_impl.inl>

namespace DrxVR
{
namespace OpenVR {

CPlugin_OpenVR::~CPlugin_OpenVR()
{
	ISystem* pSystem = GetISystem();
	if (pSystem)
	{
		pSystem->GetHmdUpr()->UnregisterDevice(GetName());
	}

	DrxVR::OpenVR::Resources::Shutdown();

	if (pSystem)
	{
		pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	}
}

bool CPlugin_OpenVR::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	GetISystem()->GetISystemEventDispatcher()->RegisterListener(this,"CPlugin_OpenVR");

	return true;
}

void CPlugin_OpenVR::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_PRE_RENDERER_INIT:
		{
			// Initialize resources to make sure we query available VR devices
			DrxVR::OpenVR::Resources::Init();

			if (auto *pDevice = GetDevice())
			{
				gEnv->pSystem->GetHmdUpr()->RegisterDevice(GetName(), *pDevice);
			}
		}
	break;
	}
}

IOpenVRDevice* CPlugin_OpenVR::CreateDevice()
{
	return GetDevice();
}

IOpenVRDevice* CPlugin_OpenVR::GetDevice() const
{
	return DrxVR::OpenVR::Resources::GetAssociatedDevice();
}

DRXREGISTER_SINGLETON_CLASS(CPlugin_OpenVR)
}      // namespace OpenVR
}      // namespace DrxVR

<DinrusX/CoreX/CrtDebugStats.h>