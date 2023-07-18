// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __IslandConnectionsUpr_h__
#define __IslandConnectionsUpr_h__

#pragma once

#include <DinrusX/DinrusXAI/INavigationSystem.h>
#include <DinrusX/DinrusXAI/IslandConnections.h>

class IslandConnectionsUpr
{
public:
	IslandConnectionsUpr();
	void                    Reset();

	MNM::IslandConnections& GetIslandConnections();

	void                    SetOneWayConnectionBetweenIsland(const MNM::GlobalIslandID fromIsland, const MNM::IslandConnections::Link& link);

	bool                    AreIslandsConnected(const IEntity* pEntityToTestOffGridLinks, const MNM::GlobalIslandID startIsland, const MNM::GlobalIslandID endIsland, const INavMeshQueryFilter* pFilter) const;

#ifdef DRXAISYS_DEBUG
	void DebugDraw() const;
#endif

private:
	MNM::IslandConnections m_globalIslandConnections;
};

#endif // __IslandConnectionsUpr_h__
