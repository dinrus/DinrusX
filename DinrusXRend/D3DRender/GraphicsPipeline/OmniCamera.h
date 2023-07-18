// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

class COmniCameraStage : public CGraphicsPipelineStage
{
public:
	COmniCameraStage() = default;

	void Execute();
	bool IsEnabled() const;

protected:
	CTexture* m_pOmniCameraTexture = nullptr;
	CTexture* m_pOmniCameraCubeFaceStagingTexture = nullptr;

	CFullscreenPass m_cubemapToScreenPass;
	CDownsamplePass m_downsamplePass;
};
