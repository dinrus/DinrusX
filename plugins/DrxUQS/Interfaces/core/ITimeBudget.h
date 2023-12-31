// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// ITimeBudget - allows generators and deferred-evaluators to check for whether some of the granted time is still available (or has exhausted) when doing time-sliced work
		//
		//===================================================================================

		struct ITimeBudget
		{
			virtual         ~ITimeBudget() {}
			virtual bool    IsExhausted() const = 0;
		};

	}
}
