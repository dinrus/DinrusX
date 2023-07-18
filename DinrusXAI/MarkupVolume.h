// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAI/BoundingVolume.h>

#include <DinrusX/DinrusXAI/MNMBaseTypes.h>

namespace MNM
{

struct SMarkupVolumeData
{
	struct MeshTriangles
	{
		MeshTriangles(NavigationMeshID meshId) : meshId(meshId) {}
		NavigationMeshID meshId;
		std::vector<TriangleID> triangleIds;
	};
	std::vector<MeshTriangles> meshTriangles;
};

struct SMarkupVolume : public MNM::BoundingVolume
{
	void Swap(SMarkupVolume& other);
	
	bool operator== (const SMarkupVolume& other) const
	{
		return IsEquivalent(aabb, other.aabb)
			&& height == other.height
			&& verticesCount == other.verticesCount
			&& areaAnnotation == other.areaAnnotation
			&& bStoreTriangles == other.bStoreTriangles
			&& bExpandByAgentRadius == other.bExpandByAgentRadius
			&& GetBoundaryVertices() == other.GetBoundaryVertices();
	}

	// Properties
	AreaAnnotation areaAnnotation;
	bool bStoreTriangles;
	bool bExpandByAgentRadius;
};

}
