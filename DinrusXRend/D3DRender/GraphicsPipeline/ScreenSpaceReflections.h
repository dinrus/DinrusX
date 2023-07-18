// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CScreenSpaceReflectionsStage : public CGraphicsPipelineStage
{
public:
	void Init();
	void Execute();

private:
	CFullscreenPass    m_passRaytracing;
	CFullscreenPass    m_passComposition;
	CStretchRectPass   m_passCopy;
	CStretchRectPass   m_passDownsample0;
	CStretchRectPass   m_passDownsample1;
	CStretchRectPass   m_passDownsample2;
	CGaussianBlurPass  m_passBlur0;
	CGaussianBlurPass  m_passBlur1;
	CGaussianBlurPass  m_passBlur2;

	Matrix44           m_prevViewProj[MAX_GPU_NUM];
};
