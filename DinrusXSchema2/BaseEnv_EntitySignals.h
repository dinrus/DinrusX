// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	typedef TemplateUtils::CSignalv2<void (const SEntityEvent&), EntityId> EntityEventSignal;

	// g_eventSignal is used to forward the following entity events from the entity game object extension:
	//   ENTITY_EVENT_XFORM
	//   ENTITY_EVENT_DONE
	//   ENTITY_EVENT_HIDE
	//   ENTITY_EVENT_UNHIDE
	//   ENTITY_EVENT_SCRIPT_EVENT
	//   ENTITY_EVENT_ENTERAREA
	//   ENTITY_EVENT_LEAVEAREA
	extern EntityEventSignal g_entityEventSignal;
}
