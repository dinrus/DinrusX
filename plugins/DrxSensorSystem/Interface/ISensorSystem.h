// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace Drx
{
	namespace SensorSystem
	{
		struct ISensorMap;
		struct ISensorTagLibrary;

		struct ISensorSystem
		{
			virtual ~ISensorSystem() {}

			virtual ISensorTagLibrary& GetTagLibrary() = 0;
			virtual ISensorMap&        GetMap() = 0;
		};
	}
}