// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAct/ClassRegistryReplicator.h>

void AddOnClientConnect(IContextEstablisher* pEst, EContextViewState state, bool isReset);
void AddOnClientEnteredGame(IContextEstablisher* pEst, EContextViewState state, bool isReset);