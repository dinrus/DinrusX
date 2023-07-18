// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXEntity/IEntityComponent.h>
#include <DinrusX/DinrusXAI/IBehaviorTree.h>

struct IEntityBehaviorTreeComponent : public IEntityComponent
{
	static void ReflectType(Schematyc::CTypeDesc<IEntityBehaviorTreeComponent>& desc)
	{
		desc.SetGUID("C6B972F7-2878-4498-8257-C7D62653FA50"_drx_guid);
	}

	virtual bool IsRunning() const = 0;
	virtual void SendEvent(const char* szEventName) = 0;
	
	template <class TValue>
	bool SetBBKeyValue(const char* szKeyName, const TValue& value)
	{		
		if (szKeyName == nullptr)
			return false;
		
		if (!IsRunning())
			return false;

		if (BehaviorTree::Blackboard* pBlackboard = gEnv->pAISystem->GetIBehaviorTreeUpr()->GetBehaviorTreeBlackboard(GetEntityId()))
		{
			return pBlackboard->SetVariable(BehaviorTree::BlackboardVariableId(szKeyName), value);
		}
		return false;
	}
};