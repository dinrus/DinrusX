// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXEntity/IEntityComponent.h>

struct IEntityFactionComponent : public IEntityComponent
{
	virtual uint8 GetFactionId() const = 0;
	virtual void SetFactionId(const uint8 factionId) = 0;
	virtual IFactionMap::ReactionType GetReaction(const EntityId otherEntityId) const = 0;
	virtual void SetReactionChangedCallback(std::function<void(const uint8, const IFactionMap::ReactionType)> callbackFunction) = 0;

	static void ReflectType(Schematyc::CTypeDesc<IEntityFactionComponent>& desc)
	{
		desc.SetGUID("C0B0554B-FBA5-402B-905A-EAE1D5B1A527"_drx_guid);
	}
};