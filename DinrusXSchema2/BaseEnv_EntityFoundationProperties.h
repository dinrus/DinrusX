// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	struct SEntityFoundationProperties
	{
		SEntityFoundationProperties();

		void Serialize(Serialization::IArchive& archive);

		string icon;
		bool   bHideInEditor;
		bool   bTriggerAreas;
	};
}