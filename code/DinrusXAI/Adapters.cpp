// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   Adapters.cpp
//  Created:     02/02/2009 by Matthew
//  Описание: Implements adapters for AI objects from external interfaces to internal
//               This is purely a translation layer without concrete instances
//               They can have no state and must remain abstract
// --------------------------------------------------------------------------------------
//  History:
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXAI/StdAfx.h>

#include <DinrusX/DinrusXAI/Adapters.h>

CWeakRef<CAIObject> GetWeakRefSafe(IAIObject* pObj) { return (pObj ? GetWeakRef((CAIObject*)pObj) : NILREF); }

IAIObject*          CAIGroupAdapter::GetAttentionTarget(bool bHostileOnly, bool bLiveOnly) const
{
	CWeakRef<CAIObject> refTarget = GetAttentionTarget(bHostileOnly, bLiveOnly, NILREF);
	return refTarget.GetIAIObject();
}
