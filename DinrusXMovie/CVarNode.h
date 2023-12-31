// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __CVarNode_h__
#define __CVarNode_h__
#pragma once

#include <DinrusX/DinrusXMovie/AnimNode.h>

class CAnimCVarNode : public CAnimNode
{
public:
	CAnimCVarNode(const int id);

	virtual EAnimNodeType  GetType() const override { return eAnimNodeType_CVar; }

	virtual void           SetName(const char* name) override;
	virtual void           Animate(SAnimContext& animContext) override;
	virtual void           CreateDefaultTracks() override;
	virtual void           OnReset() override;
	virtual void           OnResume() override;

	virtual unsigned int   GetParamCount() const override;
	virtual CAnimParamType GetParamType(unsigned int nIndex) const override;

protected:
	virtual bool GetParamInfoFromType(const CAnimParamType& paramId, SParamInfo& info) const override;

private:
	float m_value;
};

#endif // __CVarNode_h__
