// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/IDrxMannequin.h>
#include <DrxExtension/ClassWeaver.h>
#include <DinrusX/DinrusXAI/IAgent.h>

#include <DinrusX/DinrusXGame/Player.h>
#include <DinrusX/DinrusXGame/PlayerAI.h>


class CProceduralClipCompromiseCover : public TProceduralClip<SNoProceduralParams>
{
protected:
	virtual void OnEnter( float blendTime, float duration, const SNoProceduralParams& params )
	{
		const EntityId actorEntityID = m_scope->GetEntityId();
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity( actorEntityID );
		IF_UNLIKELY( !pEntity )
			return;

		IAIObject* pAI = pEntity->GetAI();
		IF_UNLIKELY( !pAI )
			return;

		IPipeUser* pPipeUser = pAI->CastToIPipeUser();
		IF_UNLIKELY( !pPipeUser )
			return;

		CPlayer* pPlayer = static_cast<CPlayer*>( g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor( actorEntityID ) );
		IF_UNLIKELY( !pPlayer )
			return;

		CAIAnimationComponent* pAIAnimationComponent = pPlayer->GetAIAnimationComponent();
		IF_UNLIKELY( !pAIAnimationComponent )
			return;

		pPipeUser->SetCoverCompromised();

		DoExplicitStanceChange( *pPlayer, *pAIAnimationComponent, STANCE_STAND );
	}

	virtual void Update( float timePassed )
	{
	}

	virtual void OnExit( float blendTime )
	{
	}

	void DoExplicitStanceChange( CPlayer &player, CAIAnimationComponent& pAIAnimationComponent, EStance targetStance )
	{
		if ( targetStance != STANCE_NULL )
		{
			player.SetStance( targetStance );

			CAIAnimationState& aiAnimationState = pAIAnimationComponent.GetAnimationState();
			aiAnimationState.SetRequestedStance( targetStance );
			aiAnimationState.SetStance( targetStance );
		}
	}
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipCompromiseCover, "CompromiseCover");
