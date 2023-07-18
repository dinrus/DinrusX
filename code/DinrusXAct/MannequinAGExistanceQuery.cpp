// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/MannequinAGExistanceQuery.h>

// ============================================================================
// ============================================================================

namespace MannequinAG
{

// ============================================================================
// ============================================================================

//////////////////////////////////////////////////////////////////////////
CMannequinAGExistanceQuery::CMannequinAGExistanceQuery(IAnimationGraphState* pAnimationGraphState)
	: m_pAnimationGraphState(pAnimationGraphState)
{
	DRX_ASSERT(m_pAnimationGraphState != NULL);
}

//////////////////////////////////////////////////////////////////////////
CMannequinAGExistanceQuery::~CMannequinAGExistanceQuery()
{
}

//////////////////////////////////////////////////////////////////////////
IAnimationGraphState* CMannequinAGExistanceQuery::GetState()
{
	return m_pAnimationGraphState;
}

//////////////////////////////////////////////////////////////////////////
void CMannequinAGExistanceQuery::SetInput(InputID, float)
{
}

//////////////////////////////////////////////////////////////////////////
void CMannequinAGExistanceQuery::SetInput(InputID, int)
{
}

//////////////////////////////////////////////////////////////////////////
void CMannequinAGExistanceQuery::SetInput(InputID, const char*)
{
}

//////////////////////////////////////////////////////////////////////////
bool CMannequinAGExistanceQuery::Complete()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
CTimeValue CMannequinAGExistanceQuery::GetAnimationLength() const
{
	return CTimeValue();
}

//////////////////////////////////////////////////////////////////////////
void CMannequinAGExistanceQuery::Reset()
{
}

// ============================================================================
// ============================================================================

} // MannequinAG
