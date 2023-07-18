// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/StandardGraphicsPipeline.h>

//////////////////////////////////////////////////////////////////////////
CStandardGraphicsPipeline& CGraphicsPipelineStage::GetStdGraphicsPipeline()
{
	assert(m_pGraphicsPipeline);
	return *static_cast<CStandardGraphicsPipeline*>(m_pGraphicsPipeline);
}

//////////////////////////////////////////////////////////////////////////
const CStandardGraphicsPipeline& CGraphicsPipelineStage::GetStdGraphicsPipeline() const
{
	assert(m_pGraphicsPipeline);
	return *static_cast<const CStandardGraphicsPipeline*>(m_pGraphicsPipeline);
}

const CRenderOutput& CGraphicsPipelineStage::GetRenderOutput() const
{
	assert(RenderView()->GetRenderOutput());
	return *RenderView()->GetRenderOutput();
}

CRenderOutput& CGraphicsPipelineStage::GetRenderOutput()
{
	assert(RenderView()->GetRenderOutput());
	return *RenderView()->GetRenderOutput();
}

void CGraphicsPipelineStage::ClearDeviceOutputState()
{
	GetDeviceObjectFactory().GetCoreCommandList().GetGraphicsInterface()->ClearState(true);
}
