// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace DrxAudio
{
namespace Impl
{
namespace Fmod
{
class CCVars final
{
public:

	CCVars() = default;
	CCVars(CCVars const&) = delete;
	CCVars(CCVars&&) = delete;
	CCVars& operator=(CCVars const&) = delete;
	CCVars& operator=(CCVars&&) = delete;

	void    RegisterVariables();
	void    UnregisterVariables();

	int   m_maxChannels = 0;
	int   m_enableLiveUpdate = 0;
	int   m_enableSynchronousUpdate = 1;

	float m_lowpassMinCutoffFrequency = 10.0f;
	float m_distanceFactor = 1.0f;
	float m_dopplerScale = 1.0f;
	float m_rolloffScale = 1.0f;

#if DRX_PLATFORM_DURANGO
	int m_secondaryMemoryPoolSize = 0;
#endif  // DRX_PLATFORM_DURANGO

#if defined(INCLUDE_FMOD_IMPL_PRODUCTION_CODE)
#endif  // INCLUDE_FMOD_IMPL_PRODUCTION_CODE
};

extern CCVars g_cvars;
} // namespace Fmod
} // namespace Impl
} // namespace DrxAudio
