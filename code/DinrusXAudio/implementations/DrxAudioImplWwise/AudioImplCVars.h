// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace DrxAudio
{
namespace Impl
{
namespace Wwise
{
class CImpl;

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
	void    SetImpl(CImpl* const pImpl) { s_pImpl = pImpl; }

	int m_secondaryMemoryPoolSize = 0;
	int m_prepareEventMemoryPoolSize = 0;
	int m_streamUprMemoryPoolSize = 0;
	int m_streamDeviceMemoryPoolSize = 0;
	int m_soundEngineDefaultMemoryPoolSize = 0;
	int m_commandQueueMemoryPoolSize = 0;
	int m_lowerEngineDefaultPoolSize = 0;
	int m_enableEventUprThread = 0;
	int m_enableSoundBankUprThread = 0;
	int m_panningRule = 0;

#if defined(INCLUDE_WWISE_IMPL_PRODUCTION_CODE)
	int m_enableCommSystem = 0;
	int m_enableOutputCapture = 0;
	int m_monitorMemoryPoolSize = 0;
	int m_monitorQueueMemoryPoolSize = 0;
#endif  // INCLUDE_WWISE_IMPL_PRODUCTION_CODE

	static CImpl* s_pImpl;
};

extern CCVars g_cvars;
} // namespace Wwise
} // namespace Impl
} // namespace DrxAudio
