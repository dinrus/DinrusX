// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	Base class for effects managed by the effect system

   -------------------------------------------------------------------------
   History:
   - 17:01:2006:		Created by Marco Koegler

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/Effect.h>

bool CEffect::Activating(float delta)
{
	return true;
}
bool CEffect::Update(float delta)
{
	return true;
}
bool CEffect::Deactivating(float delta)
{
	return true;
}

bool CEffect::OnActivate()
{
	return true;
}

bool CEffect::OnDeactivate()
{
	return true;
}

void CEffect::SetState(EEffectState state)
{
	m_state = state;
}

EEffectState CEffect::GetState()
{
	return m_state;
}
