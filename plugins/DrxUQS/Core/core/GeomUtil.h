// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CGeomUtil
		//
		//===================================================================================

		class CGeomUtil
		{
		public:
			static float     Distance_Lineseg_AABB(const Lineseg& lineseg, const AABB& aabb);

		private:
			static void      TrianglesFromAABB(Triangle (&outTriangles)[12], const AABB& aabb);
		};

	}
}
