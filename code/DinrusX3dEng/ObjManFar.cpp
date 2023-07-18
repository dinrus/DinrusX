// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   statobjmanfar.cpp
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Compilers:   Visual Studio.NET
//  Описание:    Отрисовка дальних объектов как спрайтов.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusX3dEng/StdAfx.h>

#include <DinrusX/DinrusX3dEng/StatObj.h>
#include <DinrusX/DinrusX3dEng/ObjMan.h>
#include <DinrusX/DinrusX3dEng/3dEngine.h>

static PodArray<SVegetationSpriteInfo> arrVegetationSprites[RT_COMMAND_BUF_COUNT][MAX_RECURSION_LEVELS];

void CObjUpr::UnloadFarObjects()
{
	for (int i = 0; i < RT_COMMAND_BUF_COUNT; ++i)
		for (int j = 0; j < MAX_RECURSION_LEVELS; ++j)
			stl::free_container(arrVegetationSprites[i][j]);
}

void CObjUpr::RenderFarObjects(const SRenderingPassInfo& passInfo)
{
}
/*
   static inline int Compare(CVegetation *& p1, CVegetation *& p2)
   {
   if(p1->m_fDistance > p2->m_fDistance)
    return 1;
   else
   if(p1->m_fDistance < p2->m_fDistance)
    return -1;

   return 0;
   } */

void CObjUpr::DrawFarObjects(float fMaxViewDist, const SRenderingPassInfo& passInfo)
{
	if (!GetCVars()->e_VegetationSprites)
		return;

	FUNCTION_PROFILER_3DENGINE;

	if (passInfo.GetRecursiveLevel() >= MAX_RECURSION_LEVELS)
	{
		assert(!"Recursion depther than MAX_RECURSION_LEVELS is not supported");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//  Draw all far
	//////////////////////////////////////////////////////////////////////////////////////

	//PodArray<SVegetationSpriteInfo> * pList = &arrVegetationSprites[m_nRenderStackLevel];
	//if (pList->Count())
	//  GetRenderer()->DrawObjSprites(pList);
}

void CObjUpr::GenerateFarObjects(float fMaxViewDist, const SRenderingPassInfo& passInfo)
{
	if (!GetCVars()->e_VegetationSprites)
		return;

	FUNCTION_PROFILER_3DENGINE;

	if (passInfo.GetRecursiveLevel() >= MAX_RECURSION_LEVELS)
	{
		assert(!"Recursion depther than MAX_RECURSION_LEVELS is not supported");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//  Draw all far
	//////////////////////////////////////////////////////////////////////////////////////

	/*arrVegetationSprites[m_nRenderStackLevel].Clear();

	   for(int t=0; t<nThreadsNum; t++)
	   {
	   PodArray<SVegetationSpriteInfo> * pList = &m_arrVegetationSprites[m_nRenderStackLevel][t];
	   if (pList->Count())
	    arrVegetationSprites[m_nRenderStackLevel].AddList(*pList);
	   }*/

	//PodArray<SVegetationSpriteInfo> * pList = &arrVegetationSprites[m_nRenderStackLevel];
	//if (pList->Count())
	//  GetRenderer()->GenerateObjSprites(pList);
}
