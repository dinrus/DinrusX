// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXDynRespSys/stdafx.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>
#include <DinrusX/DinrusXDynRespSys/ActionResetTimer.h>
#include <DinrusX/DinrusXDynRespSys/VariableCollection.h>
#include <DinrusX/DinrusXDynRespSys/ResponseSystem.h>
#include <DinrusX/DinrusXDynRespSys/ResponseInstance.h>

using namespace DrxDRS;

//////////////////////////////////////////////////////////
DRS::IResponseActionInstanceUniquePtr CActionResetTimerVariable::Execute(DRS::IResponseInstance* pResponseInstance)
{
	CVariableCollection* pCollectionToUse = GetCurrentCollection(static_cast<CResponseInstance*>(pResponseInstance));
	if (!pCollectionToUse)
	{
		CVariableCollectionUpr* pVcUpr = CResponseSystem::GetInstance()->GetVariableCollectionUpr();
		pCollectionToUse = pVcUpr->CreateVariableCollection(m_collectionName);
	}
	if (pCollectionToUse)
	{
		pCollectionToUse->SetVariableValue(m_variableName, CResponseSystem::GetInstance()->GetCurrentDrsTime());
	}
	return nullptr;
}

//////////////////////////////////////////////////////////
string CActionResetTimerVariable::GetVerboseInfo() const
{
	return "in variable" + GetVariableVerboseName();
}

//////////////////////////////////////////////////////////
void CActionResetTimerVariable::Serialize(Serialization::IArchive& ar)
{
	_Serialize(ar, "^TimerVariable");

#if defined (ENABLE_VARIABLE_VALUE_TYPE_CHECKINGS)
	CVariableCollection* pCollection = CResponseSystem::GetInstance()->GetCollection(m_collectionName);
	if (pCollection)
	{
		if (pCollection->GetVariableValue(m_variableName).GetType() != eDRVT_Float && pCollection->GetVariableValue(m_variableName).GetType() != eDRVT_Undefined)
		{
			ar.warning(m_collectionName.m_textCopy, "Variable to set needs to hold a time value as float");
		}
	}
#endif
}
