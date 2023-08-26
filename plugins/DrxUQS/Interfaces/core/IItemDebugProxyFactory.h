// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// IItemDebugProxyFactory
		//
		//===================================================================================

		struct IItemDebugProxyFactory
		{
			virtual                              ~IItemDebugProxyFactory() {}
			virtual IItemDebugProxy_Sphere&      CreateSphere() = 0;
			virtual IItemDebugProxy_Path&        CreatePath() = 0;
			virtual IItemDebugProxy_AABB&        CreateAABB() = 0;
		};

	}
}
