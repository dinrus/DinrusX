// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXDynRespSys/stdafx.h>
#include <DinrusX/DinrusXDynRespSys/ActionWait.h>
#include <DinrusX/DinrusXDynRespSys/ResponseSystem.h>

#include <DinrusX/CoreX/String/StringUtils.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>
#include <DinrusX/CoreX/Math/Random.h>

using namespace DrxDRS;
//--------------------------------------------------------------------------------------------------
DRS::IResponseActionInstanceUniquePtr CActionWait::Execute(DRS::IResponseInstance* pResponseInstance)
{
	return DRS::IResponseActionInstanceUniquePtr(new CActionWaitInstance(drx_random(m_minTimeToWait, m_maxTimeToWait)));
}

//--------------------------------------------------------------------------------------------------
string CActionWait::GetVerboseInfo() const
{
	return string().Format("min '%f', max '%f'", m_minTimeToWait, m_maxTimeToWait);
}

//--------------------------------------------------------------------------------------------------
void CActionWait::Serialize(Serialization::IArchive& ar)
{
	ar(m_minTimeToWait, "Time", "^Min Time (in seconds)");
	ar(m_maxTimeToWait, "MaxTime", "^Max Time (in seconds)");
	if (m_maxTimeToWait < m_minTimeToWait)
	{
		m_maxTimeToWait = m_minTimeToWait;
	}
}

//--------------------------------------------------------------------------------------------------
CActionWaitInstance::CActionWaitInstance(float timeToWait)
{
	m_finishTime = CResponseSystem::GetInstance()->GetCurrentDrsTime() + timeToWait;
}

//--------------------------------------------------------------------------------------------------
DRS::IResponseActionInstance::eCurrentState CActionWaitInstance::Update()
{
	if (CResponseSystem::GetInstance()->GetCurrentDrsTime() > m_finishTime)
	{
		return DRS::IResponseActionInstance::CS_FINISHED;
	}
	return DRS::IResponseActionInstance::CS_RUNNING;
}
