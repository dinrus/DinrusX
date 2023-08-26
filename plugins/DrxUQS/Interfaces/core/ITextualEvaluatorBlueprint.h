// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// ITextualEvaluatorBlueprint
		//
		//===================================================================================

		struct ITextualEvaluatorBlueprint
		{
			virtual                                       ~ITextualEvaluatorBlueprint() {}

			virtual const char*                           GetEvaluatorName() const = 0;
			virtual void                                  SetEvaluatorName(const char* szEvaluatorName) = 0;

			virtual const DrxGUID&                        GetEvaluatorGUID() const = 0;
			virtual void                                  SetEvaluatorGUID(const DrxGUID& evaluatorGUID) = 0;

			virtual float                                 GetWeight() const = 0;
			virtual void                                  SetWeight(float weight) = 0;

			virtual const char*                           GetScoreTransformName() const = 0;
			virtual void                                  SetScoreTransformName(const char* szScoreTransformName) = 0;

			virtual const DrxGUID&                        GetScoreTransformGUID() const = 0;
			virtual void                                  SetScoreTransformGUID(const DrxGUID& scoreTransformGUID) = 0;

			virtual bool                                  GetNegateDiscard() const = 0;
			virtual void                                  SetNegateDiscard(bool bNegateDiscard) = 0;

			virtual const ITextualInputBlueprint&         GetInputRoot() const = 0;  // read access
			virtual ITextualInputBlueprint&               GetInputRoot() = 0;        // write access

			virtual DataSource::ISyntaxErrorCollector*    GetSyntaxErrorCollector() const = 0;
			virtual void                                  SetSyntaxErrorCollector(DataSource::SyntaxErrorCollectorUniquePtr pSyntaxErrorCollector) = 0;
		};

	}
}
