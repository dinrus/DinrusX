// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CDepthOfFieldStage : public CGraphicsPipelineStage
{
public:
	void Execute();

private:
	Vec4 ToUnitDisk(Vec4& origin, float blades, float fstop);

private:
	CStretchRectPass m_passCopySceneTarget;
	CFullscreenPass  m_passLayerDownscale;
	CFullscreenPass  m_passTileMinCoC[MIN_DOF_COC_K];
	CFullscreenPass  m_passGather0;
	CFullscreenPass  m_passGather1;
	CFullscreenPass  m_passComposition;
};
