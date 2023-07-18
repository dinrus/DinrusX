// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Thread/DrxThreadSafeRendererContainer.h>
#include <DinrusX/CoreX/Thread/DrxThreadSafeWorkerContainer.h>
#include <DinrusX/CoreX/Math/Range.h>

#include <DinrusX/DinrusXRend/RenderPipeline.h>
#include <DinrusX/DinrusXRend/D3DRender/GraphicsPipeline/GraphicsPipelineStage.h>

class CRenderItemDrawer
{
public:
	//////////////////////////////////////////////////////////////////////////
	// Multi-threaded draw-command recording /////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	struct SRenderListCollection
	{
		void Init()
		{
			pEnd = pStart;
		}

		void Reserve(size_t numAdditionalItems)
		{
			size_t numItems =
			  (pEnd - pStart);
			size_t numItemsAfter = numItems +
			                       numAdditionalItems;

			if (rendList.size() < numItemsAfter)
			{
				rendList.resize(numItemsAfter);
				pStart = &rendList[0];
				pEnd = pStart + numItems;
			}
		}

		void Expand(const SGraphicsPipelinePassContext& rendList)
		{
			* pEnd++ = rendList;
		}

		void WaitForJobs()
		{
			if (pCommandLists.size())
			{
				gEnv->pJobUpr->WaitForJob(jobState);
				GetDeviceObjectFactory().ForfeitCommandLists(std::move(pCommandLists));
			}
		}

		void PrepareJobs(uint32 numTasks)
		{
			WaitForJobs();

			pCommandLists = GetDeviceObjectFactory().AcquireCommandLists(numTasks);
		}

		std::vector<SGraphicsPipelinePassContext> rendList;

		SGraphicsPipelinePassContext*             pStart;
		SGraphicsPipelinePassContext*             pEnd;

		std::vector<CDeviceCommandListUPtr>       pCommandLists;
		JobUpr::SJobState                     jobState;
	};

	SRenderListCollection m_CoalescedContexts;

	void DrawCompiledRenderItems(const SGraphicsPipelinePassContext& passContext) const;

	void InitDrawSubmission();
	void JobifyDrawSubmission(bool bForceImmediateExecution = false);
	void WaitForDrawSubmission();
};
