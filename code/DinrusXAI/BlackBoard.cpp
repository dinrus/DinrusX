// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$

   -------------------------------------------------------------------------
   History:
   - 17:12:2007   11:29 : Created by Mieszko Zielinski

*************************************************************************/
#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/BlackBoard.h>

CBlackBoard::CBlackBoard()
{
	m_BB.Create(gEnv->pSystem->GetIScriptSystem());
}

void CBlackBoard::SetFromScript(SmartScriptTable& sourceBB)
{
	m_BB->Clone(sourceBB, true);
}
