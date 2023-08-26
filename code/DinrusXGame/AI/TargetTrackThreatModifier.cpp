// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Game-side logic for controlling the threat level of an AI
				agent's current target
  
 -------------------------------------------------------------------------
  History:
  - 09:08:2010: Created by Kevin Kirst

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/TargetTrackThreatModifier.h>
#include <DinrusX/DinrusXGame/GameCVars.h>
#include <DinrusX/DinrusXAI/IAIObject.h>
#include <DinrusX/DinrusXAI/IAIActor.h>
#include <DinrusX/DinrusXAI/IAIActorProxy.h>

//////////////////////////////////////////////////////////////////////////
CTargetTrackThreatModifier::CTargetTrackThreatModifier()
{
	ITargetTrackUpr *pTargetTrackUpr = gEnv->pAISystem->GetTargetTrackUpr();
	if (pTargetTrackUpr)
		pTargetTrackUpr->SetTargetTrackThreatModifier(this);

	IConsole *pConsole = gEnv->pConsole;
	assert(pConsole);

	m_pCVar_CloakMinDist = pConsole->GetCVar("ai_CloakMinDist");
	m_pCVar_CloakMaxDist = pConsole->GetCVar("ai_CloakMaxDist");
	m_pCVar_SOMSpeedRelaxed = pConsole->GetCVar("ai_SOMSpeedRelaxed");
	m_pCVar_SOMSpeedCombat = pConsole->GetCVar("ai_SOMSpeedCombat");
}

//////////////////////////////////////////////////////////////////////////
CTargetTrackThreatModifier::~CTargetTrackThreatModifier()
{
	ITargetTrackUpr *pTargetTrackUpr = gEnv->pAISystem->GetTargetTrackUpr();
	if (pTargetTrackUpr)
		pTargetTrackUpr->ClearTargetTrackThreatModifier();
}

//////////////////////////////////////////////////////////////////////////
void CTargetTrackThreatModifier::ModifyTargetThreat(IAIObject &ownerAI, IAIObject &targetAI, const ITargetTrack &track, float &outThreatRatio, EAITargetThreat &outThreat) const
{
	if (track.GetTargetType() > AITARGET_SOUND && outThreat >= AITHREAT_AGGRESSIVE)
	{
		outThreatRatio = 1.0f;
	}
}
