#START-FILE-LIST
# File List auto generated by waf2cmake.py, do not modify manually.

start_sources()
# -- uqs_core.waf_files --
sources_platform(ALL)
add_sources("NoUberFile"
	SOURCE_GROUP "core"
		"core/StdAfx.cpp"
		"core/StdAfx.h"
)

add_sources("UQS_uber_core.cpp"
	SOURCE_GROUP "Root"
		"resource.h"
	SOURCE_GROUP "core"
		"core/Logger.h"
		"core/Logger.cpp"
		"core/FactoryDatabase.h"
		"core/ItemFactoryDatabase.h"
		"core/DebugRenderPrimitives.h"
		"core/DebugRenderPrimitives.cpp"
		"core/DebugRenderWorld.h"
		"core/DebugRenderWorld.cpp"
		"core/ItemList.h"
		"core/ItemList.cpp"
		"core/ItemSerializationSupport.h"
		"core/ItemSerializationSupport.cpp"
		"core/InputBlueprint.h"
		"core/InputBlueprint.cpp"
		"core/BlueprintWithInputs.h"
		"core/BlueprintWithInputs.cpp"
		"core/FunctionFactoryDatabase.h"
		"core/FunctionBlueprint.h"
		"core/FunctionBlueprint.cpp"
		"core/FunctionCallHierarchy.h"
		"core/FunctionCallHierarchy.cpp"
		"core/InstantEvaluatorFactoryDatabase.h"
		"core/InstantEvaluatorBlueprint.h"
		"core/InstantEvaluatorBlueprint.cpp"
		"core/DeferredEvaluatorFactoryDatabase.h"
		"core/DeferredEvaluatorBlueprint.h"
		"core/DeferredEvaluatorBlueprint.cpp"
		"core/GeneratorFactoryDatabase.h"
		"core/GeneratorBlueprint.h"
		"core/GeneratorBlueprint.cpp"
		"core/GlobalConstantParamsBlueprint.h"
		"core/GlobalConstantParamsBlueprint.cpp"
		"core/GlobalRuntimeParamsBlueprint.h"
		"core/GlobalRuntimeParamsBlueprint.cpp"
		"core/QueryBlueprint.h"
		"core/QueryBlueprint.cpp"
		"core/QueryBlueprintLibrary.h"
		"core/QueryBlueprintLibrary.cpp"
		"core/Query_Regular.h"
		"core/Query_Regular.cpp"
		"core/QueryUpr.h"
		"core/QueryUpr.cpp"
		"core/StartupConsistencyChecker.h"
		"core/StartupConsistencyChecker.cpp"
		"core/Hub.h"
		"core/Hub.cpp"
		"core/HubPlugin.cpp"
		"core/Cvars.h"
		"core/Cvars.cpp"
		"core/QueryHistory.h"
		"core/QueryHistory.cpp"
		"core/QueryHistoryUpr.h"
		"core/QueryHistoryUpr.cpp"
		"core/QueryHistoryInGameGUI.h"
		"core/QueryHistoryInGameGUI.cpp"
		"core/ItemDebugProxies.h"
		"core/ItemDebugProxies.cpp"
		"core/ItemDebugProxyFactory.h"
		"core/ItemDebugProxyFactory.cpp"
		"core/Serialize.h"
		"core/Serialize.cpp"
		"core/DrawUtil2d.h"
		"core/DrawUtil2d.cpp"
		"core/EditorService.h"
		"core/EditorService.cpp"
		"core/Utils.h"
		"core/Utils.cpp"
		"core/QueryResultSet.h"
		"core/QueryResultSet.cpp"
		"core/QueryBase.h"
		"core/QueryBase.cpp"
		"core/QueryFactory.h"
		"core/QueryFactory.cpp"
		"core/Query_SequentialBase.h"
		"core/Query_SequentialBase.cpp"
		"core/Query_Chained.h"
		"core/Query_Chained.cpp"
		"core/Query_Fallbacks.h"
		"core/Query_Fallbacks.cpp"
		"core/GeomUtil.h"
		"core/GeomUtil.cpp"
		"core/TimeBudget.h"
		"core/TimeBudget.cpp"
		"core/LeafFunctionReturnValue.h"
		"core/LeafFunctionReturnValue.cpp"
		"core/SchematycSupport.h"
		"core/SchematycSupport.cpp"
		"core/ScoreTransformFactory.h"
		"core/ScoreTransformFactory.cpp"
		"core/EvaluationResultTransform.h"
		"core/EvaluationResultTransform.cpp"
		"core/TextualEvaluatorBlueprint.h"
		"core/EvaluatorBlueprintBase.h"
		"core/EvaluatorBlueprintBase.cpp"
		"core/SettingsUpr.h"
		"core/SettingsUpr.cpp"
)

end_sources()
#END-FILE-LIST

DinrusXModule(DrxUQS_Core PCH "core/StdAfx.cpp" SOLUTION_FOLDER "DrxPlugins/DrxUQS")
target_link_libraries (${THIS_PROJECT} PRIVATE DrxUQS_DataSource_XML)
target_link_libraries (${THIS_PROJECT} PRIVATE DrxUQS_StdLib)

target_include_directories(${THIS_PROJECT}
	PRIVATE core
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)

if(OPTION_UQS_SCHEMATYC_SUPPORT)
	target_include_directories(${THIS_PROJECT}
		PUBLIC "${DRXENGINE_DIR}/Code/DinrusX/DinrusXSchema/Core/Interface"
		PUBLIC "${DRXENGINE_DIR}/Code/DinrusX/DinrusXSchema/STDEnv/Interface"
	)
endif()
