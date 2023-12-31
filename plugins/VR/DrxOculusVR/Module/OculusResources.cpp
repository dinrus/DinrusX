// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#include "OculusResources.h"
#include "OculusDevice.h"

namespace DrxVR {
namespace Oculus {

// static member variables
Resources* Resources::ms_pRes = 0;
bool Resources::ms_libInitialized = false;

// ------------------------------------------------------------------------
// Creates the resource manager for Oculus devices.
// If an Oculus devices is attached it will create and store an instance of the device.
Resources::Resources()
	: m_pDevice(0)
{
	DrxLogAlways("[HMD][Oculus] Initialising Resources - Using OculusVR SDK " OVR_VERSION_STRING);

	// Initialized the Oculus SDK
	if (OVR_SUCCESS(ovr_Initialize(NULL)))
	{
		ms_libInitialized = true;
		DrxLogAlways("[HMD][Oculus] SDK successfully initialized");

		m_pDevice = Device::CreateInstance();
		if (m_pDevice != nullptr)
		{
			const bool initOk = m_pDevice->GetDesc().Type != ovrHmd_None;
			DrxLogAlways("[HMD][Oculus] Oculus HMD was created. Initialization [%s]", initOk ? "was sucessful" : "failed");
			return;
		}
		else
		{
			DrxLogAlways("[HMD][Oculus] Oculus HMD device creation failed");
		}
	}
	else
	{
		ovrErrorInfo errorInfo;
		ovr_GetLastErrorInfo(&errorInfo);
		DrxLogAlways("[HMD][Oculus] Failed to initialize OculusVR SDK! [%s]", *errorInfo.ErrorString ? errorInfo.ErrorString : "unspecified error");
	}
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
		ovr_Shutdown();
		DrxLogAlways("[HMD][Oculus] ovr_Shutdown finished");
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

} // namespace Oculus
} // namespace DrxVR
