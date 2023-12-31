// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/WeaponZoomOffset.h>
#include <DinrusX/DinrusXGame/Weapon.h>
#include <DinrusX/DinrusXGame/WeaponSharedParams.h>
#include <DinrusX/DinrusXGame/Utility/Hermite.h>
#include <DinrusX/DinrusXGame/WeaponFPAiming.h>



CWeaponZoomOffset::CWeaponZoomOffset()
	:	m_zoomOffset(ZERO)
	,	m_zoomTransition(0.0f)
	,	m_zoomRotation(0.0f)
{
}



QuatT CWeaponZoomOffset::Compute(float frameTime)
{
	float zoomTransition = Hermite2(m_zoomTransition);

	SWeaponOffset shoulderTargetOffset = m_shoulderOffset.Blend(frameTime);

	Vec3 positionOffset(ZERO);
	Ang3 rotationOffset(ZERO);
	QuatT offset(IDENTITY);

	Vec3 shoulderTension = (m_zoomOffset.m_position - shoulderTargetOffset.m_position);
	shoulderTension.y = 0.0f;
	Vec3 ironSightTension = (m_zoomOffset.m_position - shoulderTargetOffset.m_position) * 2.5f;
	ironSightTension.x = 0.0f;
	ironSightTension.z = 0.0f;

	positionOffset = HermiteInterpolate(
		shoulderTargetOffset.m_position,
		shoulderTension,
		m_zoomOffset.m_position,
		ironSightTension,
		zoomTransition);

	rotationOffset = HermiteInterpolate(
		shoulderTargetOffset.m_rotation,
		Ang3(0, 0, -m_zoomRotation),
		m_zoomOffset.m_rotation,
		Ang3(0, 0, m_zoomRotation),
		zoomTransition);

	const QuatT poseOffset(positionOffset, Quat::CreateRotationXYZ(rotationOffset));

	return poseOffset;
}



QuatT CWeaponZoomOffset::GetLeftHandOffset(float frameTime)
{
	SWeaponOffset leftHandOffset = m_leftHandOffset.Blend(frameTime);
	return ToQuatT(leftHandOffset);
}



void CWeaponZoomOffset::SetZoomTransition(float transition)
{
	m_zoomTransition = transition;
}



void CWeaponZoomOffset::SetZoomTransitionRotation(float rotation)
{
	m_zoomRotation = rotation;
}
