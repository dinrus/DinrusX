// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/BehaviorTreeNodeRegistration.h>

#include <DinrusX/DinrusXAI/BehaviorTreeNodes_Core.h>
#include <DinrusX/DinrusXAI/BehaviorTreeNodes_AI.h>
#include <DinrusX/DinrusXAI/BehaviorTreeNodes_Helicopter.h>
#include <DinrusX/DinrusXAI/BehaviorTreeNodes_Basic.h>

namespace BehaviorTree
{
void RegisterBehaviorTreeNodes()
{
	RegisterBehaviorTreeNodes_Core();
	RegisterBehaviorTreeNodes_AI();
	RegisterBehaviorTreeNodesHelicopter();
	RegisterBehaviorTreeNodes_Basic();
}
}
