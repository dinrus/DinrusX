// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace DataSource
	{

		//===================================================================================
		//
		// IQueryBlueprintLoader
		//
		//===================================================================================

		struct IQueryBlueprintLoader
		{
			virtual                        ~IQueryBlueprintLoader() {}
			virtual bool                   LoadTextualQueryBlueprint(Core::ITextualQueryBlueprint& out, Shared::IUqsString& error) = 0;
		};

	}
}
