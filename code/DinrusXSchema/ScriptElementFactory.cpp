// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptElementFactory.h>

#include <DinrusX/DinrusXSchema/ScriptActionInstance.h>
#include <DinrusX/DinrusXSchema/ScriptBase.h>
#include <DinrusX/DinrusXSchema/ScriptClass.h>
#include <DinrusX/DinrusXSchema/ScriptComponentInstance.h>
#include <DinrusX/DinrusXSchema/ScriptConstructor.h>
#include <DinrusX/DinrusXSchema/ScriptEnum.h>
#include <DinrusX/DinrusXSchema/ScriptFunction.h>
#include <DinrusX/DinrusXSchema/ScriptInterface.h>
#include <DinrusX/DinrusXSchema/ScriptInterfaceFunction.h>
#include <DinrusX/DinrusXSchema/ScriptInterfaceImpl.h>
#include <DinrusX/DinrusXSchema/ScriptInterfaceTask.h>
#include <DinrusX/DinrusXSchema/ScriptModule.h>
#include <DinrusX/DinrusXSchema/ScriptSignal.h>
#include <DinrusX/DinrusXSchema/ScriptSignalReceiver.h>
#include <DinrusX/DinrusXSchema/ScriptState.h>
#include <DinrusX/DinrusXSchema/ScriptStateMachine.h>
#include <DinrusX/DinrusXSchema/ScriptStruct.h>
#include <DinrusX/DinrusXSchema/ScriptTimer.h>
#include <DinrusX/DinrusXSchema/ScriptVariable.h>

namespace Schematyc
{
IScriptElementPtr CScriptElementFactory::CreateElement(EScriptElementType elementType)
{
	switch (elementType)
	{
	case EScriptElementType::Module:
		{
			return std::make_shared<CScriptModule>();
		}
	case EScriptElementType::Enum:
		{
			return std::make_shared<CScriptEnum>();
		}
	case EScriptElementType::Struct:
		{
			return std::make_shared<CScriptStruct>();
		}
	case EScriptElementType::Signal:
		{
			return std::make_shared<CScriptSignal>();
		}
	case EScriptElementType::Constructor:
		{
			return std::make_shared<CScriptConstructor>();
		}
	case EScriptElementType::Function:
		{
			return std::make_shared<CScriptFunction>();
		}
	case EScriptElementType::Interface:
		{
			return std::make_shared<CScriptInterface>();
		}
	case EScriptElementType::InterfaceFunction:
		{
			return std::make_shared<CScriptInterfaceFunction>();
		}
	case EScriptElementType::InterfaceTask:
		{
			return std::make_shared<CScriptInterfaceTask>();
		}
	case EScriptElementType::Class:
		{
			return std::make_shared<CScriptClass>();
		}
	case EScriptElementType::Base:
		{
			return std::make_shared<CScriptBase>();
		}
	case EScriptElementType::StateMachine:
		{
			return std::make_shared<CScriptStateMachine>();
		}
	case EScriptElementType::State:
		{
			return std::make_shared<CScriptState>();
		}
	case EScriptElementType::Variable:
		{
			return std::make_shared<CScriptVariable>();
		}
	case EScriptElementType::Timer:
		{
			return std::make_shared<CScriptTimer>();
		}
	case EScriptElementType::SignalReceiver:
		{
			return std::make_shared<CScriptSignalReceiver>();
		}
	case EScriptElementType::InterfaceImpl:
		{
			return std::make_shared<CScriptInterfaceImpl>();
		}
	case EScriptElementType::ComponentInstance:
		{
			return std::make_shared<CScriptComponentInstance>();
		}
	case EScriptElementType::ActionInstance:
		{
			return std::make_shared<CScriptActionInstance>();
		}
	}
	return IScriptElementPtr();
}
} // Schematyc
