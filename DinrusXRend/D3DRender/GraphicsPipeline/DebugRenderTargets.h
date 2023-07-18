// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/GraphicsPipelineStage.h>
#include <DinrusX/DinrusXRend/PrimitiveRenderPass.h>

class CDebugRenderTargetsStage : public CGraphicsPipelineStage
{
public:
	static const char* showRenderTargetHelp;

	void Execute();

	void OnShowRenderTargetsCmd(IConsoleCmdArgs* pArgs);

private:

	struct SRenderTargetInfo
	{
		CTexture* pTexture      = nullptr;
		Vec4      channelWeight = Vec4(1.0f);
		bool      bFiltered     = false;
		bool      bRGBKEncoded  = false;
		bool      bAliased      = false;
		int       slice         = -1;
		int       mip           = -1;
	};

	void ResetRenderTargetList();
	void ExecuteShowTargets();

private:
	CPrimitiveRenderPass          m_debugPass;
	std::vector<CRenderPrimitive> m_debugPrimitives;
	int                           m_primitiveCount = 0;

	std::vector<SRenderTargetInfo> m_renderTargetList;
	bool                           m_bShowList;
	int                            m_columnCount;
};