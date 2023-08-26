// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/GameContext.h>
#include <DinrusX/DinrusXAct/VoiceListener.h>

void CGameContext::RegisterExtensions(IGameFramework* pFW)
{
#ifndef OLD_VOICE_SYSTEM_DEPRECATED
	REGISTER_FACTORY(pFW, "VoiceListener", CVoiceListener, false);
#endif
}
