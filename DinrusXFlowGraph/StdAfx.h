// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define eDrxModule eDrxM_FlowGraph

#include <DinrusX/CoreX/Platform/platform.h>

#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXScript/IScriptSystem.h>

#include <DinrusX/DinrusXEntity/IEntity.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXEntity/IEntityComponent.h>

#include <DinrusX/DinrusXPhys/IPhysics.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/CoreX/Game/IGameTokens.h>
#include <DinrusX/DinrusXInput/IInput.h>
#include <DinrusX/CoreX/Renderer/IRenderer.h>
#include <DinrusX/CoreX/Game/IGame.h>
#include <DinrusX/CoreX/Game/IGameFramework.h>
#include <DinrusX/DinrusXFlowGraph/IFlowSystem.h>

#include <DinrusX/DinrusXAI/IAISystem.h>

// Include DinrusAction specific headers
#include <DinrusX/DinrusXAct/IActorSystem.h>
#include <DinrusX/DinrusXAct/IViewSystem.h>
#include <DinrusX/DinrusXAct/ILevelSystem.h>
#include <DinrusX/DinrusXAct/IGameRulesSystem.h>
#include <DinrusX/DinrusXAct/IVehicleSystem.h>
#include <DinrusX/DinrusXAct/IForceFeedbackSystem.h>
#include <DinrusX/DinrusXAct/IAnimatedCharacter.h>
#include <DinrusX/DinrusXAct/IItemSystem.h>

#include "HelperFunctions.h"