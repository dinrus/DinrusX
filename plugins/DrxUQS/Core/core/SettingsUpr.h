// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CSettingsUpr
		//
		//===================================================================================

		class CSettingsUpr final : public ISettingsUpr
		{
		public:

			// ISettingsUpr
			virtual float   GetTimeBudgetInSeconds() const override;
			virtual void    SetTimeBudgetInSeconds(float timeBudgetInSeconds) override;
			// ~ISettingsUpr
		};

	}
}
