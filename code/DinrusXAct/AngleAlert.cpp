// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/********************************************************************
   ---------------------------------------------------------------------
   File name:   AngleAlert.h
   $Id$
   $DateTime$
   Описание:    Срез под одним углом.
   ---------------------------------------------------------------------
   History:
   - 11:09:2007 : Created by Ricardo Pillosu

 *********************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/AngleAlert.h>
#include <DinrusX/DinrusXAct/PersonalRangeSignaling.h>
#include <DinrusX/DinrusXAI/IAIObject.h>
// Описание:
//   Constructor
// Arguments:
//
// Return:
//
CAngleAlert::CAngleAlert(CPersonalRangeSignaling* pPersonal) : m_pPersonal(pPersonal), m_fAngle(-1.0f), m_fBoundary(-1.0f), m_pSignalData(NULL)
{
	DRX_ASSERT(pPersonal != NULL);
}

// Описание:
//   Destructor
// Arguments:
//
// Return:
//
CAngleAlert::~CAngleAlert()
{
	if (gEnv->pAISystem)
		gEnv->pAISystem->FreeSignalExtraData(m_pSignalData);
}

// Описание:
//   Init
// Arguments:
//
// Return:
//
void CAngleAlert::Init(float fAngle, float fBoundary, const char* sSignal, IAISignalExtraData* pData /*= NULL*/)
{
	DRX_ASSERT(fAngle >= 1.0f);
	DRX_ASSERT(fBoundary >= 0.0f);
	DRX_ASSERT(sSignal != NULL);

	m_sSignal = sSignal;
	m_fAngle = DEG2RAD(fAngle);
	m_fBoundary = DEG2RAD(fBoundary) + m_fAngle;

	// Clone extra data
	if (pData && gEnv->pAISystem)
	{
		gEnv->pAISystem->FreeSignalExtraData(m_pSignalData);
		m_pSignalData = gEnv->pAISystem->CreateSignalExtraData();
		DRX_ASSERT(m_pSignalData);
		*m_pSignalData = *pData;
	}
}

// Описание:
//
// Arguments:
//
// Return:
//
bool CAngleAlert::Check(const Vec3& vPos) const
{
	return(GetAngleTo(vPos) < m_fAngle);
}

// Описание:
//
// Arguments:
//
// Return:
//
bool CAngleAlert::CheckPlusBoundary(const Vec3& vPos) const
{
	return(GetAngleTo(vPos) < m_fBoundary);
}

// Описание:
//
// Arguments:
//
// Return:
//
float CAngleAlert::GetAngleTo(const Vec3& vPos) const
{
	float fResult = 0.0f;
	IEntity* pEntity = m_pPersonal->GetEntity();
	DRX_ASSERT(pEntity);
	if (pEntity)
	{
		const Vec3& vEntityPos = pEntity->GetPos();
		const Vec3& vEntityDir = pEntity->GetAI()->GetViewDir();
		Vec3 vDiff = vPos - vEntityPos;
		vDiff.NormalizeSafe();
		fResult = acosf(vEntityDir.Dot(vDiff));
	}
	return fResult;
}
