// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/SubtitleUpr.h>
#include <DinrusX/DinrusXAct/ISubtitleUpr.h>
#include <DinrusX/DinrusXAct/DialogActorContext.h>

CSubtitleUpr* CSubtitleUpr::s_Instance = nullptr;

//////////////////////////////////////////////////////////////////////////
CSubtitleUpr::CSubtitleUpr()
{
	m_bEnabled = false;
	m_bAutoMode = true;
	m_pHandler = nullptr;

	s_Instance = this;
}

//////////////////////////////////////////////////////////////////////////
CSubtitleUpr::~CSubtitleUpr()
{
	SetEnabled(false);
	s_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::SetEnabled(bool bEnabled)
{
	if (bEnabled != m_bEnabled)
	{
		m_bEnabled = bEnabled;

		if (m_bAutoMode)
		{
			if (bEnabled)
			{
				gEnv->pAudioSystem->AddRequestListener(&CSubtitleUpr::OnAudioTriggerStarted, nullptr, DrxAudio::ESystemEvents::TriggerExecuted);
				gEnv->pAudioSystem->AddRequestListener(&CSubtitleUpr::OnAudioTriggerFinished, nullptr, DrxAudio::ESystemEvents::TriggerFinished);
			}
			else
			{
				gEnv->pAudioSystem->RemoveRequestListener(&CSubtitleUpr::OnAudioTriggerStarted, nullptr);
				gEnv->pAudioSystem->RemoveRequestListener(&CSubtitleUpr::OnAudioTriggerFinished, nullptr);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::SetAutoMode(bool bOn)
{
	if (bOn != m_bAutoMode)
	{
		if (m_bEnabled)
		{
			SetEnabled(false);  //force refresh for add/remove as audio listener
			m_bAutoMode = bOn;
			SetEnabled(true);
		}
		else
		{
			m_bAutoMode = bOn;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::ShowSubtitle(const char* subtitleLabel, bool bShow)
{
	if (m_bEnabled && m_pHandler)
	{
		m_pHandler->ShowSubtitle(subtitleLabel, bShow);
	}
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::ShowSubtitle(const DrxAudio::SRequestInfo* const pAudioRequestInfo, bool bShow)
{
	if (m_bEnabled && m_pHandler)
	{
		m_pHandler->ShowSubtitle(pAudioRequestInfo, bShow);
	}
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::OnAudioTriggerStarted(const DrxAudio::SRequestInfo* const pAudioRequestInfo)
{
	CSubtitleUpr::s_Instance->ShowSubtitle(pAudioRequestInfo, true);
}

//////////////////////////////////////////////////////////////////////////
void CSubtitleUpr::OnAudioTriggerFinished(const DrxAudio::SRequestInfo* const pAudioRequestInfo)
{
	CSubtitleUpr::s_Instance->ShowSubtitle(pAudioRequestInfo, false);
}
