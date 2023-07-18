// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Math/Drx_Geo.h>
#include <DinrusX/CoreX/Math/Drx_Vector3.h>

namespace MNM
{

//! Represents the shape of the navigation mesh bounding volume or an exclusion volume
struct SBoundingVolume
{
	AABB        aabb = AABB::RESET;
	const Vec3* pVertices = nullptr;
	size_t      verticesCount = 0;
	float       height = 0.0f;
};

//! Set of parameters used for creating Navigation markup shape volume 
struct SMarkupVolumeParams
{
	AreaAnnotation areaAnnotation;
	float height;
	bool bStoreTriangles;
	bool bExpandByAgentRadius;
};

} // namespace MNM
