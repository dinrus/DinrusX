// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAnimation/IAnimEventPlayer.h>
#include <DrxExtension/ClassWeaver.h>
#include <DrxExtension/DrxCreateClassInstance.h>

// In order to be found by the Character Tool, the class name must match the value of the 'sys_game_name' CVar
// (replace spaces with underscores in the class name).
class AnimEventPlayer_DRXENGINE_SDK : public IAnimEventPlayer
{
public:
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(IAnimEventPlayer)
	DRXINTERFACE_END()
	DRXGENERATE_CLASS(AnimEventPlayer_DRXENGINE_SDK, "AnimEventPlayer_DRXENGINE_SDK", 0x7cb241402ca9e311, 0x058e7ce6b77865e4)

	const SCustomAnimEventType* GetCustomType(int customTypeIndex) const override;
	int GetCustomTypeCount() const override;
	bool Play(ICharacterInstance* character, const AnimEventInstance& event) override;
};
