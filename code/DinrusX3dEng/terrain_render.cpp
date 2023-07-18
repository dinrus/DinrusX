// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   terrain_render.cpp
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Compilers:   Visual Studio.NET
//  Описание:    Отрисовка видимых секторов.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusX3dEng/StdAfx.h>
#include <DinrusX/DinrusX3dEng/terrain.h>
#include <DinrusX/DinrusX3dEng/terrain_sector.h>

int CTerrain::GetDetailTextureMaterials(IMaterial* materials[])
{
	int materialNumber = 0;

	uchar szProj[] = "XYZ";

	for (int s = 0; s < SRangeInfo::e_hole; s++)
	{
		SSurfaceType* pSurf = &m_SSurfaceType[s];

		if (pSurf->HasMaterial())
		{
			for (int p = 0; p < 3; p++)
			{
				if (IMaterial* pMat = pSurf->GetMaterialOfProjection(szProj[p]))
				{
					if (materials)
					{
						materials[materialNumber] = pMat;
					}
					materialNumber++;
				}
			}
		}
	}

	return materialNumber;
}

void CTerrain::DrawVisibleSectors(const SRenderingPassInfo& passInfo)
{
	FUNCTION_PROFILER_3DENGINE;

	if (!passInfo.RenderTerrain() || !Get3DEngine()->m_bShowTerrainSurface)
		return;

	for (STerrainVisItem node : m_lstVisSectors)
	{
		node.first->RenderNodeHeightmap(passInfo, node.second);
	}
}

void CTerrain::UpdateSectorMeshes(const SRenderingPassInfo& passInfo)
{
	FUNCTION_PROFILER_3DENGINE;

	m_pTerrainUpdateDispatcher->SyncAllJobs(false, passInfo);
}
