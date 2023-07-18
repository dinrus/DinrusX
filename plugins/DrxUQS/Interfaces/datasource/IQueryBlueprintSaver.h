// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace DataSource
	{

		//===================================================================================
		//
		// IQueryBlueprintSaver
		//
		//===================================================================================

		struct IQueryBlueprintSaver
		{
			virtual                        ~IQueryBlueprintSaver() {}
			virtual bool                   SaveTextualQueryBlueprint(const Core::ITextualQueryBlueprint& queryBlueprintToSave, Shared::IUqsString& error) = 0;
		};

	}
}
