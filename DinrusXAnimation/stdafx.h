// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_Animation

#define DRXANIMATION_EXPORTS
//#define NOT_USE_DRX_MEMORY_MANAGER
//#define DEFINE_MODULE_NAME "DinrusXAnimation"

//! Include standard headers.
#include <DinrusX/CoreX/Platform/platform.h>

#if defined(USER_ivof) || defined(USER_ivo) || defined(USER_sven)
	#define ASSERT_ON_ASSET_CHECKS
#endif

#ifdef ASSERT_ON_ASSET_CHECKS
	#define ANIM_ASSET_ASSERT_TRACE(condition, parenthese_message) DRX_ASSERT_TRACE(condition, parenthese_message)
	#define ANIM_ASSET_ASSERT(condition)                           DRX_ASSERT(condition)
#else
	#define ANIM_ASSET_ASSERT_TRACE(condition, parenthese_message)
	#define ANIM_ASSET_ASSERT(condition)
#endif

#define LOG_ASSET_PROBLEM(condition, parenthese_message)                \
  do                                                                    \
  {                                                                     \
    gEnv->pLog->LogWarning("Anim asset error: %s failed", # condition); \
    gEnv->pLog->LogWarning parenthese_message;                          \
  } while (0)

#define ANIM_ASSET_CHECK_TRACE(condition, parenthese_message)     \
  do                                                              \
  {                                                               \
    bool anim_asset_ok = (condition);                             \
    if (!anim_asset_ok)                                           \
    {                                                             \
      ANIM_ASSET_ASSERT_TRACE(anim_asset_ok, parenthese_message); \
      LOG_ASSET_PROBLEM(condition, parenthese_message);           \
    }                                                             \
  } while (0)

#define ANIM_ASSET_CHECK_MESSAGE(condition, message) ANIM_ASSET_CHECK_TRACE(condition, (message))
#define ANIM_ASSET_CHECK(condition)                  ANIM_ASSET_CHECK_MESSAGE(condition, NULL)

//! Include main interfaces.
#include <DinrusX/CoreX/Math/Drx_Math.h>

#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/ITimer.h>
#include <DinrusX/DinrusXSys/ILog.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/CoreX/StlUtils.h>

#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXAnimation/AnimationBase.h>
#include <DinrusX/DinrusXSys/FrameProfiler_JobSystem.h>  // to be removed
