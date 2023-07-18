// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

class CAutoExposureStage : public CGraphicsPipelineStage
{
public:
	void Execute();

private:
	void MeasureLuminance();
	void AdjustExposure();

private:
	CFullscreenPass m_passLuminanceInitial;
	CFullscreenPass m_passLuminanceIteration[NUM_HDR_TONEMAP_TEXTURES];
	CFullscreenPass m_passAutoExposure;
};
