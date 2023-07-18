// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/StandardGraphicsPipeline.h>

namespace ShadowMaskInternal
{
	class CSunShadows;
	class CLocalLightShadows;
}

class CShadowMaskStage : public CGraphicsPipelineStage
{
	friend class ShadowMaskInternal::CSunShadows;
	friend class ShadowMaskInternal::CLocalLightShadows;

public:
	CShadowMaskStage();

	void Init() final;
	void Prepare();

	void Execute();

	void OnCVarsChanged(const CCVarUpdateRecorder& cvarUpdater) final;

private:
	std::unique_ptr<ShadowMaskInternal::CSunShadows>        m_pSunShadows;
	std::unique_ptr<ShadowMaskInternal::CLocalLightShadows> m_pLocalLightShadows;

	std::vector<CPrimitiveRenderPass>                       m_maskGenPasses;
	CPrimitiveRenderPass                                    m_debugCascadesPass;

	CTexture*                                               m_pShadowMaskRT;
	CConstantBufferPtr                                      m_pPerViewConstantBuffer;

	SRenderViewInfo                                         m_viewInfo[2];
	size_t                                                  m_viewInfoCount;

	int                                                     m_sunShadowPrimitives;
	int                                                     m_localLightPrimitives;
};
