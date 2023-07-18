// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/MovementBlock_HarshStop.h>
#include <DinrusX/DinrusXAI/MovementUpdateContext.h>
#include <DinrusX/DinrusXAI/MovementActor.h>
#include <DinrusX/DinrusXAI/PipeUser.h>

namespace Movement
{
namespace MovementBlocks
{
void HarshStop::Begin(IMovementActor& actor)
{
	actor.GetAdapter().ClearMovementState();
}

Movement::Block::Status HarshStop::Update(const MovementUpdateContext& context)
{
	const bool stopped = !context.actor.GetAdapter().IsMoving();

	return stopped ? Finished : Running;
}
}
}
