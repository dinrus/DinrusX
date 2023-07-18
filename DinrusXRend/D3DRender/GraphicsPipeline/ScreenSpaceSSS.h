// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

// Screen Space Subsurface Scattering
class CScreenSpaceSSSStage : public CGraphicsPipelineStage
{
public:
	void Execute(CTexture* pIrradianceTex);

private:
	CFullscreenPass m_passH;
	CFullscreenPass m_passV;
};
