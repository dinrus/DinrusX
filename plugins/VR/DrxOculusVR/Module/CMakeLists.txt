#START-FILE-LIST
# File List auto generated by waf2cmake.py, do not modify manually.

start_sources()
# -- cryoculusvr.waf_files --
sources_platform(ALL)
add_sources("DrxOculusVR_uber_0.cpp"
	SOURCE_GROUP "Interface"
		"../Interface/IHmdOculusRiftDevice.h"
		"../Interface/IOculusVRPlugin.h"
	SOURCE_GROUP "Root"
		"PluginDll.cpp"
		"PluginDll.h"
		"OculusTouchController.cpp"
		"OculusTouchController.h"
		"OculusDevice.cpp"
		"OculusDevice.h"
		"OculusResources.cpp"
		"OculusResources.h"
		"resource.h"
		"StdAfx.cpp"
		"StdAfx.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Root"
		"StdAfx.cpp"
)

end_sources()
#END-FILE-LIST

# We currently only build the oculus library for Windows
if (WIN32 OR WIN64)
	DinrusXModule(DrxOculusVR PCH "StdAfx.cpp" SOLUTION_FOLDER "DrxPlugins")

	target_include_directories( ${THIS_PROJECT}
	PRIVATE
		"${DRXENGINE_DIR}/Code/SDKs/OculusSDK/LibOVR/Include"
		"${DRXENGINE_DIR}/Code/DinrusX/DrxCommon"
	PUBLIC
		../Interface
	)
	
	target_link_libraries( ${THIS_PROJECT} PRIVATE oculus )
endif()
