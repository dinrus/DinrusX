// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/IAIObject.h>

class IAISystemComponent
{
	friend struct IAISystem;

public:
	virtual ~IAISystemComponent() {}

	virtual void Init() {}
	virtual void PostInit() {}

	virtual void Reset(IAISystem::EResetReason reason) {}
	virtual void Serialize(TSerialize ser) {}

	virtual void Update(float frameDelta) {}
	virtual void ActorUpdate(IAIObject* pAIObject, IAIObject::EUpdateType type, float frameDelta) {}
	virtual bool WantActorUpdates(IAIObject::EUpdateType type) { return false; }

	virtual void DebugDraw(IAIDebugRenderer* pDebugRenderer) {}
};

