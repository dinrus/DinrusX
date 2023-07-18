// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

class CMobileCompositionStage : public CGraphicsPipelineStage
{
public:
	void Init() final;
	void Execute();

private:
	CDepthDownsamplePass m_passDepthDownsample2;
	CDepthDownsamplePass m_passDepthDownsample4;
	CDepthDownsamplePass m_passDepthDownsample8;
	
	CFullscreenPass m_passLighting;
	CFullscreenPass m_passTonemappingTAA;
};