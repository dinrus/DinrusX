// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusX3dEng/StdAfx.h>
#include <DinrusX/DinrusX3dEng/PolygonClipContext.h>
#include <DinrusX/DinrusX3dEng/RoadRenderNode.h>
#include <DinrusX/DinrusX3dEng/Brush.h>
#include <DinrusX/DinrusX3dEng/RenderMeshMerger.h>
#include <DinrusX/DinrusX3dEng/DecalUpr.h>
#include <DinrusX/DinrusX3dEng/VisAreas.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusX3dEng/LightEntity.h>

#ifndef PI
	#define PI 3.14159f
#endif

//////////////////////////////////////////////////////////////////////////
void CObjUpr::PrepareCullbufferAsync(const CCamera& rCamera)
{
	if (gEnv->pRenderer)
	{
		m_CullThread.PrepareCullbufferAsync(rCamera);
	}
}

//////////////////////////////////////////////////////////////////////////
void CObjUpr::BeginOcclusionCulling(const SRenderingPassInfo& passInfo)
{
	if (gEnv->pRenderer)
	{
		m_CullThread.CullStart(passInfo);
	}
}

//////////////////////////////////////////////////////////////////////////
void CObjUpr::EndOcclusionCulling()
{
	if (gEnv->pRenderer)
	{
		m_CullThread.CullEnd();
	}
}

//////////////////////////////////////////////////////////////////////////
void CObjUpr::RenderNonJobObjects(const SRenderingPassInfo& passInfo)
{
	DRX_PROFILE_REGION(PROFILE_3DENGINE, "3DEngine: RenderNonJobObjects");
	DRXPROFILE_SCOPE_PROFILE_MARKER("RenderNonJobObjects");

	SCheckOcclusionOutput outputData;
	while (1)
	{
		// process till we know that no more procers are working
		if (!GetObjUpr()->PopFromCullOutputQueue(&outputData))
			break;

		switch (outputData.type)
		{
		case SCheckOcclusionOutput::ROAD_DECALS:
			GetObjUpr()->RenderDecalAndRoad(outputData.common.pObj,
			                                    outputData.common.pAffectingLights,
			                                    outputData.vAmbColor,
			                                    outputData.objBox,
			                                    outputData.common.fEntDistance,
			                                    outputData.common.bCheckPerObjectOcclusion,
			                                    passInfo);
			break;

		case SCheckOcclusionOutput::COMMON:
			{
				switch (outputData.common.pObj->GetRenderNodeType())
				{
				case eERType_Brush:
				case eERType_MovableBrush:
					GetObjUpr()->RenderBrush((CBrush*)outputData.common.pObj,
					                             outputData.common.pAffectingLights,
					                             outputData.common.pTerrainTexInfo,
					                             outputData.objBox,
					                             outputData.common.fEntDistance,
					                             outputData.common.bCheckPerObjectOcclusion,
					                             passInfo,
					                             outputData.passCullMask);
					break;

				case eERType_Vegetation:
					GetObjUpr()->RenderVegetation((CVegetation*)outputData.common.pObj,
					                                  outputData.common.pAffectingLights,
					                                  outputData.objBox,
					                                  outputData.common.fEntDistance,
					                                  outputData.common.pTerrainTexInfo,
					                                  outputData.common.bCheckPerObjectOcclusion,
					                                  passInfo,
					                                  outputData.passCullMask);
					break;

				default:
					GetObjUpr()->RenderObject(outputData.common.pObj,
					                              outputData.common.pAffectingLights,
					                              outputData.vAmbColor,
					                              outputData.objBox,
					                              outputData.common.fEntDistance,
					                              outputData.common.pObj->GetRenderNodeType(),
					                              passInfo,
					                              outputData.passCullMask);
					break;
				}
			}
			break;
		case SCheckOcclusionOutput::TERRAIN:
			outputData.terrain.pTerrainNode->RenderNodeHeightmap(passInfo, outputData.passCullMask);
			break;

		case SCheckOcclusionOutput::DEFORMABLE_BRUSH:
			outputData.deformable_brush.pBrush->m_pDeform->RenderInternalDeform(outputData.deformable_brush.pRenderObject,
			                                                                    outputData.deformable_brush.nLod,
			                                                                    outputData.deformable_brush.pBrush->CBrush::GetBBox(),
			                                                                    passInfo);
			break;

		default:
			DrxFatalError("Got Unknown Output type from CheckOcclusion");
			break;
		}
	}
#if !defined(_RELEASE)
	if (GetCVars()->e_CoverageBufferDebug)
	{
		GetObjUpr()->CoverageBufferDebugDraw();
	}
#endif
}

bool IsValidOccluder(IMaterial* pMat)
{
	CMatInfo* __restrict pMaterial = static_cast<CMatInfo*>(pMat);
	if (!pMaterial)
		return false;

	for (size_t a = 0, S = pMaterial->GetSubMtlCount(); a <= S; a++)
	{
		IMaterial* pSubMat = a < S ? pMaterial->GetSubMtl(a) : pMaterial;
		if (!pSubMat)
			continue;
		SShaderItem& rShaderItem = pSubMat->GetShaderItem();
		IShader* pShader = rShaderItem.m_pShader;
		if (!pShader)
			continue;
		if (pShader->GetFlags2() & (EF2_HASALPHATEST | EF2_HASALPHABLEND))
			return false;
		if (pShader->GetFlags() & (EF_NODRAW | EF_DECAL))
			return false;
		if (pShader->GetShaderType() == eST_Vegetation)
			return false;
		else if (!rShaderItem.m_pShaderResources)
			continue;
		if (rShaderItem.m_pShaderResources->IsAlphaTested())
			return false;
		if (rShaderItem.m_pShaderResources->IsTransparent())
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CObjUpr::BeginCulling()
{
	m_CheckOcclusionOutputQueue.SetRunningState();
}
