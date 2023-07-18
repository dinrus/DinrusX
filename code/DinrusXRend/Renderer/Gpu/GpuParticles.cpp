// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/D3DRender/Gpu/Particles/GpuParticleUpr.h>
#include <DinrusX/DinrusXRend/GpuParticles.h>

CComputeParticlesStage::CComputeParticlesStage()
	: m_oldFrameIdExecute(-1)
	, m_oldFrameIdPreDraw(-1)
	, m_oldFrameIdPostDraw(-1)
{
}

CComputeParticlesStage::~CComputeParticlesStage()
{
	if (m_pGpuParticleUpr)
		m_pGpuParticleUpr->CleanupResources();
}

void CComputeParticlesStage::Init()
{
	if (!m_pGpuParticleUpr)
		m_pGpuParticleUpr = std::unique_ptr<gpu_pfx2::CUpr>(new gpu_pfx2::CUpr());
}

void CComputeParticlesStage::Update()
{
	CRenderView* pRenderView = RenderView();
	int CurrentFrameID = pRenderView->GetFrameId();
	if (CurrentFrameID != m_oldFrameIdExecute)
	{
		m_pGpuParticleUpr->RenderThreadUpdate(pRenderView);
		m_oldFrameIdExecute = CurrentFrameID;
	}
}

void CComputeParticlesStage::PreDraw()
{
	CRenderView* pRenderView = RenderView();
	int CurrentFrameID = pRenderView->GetFrameId();
	if (CurrentFrameID != m_oldFrameIdPreDraw)
	{
		m_pGpuParticleUpr->RenderThreadPreUpdate(pRenderView);
		m_oldFrameIdPreDraw = CurrentFrameID;
	}
}

void CComputeParticlesStage::PostDraw()
{
	CRenderView* pRenderView = RenderView();
	int CurrentFrameID = pRenderView->GetFrameId();
	if (CurrentFrameID != m_oldFrameIdPostDraw)
	{
		m_pGpuParticleUpr->RenderThreadPostUpdate(pRenderView);
		m_oldFrameIdPostDraw = CurrentFrameID;
	}
}

gpu_pfx2::IUpr* CD3D9Renderer::GetGpuParticleUpr()
{
	return GetGraphicsPipeline().GetComputeParticlesStage()->GetGpuParticleUpr();
}