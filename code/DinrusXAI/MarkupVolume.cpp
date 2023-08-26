// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/MarkupVolume.h>

namespace MNM
{

void SMarkupVolume::Swap(SMarkupVolume& other)
{
	BoundingVolume::Swap(other);

	std::swap(areaAnnotation, other.areaAnnotation);
	std::swap(bStoreTriangles, other.bStoreTriangles);
	std::swap(bExpandByAgentRadius, other.bExpandByAgentRadius);
}

}
