// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// Prerequisite headers.

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/IXml.h>

// Core headers.

#include <DinrusX/DinrusXSchema/Action.h>
#include <DinrusX/DinrusXSchema/Component.h>
#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/ICore.h>
#include <DinrusX/DinrusXSchema/IObject.h>
#include <DinrusX/DinrusXSchema/IObjectProperties.h>
#include <DinrusX/DinrusXSchema/ResourceTypes.h>
#include <DinrusX/DinrusXSchema/MathTypes.h>

#include <DinrusX/DinrusXSchema/CompilerContext.h>
#include <DinrusX/DinrusXSchema/ICompiler.h>
#include <DinrusX/DinrusXSchema/IGraphNodeCompiler.h>

#include <DinrusX/DinrusXSchema/IQuickSearchOptions.h>

#include <DinrusX/DinrusXSchema/EnvContext.h>
#include <DinrusX/DinrusXSchema/EnvElementBase.h>
#include <DinrusX/DinrusXSchema/EnvPackage.h>
#include <DinrusX/DinrusXSchema/EnvUtils.h>
#include <DinrusX/DinrusXSchema/IEnvContext.h>
#include <DinrusX/DinrusXSchema/IEnvElement.h>
#include <DinrusX/DinrusXSchema/IEnvPackage.h>
#include <DinrusX/DinrusXSchema/IEnvRegistrar.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>

#include <DinrusX/DinrusXSchema/EnvAction.h>
#include <DinrusX/DinrusXSchema/EnvClass.h>
#include <DinrusX/DinrusXSchema/EnvComponent.h>
#include <DinrusX/DinrusXSchema/EnvDataType.h>
#include <DinrusX/DinrusXSchema/EnvFunction.h>
#include <DinrusX/DinrusXSchema/EnvInterface.h>
#include <DinrusX/DinrusXSchema/EnvModule.h>
#include <DinrusX/DinrusXSchema/EnvSignal.h>
#include <DinrusX/DinrusXSchema/IEnvAction.h>
#include <DinrusX/DinrusXSchema/IEnvClass.h>
#include <DinrusX/DinrusXSchema/IEnvComponent.h>
#include <DinrusX/DinrusXSchema/IEnvDataType.h>
#include <DinrusX/DinrusXSchema/IEnvFunction.h>
#include <DinrusX/DinrusXSchema/IEnvInterface.h>
#include <DinrusX/DinrusXSchema/IEnvModule.h>
#include <DinrusX/DinrusXSchema/IEnvSignal.h>

#include <DinrusX/DinrusXSchema/INetworkObject.h>
#include <DinrusX/DinrusXSchema/INetworkSpawnParams.h>

#include <DinrusX/DinrusXSchema/ActionDesc.h>
#include <DinrusX/DinrusXSchema/ComponentDesc.h>
#include <DinrusX/DinrusXSchema/FunctionDesc.h>
#include <DinrusX/DinrusXSchema/ReflectionUtils.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/TypeOperators.h>

#include <DinrusX/DinrusXSchema/IRuntimeClass.h>
#include <DinrusX/DinrusXSchema/IRuntimeRegistry.h>
#include <DinrusX/DinrusXSchema/RuntimeGraph.h>
#include <DinrusX/DinrusXSchema/RuntimeParamMap.h>
#include <DinrusX/DinrusXSchema/RuntimeParams.h>

#include <DinrusX/DinrusXSchema/IScript.h>
#include <DinrusX/DinrusXSchema/IScriptElement.h>
#include <DinrusX/DinrusXSchema/IScriptExtension.h>
#include <DinrusX/DinrusXSchema/IScriptGraph.h>
#include <DinrusX/DinrusXSchema/IScriptRegistry.h>
#include <DinrusX/DinrusXSchema/IScriptView.h>
#include <DinrusX/DinrusXSchema/ScriptDependencyEnumerator.h>
#include <DinrusX/DinrusXSchema/ScriptUtils.h>

#include <DinrusX/DinrusXSchema/IScriptActionInstance.h>
#include <DinrusX/DinrusXSchema/IScriptBase.h>
#include <DinrusX/DinrusXSchema/IScriptClass.h>
#include <DinrusX/DinrusXSchema/IScriptComponentInstance.h>
#include <DinrusX/DinrusXSchema/IScriptConstructor.h>
#include <DinrusX/DinrusXSchema/IScriptEnum.h>
#include <DinrusX/DinrusXSchema/IScriptFunction.h>
#include <DinrusX/DinrusXSchema/IScriptInterface.h>
#include <DinrusX/DinrusXSchema/IScriptInterfaceFunction.h>
#include <DinrusX/DinrusXSchema/IScriptInterfaceImpl.h>
#include <DinrusX/DinrusXSchema/IScriptInterfaceTask.h>
#include <DinrusX/DinrusXSchema/IScriptModule.h>
#include <DinrusX/DinrusXSchema/IScriptRoot.h>
#include <DinrusX/DinrusXSchema/IScriptSignal.h>
#include <DinrusX/DinrusXSchema/IScriptSignalReceiver.h>
#include <DinrusX/DinrusXSchema/IScriptState.h>
#include <DinrusX/DinrusXSchema/IScriptStateMachine.h>
#include <DinrusX/DinrusXSchema/IScriptStruct.h>
#include <DinrusX/DinrusXSchema/IScriptTimer.h>
#include <DinrusX/DinrusXSchema/IScriptVariable.h>

#include <DinrusX/DinrusXSchema/ContainerSerializationUtils.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/IValidatorArchive.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>
#include <DinrusX/DinrusXSchema/SerializationQuickSearch.h>
#include <DinrusX/DinrusXSchema/SerializationToString.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>

#include <DinrusX/DinrusXSchema/ILog.h>
#include <DinrusX/DinrusXSchema/ILogRecorder.h>
#include <DinrusX/DinrusXSchema/ISettingsUpr.h>
#include <DinrusX/DinrusXSchema/ITimerSystem.h>
#include <DinrusX/DinrusXSchema/IUpdateScheduler.h>
#include <DinrusX/DinrusXSchema/LogMetaData.h>
#include <DinrusX/DinrusXSchema/LogStreamName.h>

#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/AnyArray.h>
#include <DinrusX/DinrusXSchema/Array.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/DrxLinkUtils.h>
#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>
#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/HybridArray.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>
#include <DinrusX/DinrusXSchema/IInterfaceMap.h>
#include <DinrusX/DinrusXSchema/IString.h>
#include <DinrusX/DinrusXSchema/PreprocessorUtils.h>
#include <DinrusX/DinrusXSchema/RingBuffer.h>
#include <DinrusX/DinrusXSchema/Rotation.h>
#include <DinrusX/DinrusXSchema/ScopedConnection.h>
#include <DinrusX/DinrusXSchema/Scratchpad.h>
#include <DinrusX/DinrusXSchema/SharedString.h>
#include <DinrusX/DinrusXSchema/Signal.h>
#include <DinrusX/DinrusXSchema/StackString.h>
#include <DinrusX/DinrusXSchema/STLUtils.h>
#include <DinrusX/DinrusXSchema/StringHashWrapper.h>
#include <DinrusX/DinrusXSchema/StringUtils.h>
#include <DinrusX/DinrusXSchema/Transform.h>
#include <DinrusX/DinrusXSchema/TypeName.h>
#include <DinrusX/DinrusXSchema/TypeUtils.h>
#include <DinrusX/DinrusXSchema/UniqueId.h>
#include <DinrusX/DinrusXSchema/Validator.h>
