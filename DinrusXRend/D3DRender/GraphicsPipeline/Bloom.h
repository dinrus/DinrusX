// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/FullscreenPass.h>

class CBloomStage : public CGraphicsPipelineStage
{
public:
	void Execute();

private:
	CFullscreenPass m_pass1H;
	CFullscreenPass m_pass1V;
	CFullscreenPass m_pass2H;
	CFullscreenPass m_pass2V;
};
