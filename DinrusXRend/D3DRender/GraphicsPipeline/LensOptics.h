// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/PrimitiveRenderPass.h>
#include <DinrusX/DinrusXRend/RendElements/FlareSoftOcclusionQuery.h>

class CRenderView;

class CLensOpticsStage : public CGraphicsPipelineStage
{
public:
	void      Init();
	void      Execute();

	bool      HasContent() const { return m_primitivesRendered>0; }

private:
	void      UpdateOcclusionQueries(SRenderViewInfo* pViewInfo, int viewInfoCount);

	CPrimitiveRenderPass  m_passLensOptics;
	CSoftOcclusionUpr m_softOcclusionUpr;

	int             m_occlusionUpdateFrame = -1;
	int             m_primitivesRendered   =  0;
};
