// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		void CTimeBudget::Restart(const CTimeValue& amountOfTimeFromNowOn)
		{
			m_futureTimestampOfExhaustion = gEnv->pTimer->GetAsyncTime() + amountOfTimeFromNowOn;
		}

		bool CTimeBudget::IsExhausted() const
		{
			return (gEnv->pTimer->GetAsyncTime() >= m_futureTimestampOfExhaustion);
		}

	}
}
