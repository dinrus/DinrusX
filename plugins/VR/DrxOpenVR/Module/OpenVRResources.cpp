// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#include "OpenVRResources.h"
#include "OpenVRDevice.h"

namespace DrxVR {
namespace OpenVR {

// static member variables
Resources* Resources::ms_pRes = 0;
bool Resources::ms_libInitialized = false;

// ------------------------------------------------------------------------
// Creates the resource manager for OpenVR devices.
// If an OpenVR device is attached it will create and store an instance of the device.
Resources::Resources()
	: m_pDevice(0)
{
	DrxLogAlways("[HMD][OpenVR] Initialising Resources - Using OpenVR %s", vr::IVRSystem_Version);

	vr::EVRInitError eError = vr::EVRInitError::VRInitError_None;
	vr::IVRSystem* pSystem = vr::VR_Init(&eError, vr::EVRApplicationType::VRApplication_Scene);

	if (eError != vr::EVRInitError::VRInitError_None)
	{
		pSystem = NULL;
		DrxLogAlways("[HMD][OpenVR] Unable to initialize VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return;
	}

	ms_libInitialized = true;

	if (!vr::VR_IsHmdPresent())
	{
		DrxLogAlways("[HMD][OpenVR] No HMD Connected!");
		return;
	}
	else
	{
		m_pDevice = Device::CreateInstance(pSystem);
	}
}

// ------------------------------------------------------------------------
void Resources::PostInit()
{
	Device* pDev = GetAssociatedDevice();
	if (pDev)
		pDev->SetupRenderModels();
}

// ------------------------------------------------------------------------
Resources::~Resources()
{
	#if !defined(_RELEASE)
	if (m_pDevice && m_pDevice->GetRefCount() != 1)
		__debugbreak();
	#endif

	SAFE_RELEASE(m_pDevice);

	if (ms_libInitialized)
	{
		vr::VR_Shutdown();
		DrxLogAlways("[HMD][OpenVR] Shutdown finished");
	}
}

// ------------------------------------------------------------------------
void Resources::Init()
{
	#if !defined(_RELEASE)
	if (ms_pRes)
		__debugbreak();
	#endif

	if (!ms_pRes)
		ms_pRes = new Resources();
}

// ------------------------------------------------------------------------
void Resources::Shutdown()
{
	#if !defined(_RELEASE)
	if (ms_libInitialized && !ms_pRes)
		__debugbreak();
	#endif

	SAFE_DELETE(ms_pRes);
}

} // namespace OpenVR
} // namespace DrxVR
