// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/IAIBubblesSystem.h>

bool AIQueueBubbleMessage(const char* messageName, const EntityId entityID,
                          const char* message, uint32 flags, float duration = 0, SAIBubbleRequest::ERequestType requestType = SAIBubbleRequest::eRT_ErrorMessage);
