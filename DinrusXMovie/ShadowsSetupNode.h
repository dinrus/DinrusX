// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SHADOWSSETUPNODE_H__
#define __SHADOWSSETUPNODE_H__

#pragma once

#include <DinrusX/DinrusXMovie/AnimNode.h>

class CShadowsSetupNode : public CAnimNode
{
public:
	CShadowsSetupNode(const int id);
	static void            Initialize();

	virtual EAnimNodeType  GetType() const override { return eAnimNodeType_ShadowSetup; }

	virtual void           Animate(SAnimContext& animContext) override;
	virtual void           CreateDefaultTracks() override;

	virtual void           OnReset() override;

	virtual unsigned int   GetParamCount() const override;
	virtual CAnimParamType GetParamType(unsigned int nIndex) const override;

protected:
	virtual bool GetParamInfoFromType(const CAnimParamType& paramId, SParamInfo& info) const override;
};

#endif//__SHADOWSSETUPNODE_H__
