// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
//-------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#ifndef __MANNEQUINAGEXISTANCEQUERY_H__
#define __MANNEQUINAGEXISTANCEQUERY_H__

#pragma once
#include <DinrusX/DinrusXAct/IAnimationGraph.h>

namespace MannequinAG
{

// ============================================================================
// ============================================================================

class CMannequinAGExistanceQuery
	: public IAnimationGraphExistanceQuery
{
public:
	CMannequinAGExistanceQuery(IAnimationGraphState* pAnimationGraphState);
	virtual ~CMannequinAGExistanceQuery();

private:
	// IAnimationGraphExistanceQuery
	virtual IAnimationGraphState* GetState();
	virtual void SetInput(InputID, float);
	virtual void SetInput(InputID, int);
	virtual void SetInput(InputID, const char*);

	virtual bool       Complete();
	virtual CTimeValue GetAnimationLength() const;
	virtual void       Reset();
	virtual void       Release() { delete this; }
	// ~IAnimationGraphExistanceQuery

private:
	IAnimationGraphState* m_pAnimationGraphState;
};

// ============================================================================
// ============================================================================

} // namespace MannequinAG

#endif // __MANNEQUINAGEXISTANCEQUERY_H__
