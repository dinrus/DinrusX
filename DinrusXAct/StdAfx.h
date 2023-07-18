// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   DrxGame Source File.
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	include file for standard system include files,	or project
                specific include files that are used frequently, but are
                changed infrequently

   -------------------------------------------------------------------------
   History:
   - 20:7:2004   10:51 : Created by Marco Koegler

*************************************************************************/
#if !defined(AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E42__INCLUDED_)
#define AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E42__INCLUDED_

//#define _CRTDBG_MAP_ALLOC

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule  eDrxM_GameFramework
#define RWI_NAME_TAG "RayWorldIntersection(Action)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(Action)"

#define DRXACTION_EXPORTS

// Insert your headers here
#include <DinrusX/CoreX/Platform/platform.h>

#include <memory>
#include <vector>
#include <map>
#include <queue>

inline void GameWarning(const char*, ...) PRINTF_PARAMS(1, 2);

#include <DinrusX/CoreX/Project/ProjectDefines.h>
#include <DinrusX/CoreX/StlUtils.h>
#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Camera.h>
#include <DinrusX/CoreX/Math/Random.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXNetwork/INetwork.h>
#include <DinrusX/DinrusXInput/IInput.h>
#include <DinrusX/DinrusXScript/IScriptSystem.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXNetwork/NetHelpers.h>
#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXSys/ILog.h>
#include <DinrusX/DinrusXNetwork/IRemoteControl.h>
#include <DinrusX/DinrusXNetwork/ISimpleHttpServer.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>
#include <DinrusX/CoreX/Game/IGameFramework.h>
#include <DinrusX/DinrusXAct/IActorSystem.h>
#include <DinrusX/DinrusXAct/IAnimatedCharacter.h>
#include <DinrusX/CoreX/Game/IGame.h>
#include <DinrusX/DinrusXAct/IItem.h>
#include <DinrusX/DinrusXAct/IItemSystem.h>
#include <DinrusX/DinrusXAct/IViewSystem.h>
#include <DinrusX/DinrusXAct/IVehicleSystem.h>
#include <DinrusX/DinrusXFlowGraph/IFlowSystem.h>
#include <DinrusX/DinrusXAct/IGameplayRecorder.h>
#include <DinrusX/CoreX/Game/GameUtils.h>

#pragma warning(disable: 4018)  // conditional expression is constant
#pragma warning(disable: 4018)  // conditional expression is constant
#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated

#if !defined(_RELEASE)
	#define ENABLE_NETDEBUG 1
#endif

//////////////////////////////////////////////////////////////////////////
//! Reports a Game Warning to validator with WARNING severity.
inline void GameWarning(const char* format, ...)
{
	if (!format)
		return;

	va_list args;
	va_start(args, format);
	GetISystem()->WarningV(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, 0, 0, format, args);
	va_end(args);
}

#if 1
	#define NET_USE_SIMPLE_BREAKAGE 1
#else// deprecated and won't compile
	#define NET_USE_SIMPLE_BREAKAGE 0
#endif

#if !defined(RELEASE)
	#define DRXACTION_AI_VERBOSITY
#endif

#ifdef DRXACTION_AI_VERBOSITY
	#define AIWarningID     gEnv->pAISystem->Warning
	#define AIErrorID       gEnv->pAISystem->Error
	#define AILogProgressID gEnv->pAISystem->LogProgress
	#define AILogEventID    gEnv->pAISystem->LogEvent
	#define AILogCommentID  gEnv->pAISystem->LogComment
#else
	#define AIWarningID     (void)
	#define AIErrorID       (void)
	#define AILogProgressID (void)
	#define AILogEventID    (void)
	#define AILogCommentID  (void)
#endif

inline bool IsClientActor(EntityId id)
{
	IActor* pActor = CDrxAction::GetDrxAction()->GetClientActor();
	if (pActor && pActor->GetEntity()->GetId() == id)
		return true;
	return false;
}

template<typename T> bool inline GetAttr(const XmlNodeRef& node, const char* key, T& val)
{
	return node->getAttr(key, val);
}

bool inline GetTimeAttr(const XmlNodeRef& node, const char* key, time_t& val)
{
	const char* pVal = node->getAttr(key);
	if (!pVal)
		return false;
	val = GameUtils::stringToTime(pVal);
	return true;
}

template<> bool inline GetAttr(const XmlNodeRef& node, const char* key, string& val)
{
	const char* pVal = node->getAttr(key);
	if (!pVal)
		return false;
	val = pVal;
	return true;
}

#endif // !defined(AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E42__INCLUDED_)
