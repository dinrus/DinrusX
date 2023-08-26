// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/HMDCVars.h>
#include<DinrusX/DinrusXSys/IHMDDevice.h>

namespace DrxVR
{

int CVars::hmd_info = 0;
int CVars::hmd_social_screen = static_cast<int>(EHmdSocialScreen::DistortedDualImage);
int CVars::hmd_social_screen_aspect_mode = static_cast<int>(EHmdSocialScreenAspectMode::Fill);
int CVars::hmd_post_inject_camera = 1;
int CVars::hmd_tracking_origin = static_cast<int>(EHmdTrackingOrigin::Seated);
float CVars::hmd_resolution_scale = 1.f;
ICVar* CVars::pSelectedHmdNameVar = nullptr;

}
