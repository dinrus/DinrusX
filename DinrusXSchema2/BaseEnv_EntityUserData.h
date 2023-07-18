// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	struct SEntityUserData
	{
		inline SEntityUserData(bool _bIsPreview, Schematyc2::EObjectFlags _objectFlags)
			: bIsPreview(_bIsPreview)
			, objectFlags(_objectFlags)
		{}

		bool                    bIsPreview;
		Schematyc2::EObjectFlags objectFlags;
	};
}