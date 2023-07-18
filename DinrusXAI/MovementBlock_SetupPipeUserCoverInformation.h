// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/MovementBlock.h>

namespace Movement
{
namespace MovementBlocks
{
class SetupPipeUserCoverInformation : public Movement::Block
{
public:
	SetupPipeUserCoverInformation(CPipeUser& pipeUser)
		: m_pipeUser(pipeUser)
	{}
	virtual void        Begin(IMovementActor& actor);
	virtual const char* GetName() const { return "SetCoverInfo"; }
private:
	CPipeUser& m_pipeUser;
};
}
}
