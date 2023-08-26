// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/AnimEventPlayer.h>

#include <DinrusX/CoreX/DrxCustomTypes.h>

// A list of all anim events that are exposed by this player
static SCustomAnimEventType g_sdkEvents[] = {
 	{ "swimmingStroke", 0, "Swimming strokes that cause ripples on the water surface." },
	};

AnimEventPlayer_DRXENGINE_SDK::AnimEventPlayer_DRXENGINE_SDK()
{
}

AnimEventPlayer_DRXENGINE_SDK::~AnimEventPlayer_DRXENGINE_SDK()
{
}

const SCustomAnimEventType* AnimEventPlayer_DRXENGINE_SDK::GetCustomType(int customTypeIndex) const
{
	const int count = GetCustomTypeCount();
	if (customTypeIndex < 0 || customTypeIndex >= count)
		return 0;
	return &g_sdkEvents[customTypeIndex];
}

int AnimEventPlayer_DRXENGINE_SDK::GetCustomTypeCount() const
{
	return DRX_ARRAY_COUNT(g_sdkEvents);
}

bool AnimEventPlayer_DRXENGINE_SDK::Play(ICharacterInstance* character, const AnimEventInstance& event)
{
	return false;
}

DRXREGISTER_CLASS(AnimEventPlayer_DRXENGINE_SDK)
