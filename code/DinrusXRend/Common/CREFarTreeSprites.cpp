// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/CoreX/Renderer/RendElements/RendElement.h>
#include <DinrusX/CoreX/Renderer/RendElements/CREFarTreeSprites.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>

void CREFarTreeSprites::mfPrepare(bool bCheckOverflow)
{
	if (bCheckOverflow)
		gRenDev->FX_CheckOverflow(0, 0, this);

	gRenDev->m_RP.m_pRE = this;
	gRenDev->m_RP.m_RendNumIndices = 0;
	gRenDev->m_RP.m_RendNumVerts = 0;
}

bool CREFarTreeSprites::mfDraw(CShader* ef, SShaderPass* sfm)
{
	gRenDev->DrawObjSprites(m_arrVegetationSprites[gRenDev->m_RP.m_nProcessThreadID][0]);
	return true;
}
