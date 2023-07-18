// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Memory/PoolAllocator.h>

// Class responsible for managing potentially visible render nodes.
class CVisibleRenderNodesUpr
{
public:
	static int const MAX_NODES_CHECK_PER_FRAME = 500;
	static int const MAX_DELETE_BUFFERS = 3;

	struct Statistics
	{
		int numFree;
		int numUsed;
	};

public:
	CVisibleRenderNodesUpr();
	~CVisibleRenderNodesUpr();

	SRenderNodeTempData* AllocateTempData(int lastSeenFrame);

	// Set last frame for this rendering pass.
	// Return if last frame changed and rendering should be done this frame for this pass.
	static bool SetLastSeenFrame(SRenderNodeTempData* pTempData, const SRenderingPassInfo& passInfo);

	// Iteratively update array of visible nodes checking if they are expired
	void       UpdateVisibleNodes(int currentFrame, int maxNodesToCheck = MAX_NODES_CHECK_PER_FRAME);
	void       InvalidateAll();
	void       OnRenderNodeDeleted(IRenderNode* pRenderNode);
	Statistics GetStatistics() const;

	void       ClearAll();

private:
	void OnRenderNodeVisibilityChange( IRenderNode *pRenderNode,bool bVisible );

private:
	std::vector<SRenderNodeTempData*> m_visibleNodes;
	int                               m_lastStartUpdateNode;

	int                               m_firstAddedNode;

	int                               m_currentNodesToDelete;
	std::vector<SRenderNodeTempData*> m_toDeleteNodes[MAX_DELETE_BUFFERS];

	int                               m_lastUpdateFrame;

	struct PoolSyncCriticalSection
	{
		void Lock()   { m_cs.Lock(); }
		void Unlock() { m_cs.Unlock(); }
		DrxCriticalSectionNonRecursive m_cs;
	};
	stl::TPoolAllocator<SRenderNodeTempData, PoolSyncCriticalSection> m_pool;

	DrxCriticalSectionNonRecursive m_accessLock;
};
