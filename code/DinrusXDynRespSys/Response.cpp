// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXDynRespSys/stdafx.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseCondition.h>
#include <DinrusX/CoreX/String/StringUtils.h>

#include <DinrusX/DinrusXDynRespSys/Response.h>
#include <DinrusX/DinrusXDynRespSys/ResponseInstance.h>
#include <DinrusX/DinrusXDynRespSys/ResponseUpr.h>
#include <DinrusX/DinrusXDynRespSys/ResponseSegment.h>
#include <DinrusX/DinrusXDynRespSys/VariableCollection.h>
#include <DinrusX/DinrusXDynRespSys/ResponseSystem.h>
#include <DinrusX/DinrusXDynRespSys/ConditionsCollection.h>
#include <DinrusX/CoreX/Serialization/Enum.h>
#include <DinrusX/CoreX/Serialization/Decorators/BitFlags.h>

using namespace DrxDRS;

void CConditionParserHelper::GetResponseVariableValueFromString(const char* szValueString, CVariableValue* pOutValue)
{
	for (int i = 0; szValueString[i] != '\0'; i++)
	{
		if (szValueString[i] == '.' && i > 0 && isdigit(szValueString[i+1]))
		{
			//it's (probably) a float
			float valueAsFloat = (float)atof(szValueString);
			if (valueAsFloat == 0.0f && szValueString[i-1] != '0') //atof will return 0 as an error code, so we do a very simple check, if 0 here means the value 0 or the error code
			{
				continue;
			}
			*pOutValue = valueAsFloat;
			return;

		}
		else if (szValueString[i] >= 'A' && szValueString[i] <= 'z')
		{
			if (stricmp(szValueString, "true") == 0)
			{
				//it's a boolean
				*pOutValue = CVariableValue(true);
			}
			else if (stricmp(szValueString, "false") == 0)
			{
				//it's a boolean
				*pOutValue = CVariableValue(false);
			}
			else
			{
				//its a string
				*pOutValue = CVariableValue(CHashedString(szValueString));
			}
			return;
		}
	}

	*pOutValue = CVariableValue(atoi(szValueString));
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

CResponseInstance* CResponse::StartExecution(SSignal& signal)
{
	DRS_DEBUG_DATA_ACTION(AddResponseStarted(signal.m_signalName.GetText()));
	DRS_DEBUG_DATA_ACTION(AddResponseSegmentEvaluated(&m_baseSegment));

	++m_executionCounter;
	CResponseInstance temp(signal, this);
	if (m_baseSegment.AreConditionsMet(&temp))
	{
		m_lastStartTime = CResponseSystem::GetInstance()->GetCurrentDrsTime();
		CResponseInstance* newResponseInstance = CResponseSystem::GetInstance()->GetResponseUpr()->CreateInstance(signal, this);
		newResponseInstance->Execute();
		return newResponseInstance;
	}
	else
	{
		--m_executionCounter;
		DRS_DEBUG_DATA_ACTION(AddResponseInstanceFinished(CResponseSystemDebugDataProvider::eER_NoValidSegment));
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------
void CResponse::Serialize(Serialization::IArchive& ar)
{
	ar(m_baseSegment, "BaseSegment", "+<");
}
