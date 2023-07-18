// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __CET_NETCONFIG_H__
#define __CET_NETCONFIG_H__

#pragma once

void AddEstablishedContext(IContextEstablisher* pEst, EContextViewState state, int token);
void AddDeclareWitness(IContextEstablisher* pEst, EContextViewState state);
void AddDelegateAuthorityToClientActor(IContextEstablisher* pEst, EContextViewState state);
void AddClearPlayerIds(IContextEstablisher* pEst, EContextViewState state);

#endif
