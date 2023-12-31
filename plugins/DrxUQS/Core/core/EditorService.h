// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CEditorService
		//
		//===================================================================================

		class CEditorService : public IEditorService
		{
		public:
			virtual bool      LoadTextualQueryBlueprint(const char* szQueryBlueprintName, Editor::IEditorServiceConsumer& callback, Shared::IUqsString& error) const override;
			virtual bool      SaveTextualQueryBlueprint(const char* szQueryBlueprintName, Editor::IEditorServiceConsumer& callback, Shared::IUqsString& error) const override;
			virtual bool      CreateNewTextualQueryBlueprint(const char* szQueryBlueprintName, Editor::IEditorServiceConsumer& callback, Shared::IUqsString& error) const override;
			virtual bool      RemoveQueryBlueprint(const char* szQueryBlueprintName, Shared::IUqsString& error) const override;
			virtual bool      ValidateTextualQueryBlueprint(Editor::IEditorServiceConsumer& callback) const override;
		};

	}
}
