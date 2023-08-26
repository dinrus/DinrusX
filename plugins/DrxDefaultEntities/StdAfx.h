// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#define eDrxModule eDrxM_EnginePlugin

<DinrusX/CoreX/Platform/platform.h>

<DinrusX/DinrusXEntity/IEntitySystem.h>

<DinrusX/DinrusXSchema/CoreAPI.h>

#include "PluginDll.h"

#if !defined(_RELEASE)
#define INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
#endif // _RELEASE

template<class T>
static IEntityClass* RegisterEntityWithDefaultComponent(const char* name, const char* editorCategory = "", const char* editorIcon = "", bool bIconOnTop = false)
{
	IEntityClassRegistry::SEntityClassDesc clsDesc;
	clsDesc.sName = name;

	clsDesc.editorClassInfo.sCategory = editorCategory;
	clsDesc.editorClassInfo.sIcon = editorIcon;
	clsDesc.editorClassInfo.bIconOnTop = bIconOnTop;

	struct CObjectCreator
	{
		static IEntityComponent* Create(IEntity* pEntity, SEntitySpawnParams& params, void* pUserData)
		{
			return pEntity->GetOrCreateComponentClass<T>();
		}
	};

	clsDesc.pUserProxyCreateFunc = &CObjectCreator::Create;
	clsDesc.flags |= ECLF_INVISIBLE;

	return gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(clsDesc);
}
