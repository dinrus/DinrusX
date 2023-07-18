// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
History:
- 12:05:2009   Created by Federico Rebora
*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/ColorGradientNode.h>
#include <DinrusX/DinrusXGame/Graphics/ColorGradientUpr.h>
#include <DrxRenderer/IColorGradingController.h>

enum InputPorts
{
	eIP_Trigger,
};

const SInputPortConfig CFlowNode_ColorGradient::inputPorts[] =
{
	InputPortConfig_Void("Trigger", _HELP("")),
	InputPortConfig<string>("tex_TexturePath", _HELP("Path to the Color Chart texture.")),
	InputPortConfig<float>("TransitionTime", _HELP("Fade in time (Seconds).")),
	{0},    
};

CFlowNode_ColorGradient::CFlowNode_ColorGradient(SActivationInfo* activationInformation) :
m_pTexture(NULL)
{
}

CFlowNode_ColorGradient::~CFlowNode_ColorGradient()
{
	SAFE_RELEASE(m_pTexture);
}



void CFlowNode_ColorGradient::GetConfiguration(SFlowNodeConfig& config)
{
	config.pInputPorts = inputPorts;
	config.SetCategory(EFLN_ADVANCED);
}

void CFlowNode_ColorGradient::ProcessEvent(EFlowEvent event, SActivationInfo* activationInformation)
{
	if (!gEnv->pRenderer)
	{
		return;
	}

	//Preload texture
	if(event==IFlowNode::eFE_PrecacheResources && m_pTexture==NULL)
	{
		const string texturePath = GetPortString(activationInformation, eInputPorts_TexturePath);

		const uint32 COLORCHART_TEXFLAGS = FT_NOMIPS |  FT_DONT_STREAM | FT_STATE_CLAMP;

		m_pTexture = gEnv->pRenderer->EF_LoadTexture(texturePath.c_str(), COLORCHART_TEXFLAGS);
		
		//DrxLogAlways("CFlowNode_ColorGradient 0x%p: Precache texture %s", this, texturePath.c_str());
	}

	if (event==IFlowNode::eFE_Activate && IsPortActive(activationInformation, eIP_Trigger))
	{
		const string texturePath = GetPortString(activationInformation, eInputPorts_TexturePath);
		const float timeToFade = GetPortFloat(activationInformation, eInputPorts_TransitionTime);

		g_pGame->GetColorGradientUpr().TriggerFadingColorGradient(texturePath, timeToFade);
	}
}

void CFlowNode_ColorGradient::GetMemoryUsage(IDrxSizer* sizer) const
{
	sizer->Add(*this);
}

REGISTER_FLOW_NODE("CrysisFX:ColorGradient", CFlowNode_ColorGradient);
