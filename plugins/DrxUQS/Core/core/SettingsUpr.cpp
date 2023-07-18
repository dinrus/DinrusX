// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

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

		float CSettingsUpr::GetTimeBudgetInSeconds() const
		{
			return SCvars::timeBudgetInSeconds;
		}

		void CSettingsUpr::SetTimeBudgetInSeconds(float timeBudgetInSeconds)
		{
			SCvars::timeBudgetInSeconds = timeBudgetInSeconds;
		}

	}
}
