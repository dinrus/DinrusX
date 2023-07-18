// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CMotionBlurStage : public CGraphicsPipelineStage
{
public:
	void Execute();

private:
	float ComputeMotionScale();

private:
	CFullscreenPass    m_passPacking;
	CFullscreenPass    m_passTileGen1;
	CFullscreenPass    m_passTileGen2;
	CFullscreenPass    m_passNeighborMax;
	CStretchRectPass   m_passCopy;
	CFullscreenPass    m_passMotionBlur;
};
