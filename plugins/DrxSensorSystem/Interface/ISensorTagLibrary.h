// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Serialization/Forward.h>

#include "ISensorSystem.h"

namespace Drx
{
	namespace SensorSystem
	{
		enum class ESensorTags : uint32 {};

		typedef CEnumFlags<ESensorTags> SensorTags;

		struct ISensorTagLibrary
		{
			virtual ~ISensorTagLibrary() {}

			virtual SensorTags CreateTag(const char* szTagName) = 0;
			virtual SensorTags GetTag(const char* szTagName) const = 0;
			virtual void GetTagNames(DynArray<const char*>& tagNames, SensorTags tags) const = 0;
		};

	}
}