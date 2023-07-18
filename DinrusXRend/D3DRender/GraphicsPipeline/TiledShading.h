// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/ComputeRenderPass.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

class CTiledShadingStage : public CGraphicsPipelineStage
{
public:
	CTiledShadingStage();
	~CTiledShadingStage();

	void Init();
	void Execute();

private:
	enum EVolumeTypes
	{
		eVolumeType_Box,
		eVolumeType_Sphere,
		eVolumeType_Cone,

		eVolumeType_Count
	};
	
	struct SVolumeGeometry
	{
		CGpuBuffer       vertexDataBuf;
		buffer_handle_t  vertexBuffer;
		buffer_handle_t  indexBuffer;
		uint32           numVertices;
		uint32           numIndices;
	};

private:
	CGpuBuffer            m_lightVolumeInfoBuf;

	_smart_ptr<CTexture>  m_pTexGiDiff;
	_smart_ptr<CTexture>  m_pTexGiSpec;

	SVolumeGeometry       m_volumeMeshes[eVolumeType_Count];

	CConstantBufferPtr    m_pPerViewConstantBuffer = nullptr;
	CComputeRenderPass    m_passCullingShading;
	
	CFullscreenPass       m_passCopyDepth;
	CPrimitiveRenderPass  m_passLightVolumes;
	CRenderPrimitive      m_volumePasses[eVolumeType_Count * 2];  // Inside and outside of volume for each type
	uint32                m_numVolumesPerPass[eVolumeType_Count * 2];
};
