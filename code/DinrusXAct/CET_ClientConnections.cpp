// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/CET_GameRules.h>
#include <DinrusX/DinrusXAct/IGameRulesSystem.h>
#include <DinrusX/DinrusXAct/GameClientChannel.h>
#include <DinrusX/DinrusXAct/GameServerChannel.h>
#include <DinrusX/DinrusXAct/GameContext.h>
#include <DinrusX/DinrusXAct/ActionGame.h>

#include <DinrusX/DinrusXNetwork/NetHelpers.h>
#include <DinrusX/DinrusXNetwork/INetwork.h>

class CCET_OnClient : public CCET_Base
{
public:
	CCET_OnClient(bool(INetworkedClientListener::* func)(int, bool), const char* name, bool isReset) : m_func(func), m_name(name), m_isReset(isReset) {}

	const char*                 GetName() { return m_name; }

	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		if (INetChannel* pNC = state.pSender)
		{
			if (CGameServerChannel* pSC = (CGameServerChannel*)pNC->GetGameChannel())
			{
				for (INetworkedClientListener* pListener : CDrxAction::GetDrxAction()->GetNetworkClientListeners())
				{
					if (!(pListener->*m_func)(pSC->GetChannelId(), m_isReset))
					{
						return eCETR_Failed;
					}
				}

				return eCETR_Ok;
			}
		}
		
		GameWarning("%s: No channel id", m_name);
		return eCETR_Failed;
	}

private:
	bool (INetworkedClientListener::* m_func)(int, bool);
	const char* m_name;
	bool        m_isReset;
};

void AddOnClientConnect(IContextEstablisher* pEst, EContextViewState state, bool isReset)
{
	if (!(gEnv->bHostMigrating && gEnv->bMultiplayer))
	{
		pEst->AddTask(state, new CCET_OnClient(&INetworkedClientListener::OnClientConnectionReceived, "OnClientConnectionReceived", isReset));
	}
}

void AddOnClientEnteredGame(IContextEstablisher* pEst, EContextViewState state, bool isReset)
{
	if (!(gEnv->bHostMigrating && gEnv->bMultiplayer))
	{
		pEst->AddTask(state, new CCET_OnClient(&INetworkedClientListener::OnClientReadyForGameplay, "OnClientReadyForGameplay", isReset));
	}
}