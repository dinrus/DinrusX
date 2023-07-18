// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Makes the vehicle a target for auto asist

*************************************************************************/
#ifndef __VEHICLEACTIONAUTOTARGET_H__
#define __VEHICLEACTIONAUTOTARGET_H__

#include <DinrusX/DinrusXGame/IVehicleSystem.h>
#include <DinrusX/DinrusXGame/AutoAimUpr.h>

class CVehicleActionAutoTarget
	: public IVehicleAction
{
	IMPLEMENT_VEHICLEOBJECT;

public:
	CVehicleActionAutoTarget();
	virtual ~CVehicleActionAutoTarget();

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override;

	virtual int OnEvent(int eventType, SVehicleEventParams& eventParams) override;
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

	// IVehicleObject
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) override {}
	virtual void Update(const float deltaTime) override {}
	// ~IVehicleObject

protected:
	SAutoaimTargetRegisterParams m_autoAimParams;

	IVehicle* m_pVehicle;
	bool m_RegisteredWithAutoAimUpr;

private:
	static const char * m_name;
};

#endif //__VEHICLEACTIONAUTOTARGET_H__
