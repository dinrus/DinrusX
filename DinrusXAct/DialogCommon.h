// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DialogCommon.h
//  Version:     v1.00
//  Created:     07/07/2006 by AlexL
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: Dialog Common Include file
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DIALOGCOMMON_H__
#define __DIALOGCOMMON_H__

#pragma once

#include <DinrusX/DinrusXSys/ISystem.h>
#include "DialogSystem.h"

namespace DiaLOG
{
enum ELevel
{
	eAlways = 1,
	eDebugA = 2,
	eDebugB = 3,
	eDebugC = 4
};

inline void Log(DiaLOG::ELevel level, const char*, ...) PRINTF_PARAMS(2, 3);

//////////////////////////////////////////////////////////////////////////
//! Reports a Game Warning to validator with WARNING severity.
inline void Log(DiaLOG::ELevel level, const char* format, ...)
{
	if (gEnv->pSystem && (int)level <= CDialogSystem::sDiaLOGLevel)
	{
		va_list args;
		va_start(args, format);
		gEnv->pLog->LogV(ILog::eMessage, format, args);
		va_end(args);
	}
}

};

#endif
