// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CInstantEvaluatorBlueprint
		//
		//===================================================================================

		class CInstantEvaluatorBlueprint final : public CEvaluatorBlueprintBase
		{
		public:

			explicit                                        CInstantEvaluatorBlueprint();
			Client::IInstantEvaluatorFactory&               GetFactory() const;
			void                                            PrintToConsole(CLogger& logger, const char* szMessagePrefix) const;

		private:

			// CEvaluatorBlueprintBase
			virtual bool                                    ResolveFactory(const ITextualEvaluatorBlueprint& source) override;
			virtual const Client::IInputParameterRegistry&  GetInputParameterRegistry() const override;
			// ~CEvaluatorBlueprintBase

		private:

			Client::IInstantEvaluatorFactory*               m_pInstantEvaluatorFactory;
		};

	}
}
