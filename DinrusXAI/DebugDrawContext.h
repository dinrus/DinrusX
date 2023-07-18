// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/********************************************************************
   -------------------------------------------------------------------------
   File name:   DebugDrawContext.h
   $Id$
   Описание: Render context (set of flags)

   -------------------------------------------------------------------------
   History:
   - 2009-2-17   : Created by Evgeny Adamenkov

 *********************************************************************/

#ifndef _DEBUG_DRAW_CONTEXT_H_
#define _DEBUG_DRAW_CONTEXT_H_

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXAI/IAIDebugRenderer.h>
#include <DinrusX/DinrusXAI/Environment.h>

class CDebugDrawContext
{
	IAIDebugRenderer*  m_pDebugRenderer;
	const unsigned int m_uiDepth;

public:
	CDebugDrawContext()
		: m_pDebugRenderer(gAIEnv.CVars.NetworkDebug ? gAIEnv.GetNetworkDebugRenderer() : gAIEnv.GetDebugRenderer()),
		m_uiDepth(m_pDebugRenderer->PushState())  {}

	~CDebugDrawContext()  { unsigned int uiDepth = m_pDebugRenderer->PopState(); assert(uiDepth + 1 == m_uiDepth); }

	IAIDebugRenderer* operator->() const { return m_pDebugRenderer; }
};

#endif  // #ifndef _DEBUG_DRAW_CONTEXT_H_
