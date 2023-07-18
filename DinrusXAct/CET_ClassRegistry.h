// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __CET_CLASSREGISTRY_H__
#define __CET_CLASSREGISTRY_H__

#pragma once

#include <DinrusX/DinrusXAct/ClassRegistryReplicator.h>

void AddRegisterAllClasses(IContextEstablisher* pEst, EContextViewState state, CClassRegistryReplicator* pRep);
void AddSendClassRegistration(IContextEstablisher* pEst, EContextViewState state, CClassRegistryReplicator* pRep, std::vector<SSendableHandle>** ppWaitFor);
void AddSendClassHashRegistration(IContextEstablisher* pEst, EContextViewState state, CClassRegistryReplicator* pRep, std::vector<SSendableHandle>** ppWaitFor);

#endif
