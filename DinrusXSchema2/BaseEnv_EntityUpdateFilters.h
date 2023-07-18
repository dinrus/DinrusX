// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

#include <DinrusX/DinrusX3dEng/I3DEngine.h> // Must be included before CREOcclusionQuery.h.
#include <DinrusX/CoreX/Renderer/RenderElements/CREOcclusionQuery.h> // Must be included before IEntityRenderState.h.

namespace SchematycBaseEnv
{
	struct SEntityNotHidden
	{
		inline bool operator () (IEntity* pEntity) const
		{
			return pEntity && !pEntity->IsHidden();
		}
	};

	struct SEntityVisible
	{
		inline bool operator () (IEntity* pEntity) const
		{
			if(pEntity)
			{
				return pEntity->IsRendered();
			}
			return false;
		}
	};

	typedef Schematyc2::CConfigurableUpdateFilter<IEntity*, NTypelist::CConstruct<SEntityNotHidden>::TType>                 EntityNotHiddenUpdateFilter;
	typedef Schematyc2::CConfigurableUpdateFilter<IEntity*, NTypelist::CConstruct<SEntityNotHidden, SEntityVisible>::TType> EntityVisibleUpdateFilter;
}