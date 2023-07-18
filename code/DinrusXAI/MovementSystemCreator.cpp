// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/MovementSystemCreator.h>
#include <DinrusX/DinrusXAI/MovementSystem.h>

IMovementSystem* MovementSystemCreator::CreateMovementSystem() const
{
	return new MovementSystem();
}
