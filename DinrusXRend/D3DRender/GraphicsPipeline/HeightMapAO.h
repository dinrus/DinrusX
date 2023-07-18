// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CHeightMapAOStage : public CGraphicsPipelineStage
{
public:
	void Update() final;
	void Execute();

	bool IsValid() const { return m_bHeightMapAOExecuted; }

	const ShadowMapFrustum* GetHeightMapFrustum   () const { DRX_ASSERT(m_bHeightMapAOExecuted); return m_pHeightMapFrustum; }
	CTexture*         GetHeightMapAOScreenDepthTex() const { DRX_ASSERT(m_bHeightMapAOExecuted); return m_pHeightMapAOScreenDepthTex; }
	CTexture*         GetHeightMapAOTex           () const { DRX_ASSERT(m_bHeightMapAOExecuted); return m_pHeightMapAOTex; }

private:
	CFullscreenPass m_passSampling;
	CFullscreenPass m_passSmoothing;
	CMipmapGenPass  m_passMipmapGen;

	bool                    m_bHeightMapAOExecuted = false;
	const ShadowMapFrustum* m_pHeightMapFrustum;
	CTexture*               m_pHeightMapAOScreenDepthTex;
	CTexture*               m_pHeightMapAOTex;
};
