// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/PrimitiveRenderPass.h>


class CDeferredDecalsStage : public CGraphicsPipelineStage
{
public:
	enum { MaxPersistentDecals = 512 };
	
public:
	CDeferredDecalsStage();
	virtual ~CDeferredDecalsStage();

	void Init() final;
	void Execute();

private:
	void ResizeDecalBuffers(size_t requiredDecalCount);
	void SetupDecalPrimitive(const SDeferredDecal& decal, CRenderPrimitive& primitive, _smart_ptr<IRenderShaderResources>& pShaderResources);

private:
	std::vector<_smart_ptr<IRenderShaderResources> > m_decalShaderResources;
	std::vector<CRenderPrimitive>                    m_decalPrimitives;
	CPrimitiveRenderPass                             m_decalPass;
};
