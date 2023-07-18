# Mandatory plugin, contains entities required by the engine
if (OPTION_ENGINE)
	add_subdirectory (DrxDefaultEntities/Module)
	add_subdirectory(DrxSensorSystem/Module)
endif()

# Example plugin
if (PLUGIN_FPSPLUGIN)
	add_subdirectory(DrxFramesPerSecond/Module)
endif()
if (PLUGIN_USERANALYTICS)
	add_subdirectory(DrxUserAnalytics/Module)
endif()
# VR plugins
if (PLUGIN_VR_OCULUS)
	add_subdirectory(VR/DrxOculusVR/Module)
endif()
if (PLUGIN_VR_OPENVR)	
	add_subdirectory(VR/DrxOpenVR/Module)
endif()	
if (PLUGIN_VR_OSVR)	
	add_subdirectory(VR/DrxOSVR/Module)
endif()

# UQS: Optional plugin; option PLUGIN_DRXUQS to enable/disable it resides in its own sub directory
add_subdirectory(DrxUQS)

add_subdirectory(DrxHTTP/Module)
add_subdirectory(DrxGamePlatform/Module)
add_subdirectory(DrxPerceptionSystem/Module)