// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/DrxCrc32.h>


namespace SkeletonHelpers
{

	/**
	 * Computes an animation controller ID from its corresponding skeleton bone name.
	 * @param szBoneName A C-string containing the name of the bone. Shall not be null.
	 * @return An animation controller ID computed from the provided bone name.
	 */
	inline uint32 ComputeControllerId(const char szControllerName[])
	{
		return CCrc32::Compute(szControllerName);
	}

} // namespace SkeletonHelpers
