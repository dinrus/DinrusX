// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>

class CComputeParticlesStage : public CGraphicsPipelineStage
{
public:
	CComputeParticlesStage();
	~CComputeParticlesStage();

	void Init() final;

	void Update() override;
	void PreDraw();
	void PostDraw();

	gpu_pfx2::CUpr* GetGpuParticleUpr() { return m_pGpuParticleUpr.get(); }
private:
	std::unique_ptr<gpu_pfx2::CUpr> m_pGpuParticleUpr;
	int m_oldFrameIdExecute;
	int m_oldFrameIdPreDraw;
	int m_oldFrameIdPostDraw;
};
