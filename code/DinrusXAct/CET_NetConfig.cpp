// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/CET_NetConfig.h>
#include <DinrusX/DinrusXAct/GameContext.h>
#include <DinrusX/DinrusXAct/GameClientChannel.h>
#include <DinrusX/DinrusXAct/GameClientNub.h>
#include <DinrusX/DinrusXAct/GameServerChannel.h>
#include <DinrusX/DinrusXAct/GameServerNub.h>
#include <DinrusX/DinrusXNetwork/NetHelpers.h>

/*
 * Established context
 */

class CCET_EstablishedContext : public CCET_Base
{
public:
	CCET_EstablishedContext(int token) : m_token(token) {}

	const char*                 GetName() { return "EstablishedContext"; }

	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		CDrxAction::GetDrxAction()->GetGameContext()->GetNetContext()->EstablishedContext(m_token);
		return eCETR_Ok;
	}

private:
	int m_token;
};

void AddEstablishedContext(IContextEstablisher* pEst, EContextViewState state, int token)
{
	pEst->AddTask(state, new CCET_EstablishedContext(token));
}

/*
 * Declare witness
 */

class CCET_DeclareWitness : public CCET_Base
{
public:
	const char*                 GetName() { return "DeclareWitness"; }

	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		if (CGameClientNub* pNub = CDrxAction::GetDrxAction()->GetGameClientNub())
		{
			if (CGameClientChannel* pChannel = pNub->GetGameClientChannel())
			{
				if (IActor* pActor = CDrxAction::GetDrxAction()->GetClientActor())
				{
					pChannel->GetNetChannel()->DeclareWitness(pActor->GetEntityId());
					return eCETR_Ok;
				}
			}
		}
		return eCETR_Failed;
	}
};

void AddDeclareWitness(IContextEstablisher* pEst, EContextViewState state)
{
	pEst->AddTask(state, new CCET_DeclareWitness);
}

/*
 * Delegate authority to player
 */

class CCET_DelegateAuthority_ToClientActor : public CCET_Base
{
public:
	const char* GetName() { return "DelegateAuthorityToClientActor"; }

public:
	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		EntityId entityId = GetEntity(state);
		if (!entityId || !gEnv->pEntitySystem->GetEntity(entityId))
			return eCETR_Ok; // Proceed even if there is no Actor
		CDrxAction::GetDrxAction()->GetGameContext()->GetNetContext()->DelegateAuthority(entityId, state.pSender);
		return eCETR_Ok;
	}

private:
	EntityId GetEntity(SContextEstablishState& state)
	{
		if (!state.pSender)
			return 0;
		CGameChannel* pGameChannel = static_cast<CGameChannel*>(state.pSender->GetGameChannel());
		if (pGameChannel)
			return pGameChannel->GetPlayerId();
		return 0;
	}
};

void AddDelegateAuthorityToClientActor(IContextEstablisher* pEst, EContextViewState state)
{
	pEst->AddTask(state, new CCET_DelegateAuthority_ToClientActor());
}

/*
 * Clear player ids
 */

class CCET_ClearPlayerIds : public CCET_Base
{
public:
	const char*                 GetName() { return "ClearPlayerIds"; }

	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		if (CGameServerNub* pNub = CDrxAction::GetDrxAction()->GetGameServerNub())
		{
			TServerChannelMap* pMap = pNub->GetServerChannelMap();
			for (TServerChannelMap::iterator it = pMap->begin(); it != pMap->end(); ++it)
				it->second->ResetPlayerId();
		}
		if (CGameClientNub* pCNub = CDrxAction::GetDrxAction()->GetGameClientNub())
		{
			if (CGameChannel* pChannel = pCNub->GetGameClientChannel())
				pChannel->ResetPlayerId();
		}
		return eCETR_Ok;
	}
};

void AddClearPlayerIds(IContextEstablisher* pEst, EContextViewState state)
{
	pEst->AddTask(state, new CCET_ClearPlayerIds());
}
