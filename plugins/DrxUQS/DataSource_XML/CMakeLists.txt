#START-FILE-LIST
# File List auto generated by waf2cmake.py, do not modify manually.

start_sources()
# -- uqs_datasource_xml.waf_files --
sources_platform(ALL)
add_sources("NoUberFile"
	SOURCE_GROUP "datasource_xml"
		"datasource_xml/StdAfx.cpp"
		"datasource_xml/StdAfx.h"
)

add_sources("UQS_uber_datasource_xml.cpp"
	SOURCE_GROUP "Root"
		"DataSource_XML_Includes.h"
	SOURCE_GROUP "datasource_xml"
		"datasource_xml/SyntaxErrorCollector_XML.h"
		"datasource_xml/SyntaxErrorCollector_XML.cpp"
		"datasource_xml/QueryBlueprintLoader_XML.h"
		"datasource_xml/QueryBlueprintLoader_XML.cpp"
		"datasource_xml/QueryBlueprintSaver_XML.h"
		"datasource_xml/QueryBlueprintSaver_XML.cpp"
		"datasource_xml/QueryBlueprintFileLibrary.h"
		"datasource_xml/QueryBlueprintFileLibrary.cpp"
		"datasource_xml/XMLDatasource.h"
		"datasource_xml/XMLDatasource.cpp"
)

end_sources()
#END-FILE-LIST

DinrusXStaticModule(DrxUQS_DataSource_XML PCH "datasource_xml/StdAfx.cpp" SOLUTION_FOLDER "DrxPlugins/DrxUQS")

target_include_directories(${THIS_PROJECT}
	PRIVATE datasource_xml
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

if(OPTION_UQS_SCHEMATYC_SUPPORT)
	target_include_directories(${THIS_PROJECT}
		PRIVATE "${DRXENGINE_DIR}/Code/DinrusX/DinrusXSchema/Core/Interface"
		PRIVATE "${DRXENGINE_DIR}/Code/DinrusX/DinrusXSchema/STDEnv/Interface"
	)
endif()
