// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>
#include <DinrusX/DinrusXRend/UtilityPasses.h>

class CSunShaftsStage : public CGraphicsPipelineStage
{
public:
	void      Init();
	void      Execute();

	bool      IsActive();
	CTexture* GetFinalOutputRT();
	void      GetCompositionParams(Vec4& params0, Vec4& params1);

private:
	CTexture* GetTempOutputRT();
	int       GetDownscaledTargetsIndex();

private:
	CFullscreenPass m_passShaftsMask;
	CFullscreenPass m_passShaftsGen0;
	CFullscreenPass m_passShaftsGen1;
};
