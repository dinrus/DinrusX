// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CScreenSpaceObscuranceStage : public CGraphicsPipelineStage
{
public:
	void Init();
	void Execute();

private:
	CStretchRectPass  m_passCopyFromESRAM;
	CFullscreenPass   m_passObscurance;
	CFullscreenPass   m_passFilter;
	CStretchRectPass  m_passAlbedoDownsample0;
	CStretchRectPass  m_passAlbedoDownsample1;
	CStretchRectPass  m_passAlbedoDownsample2;
	CGaussianBlurPass m_passAlbedoBlur;
};
