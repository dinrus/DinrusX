// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

namespace Schematyc2
{
	struct IScriptGraphNode;

	DECLARE_SHARED_POINTERS(IScriptGraphNode);

	struct IScriptGraphNodeCreationMenuCommand
	{
		virtual ~IScriptGraphNodeCreationMenuCommand() {}

		virtual IScriptGraphNodePtr Execute(const Vec2& pos) = 0; // #SchematycTODO : Do we really need to pass position here?
	};

	DECLARE_SHARED_POINTERS(IScriptGraphNodeCreationMenuCommand)

	struct IScriptGraphNodeCreationMenu
	{
		virtual ~IScriptGraphNodeCreationMenu() {}

		virtual bool AddOption(const char* szLabel, const char* szDescription, const char* szWikiLink, const IScriptGraphNodeCreationMenuCommandPtr& pCommand) = 0;
	};
}
