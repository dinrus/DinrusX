// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXEntity/stdafx.h>
#include <DinrusX/DinrusXEntity/EntityUtilsComponent.h>

#include <DinrusX/CoreX/Renderer/IRenderAuxGeom.h>

#include <DinrusX/DinrusXSchema/EnvFunction.h>
#include <DinrusX/DinrusXSchema/SharedString.h>
#include <DinrusX/DinrusXSchema/IObject.h>
#include <DinrusX/DinrusXSchema/EnvComponent.h>
#include <DinrusX/DinrusXSchema/EnvSignal.h>

#include  <DinrusX/DinrusXSchema/IEnvRegistrar.h>

namespace Schematyc
{
void CEntityUtilsComponent::ReflectType(CTypeDesc<CEntityUtilsComponent>& desc)
{
	desc.SetGUID("e88093df-904f-4c52-af38-911e26777cdc"_drx_guid);
	desc.SetLabel("Entity");
	desc.SetDescription("Entity utilities component");
	desc.SetIcon("icons:schematyc/entity_utils_component.ico");
	desc.SetComponentFlags({ EFlags::Singleton, EFlags::HideFromInspector, EFlags::HiddenFromUser });
}

void CEntityUtilsComponent::Register(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEntityUtilsComponent));
	}
}

} // Schematyc
