// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: Allows game modules system to talk to IGameStatistics system

	-------------------------------------------------------------------------
	History:
	- 20:10:2009  : Created by Mark Tully

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/GameRulesStatsRecording.h>
#include <DinrusX/DinrusXSys/XML/IXml.h>
#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/CoreX/Game/IGameStatistics.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXGame/StatsRecordingMgr.h>
#include <DinrusX/DinrusXGame/StatHelpers.h>

void CGameRulesStatsRecording::Init(
	XmlNodeRef			inXML)
{
	m_gameStatistics=g_pGame->GetIGameFramework()->GetIGameStatistics();
	m_actorSystem=g_pGame->GetIGameFramework()->GetIActorSystem();
	m_statsMgr=g_pGame->GetStatsRecorder();
	assert(m_gameStatistics);
	assert(m_actorSystem);
}

void CGameRulesStatsRecording::OnInGameBegin()
{
	if (m_statsMgr)
	{
		m_statsMgr->BeginRound();
	}
}

void CGameRulesStatsRecording::OnPostGameBegin()
{
	if (m_statsMgr)
	{
		m_statsMgr->StopTrackingAllPlayerStats();
		m_statsMgr->EndRound();
	}
}

void CGameRulesStatsRecording::OnGameEnd()
{
	if (m_statsMgr)
	{
		m_statsMgr->EndRound();
	}
}

void CGameRulesStatsRecording::OnPlayerRevived(
	IActor			*inActor)
{
	if (m_statsMgr)
	{
		m_statsMgr->StartTrackingStats(inActor);
	}
}

void CGameRulesStatsRecording::OnPlayerKilled(
	IActor			*inActor)
{
	if (m_statsMgr)
	{
		m_statsMgr->StopTrackingStats(inActor);
	}
}

#if USE_PC_PREMATCH
void CGameRulesStatsRecording::OnPrematchEnd(
	IActor			*inActor)
{
	if (m_statsMgr)
	{
		m_statsMgr->StopTrackingStats(inActor);
	}
}
#endif 