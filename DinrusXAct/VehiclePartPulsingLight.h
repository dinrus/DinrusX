// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   VehiclePartPulsingLight.h
//  Version:     v1.00
//  Created:     03/14/2011
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: A vehicle light which pulses over time
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __VEHICLEPARTPULSINGLIGHT_H__
#define __VEHICLEPARTPULSINGLIGHT_H__

#include "VehiclePartLight.h"

class CVehiclePartPulsingLight
	: public CVehiclePartLight
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartPulsingLight();

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo, int partType) override;
	virtual void OnEvent(const SVehiclePartEvent& event) override;
	// ~IVehiclePart

	virtual void ToggleLight(bool enable) override;

protected:

	virtual void UpdateLight(const float frameTime) override;

	float m_colorMult;
	float m_minColorMult;
	float m_currentColorMultSpeed;

	//Data from XML
	float m_colorMultSpeed;
	float m_toggleOnMinDamageRatio;

	float m_colorMultSpeedStageTwo;
	float m_toggleStageTwoMinDamageRatio;

	float m_colorChangeTimer;
};

#endif //__VEHICLEPARTPULSINGLIGHT_H__
