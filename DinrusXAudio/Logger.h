// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSys/ISystem.h> // gEnv
#include <DinrusX/CoreX/Audio/IAudioSystem.h>

/**
 * @namespace Drx
 * @brief The global engine namespace.
 */
namespace Drx
{
/**
 * @namespace Drx::Audio
 * @brief Most parent audio namespace used throughout the engine.
 */
namespace Audio
{
/**
 * A utility function to log audio specific messages.
 * @param type - log message type (ELogType)
 * @param szFormat, ... - printf-style format string and its arguments
 */
template<typename ... Args>
static void Log(DrxAudio::ELogType const type, char const* const szFormat, Args&& ... args)
{
	if (gEnv->pAudioSystem != nullptr)
	{
		gEnv->pAudioSystem->Log(type, szFormat, std::forward<Args>(args) ...);
	}
}
} // namespace Drx::Audio
} // namespace Drx
