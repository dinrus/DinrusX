// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_DynamicResponseSystem
#include <DinrusX/CoreX/Platform/platform.h>

//forward declarations
namespace DRS
{
struct IVariable;
struct IVariableCollection;
struct IResponseActor;
struct IResponseAction;
struct IResponseActionInstance;
struct IResponseCondition;
}

#include <DinrusX/CoreX/Project/ProjectDefines.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/CoreX/String/DrxString.h>

#include <DinrusX/CoreX/Serialization/yasli/ConfigLocal.h>

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/CoreX/Serialization/ClassFactory.h>
#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/Enum.h>
#include <DinrusX/CoreX/Serialization/STL.h>

//Schematyc includes
#include <DinrusX/DinrusXSchema/CoreAPI.h>

#include <DinrusX/CoreX/String/HashedString.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseSystem.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseCondition.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>

#define DrsLogError(x) DrxFatalError("%s", x);

#if !defined(_RELEASE)
	#define DRS_COLLECT_DEBUG_DATA
	#define DrsLogWarning(x) DrxWarning(VALIDATOR_MODULE_DRS, VALIDATOR_WARNING, "DRS Warning: %s: %s", __FUNCTION__, x);
//#define DrsLogInfo(x) DrxLog("DRS Info: %s: %s", __FUNCTION__, x);
	#define DrsLogInfo(x)
#else
	#define DrsLogWarning(x)
	#define DrsLogInfo(x)
#endif

#define REGISTER_DRS_ACTION(_action, _actionname, _color)                       \
  namespace DRS {                                                               \
  SERIALIZATION_CLASS_NAME(IResponseAction, _action, _actionname, _actionname); \
  SERIALIZATION_CLASS_ANNOTATION(IResponseAction, _action, "color", _color); }

#define REGISTER_DRS_CONDITION(_condition, _conditionname, _color)                          \
  namespace DRS {                                                                           \
  SERIALIZATION_CLASS_NAME(IResponseCondition, _condition, _conditionname, _conditionname); \
  SERIALIZATION_CLASS_ANNOTATION(IResponseCondition, _condition, "color", _color); }
