// Copyright 2018-2023 DinrusPro / Dinrus Group. ���� ������.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Single-shot Fire Mode Implementation

-------------------------------------------------------------------------
History:
- 11:9:2004   15:00 : Created by M�rcio Martins

*************************************************************************/
#ifndef __CHARGE_H__
#define __CHARGE_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/DinrusXGame/EntityUtility/EntityEffects.h>
#include <DinrusX/DinrusXGame/Automatic.h>

class CCharge :
	public CAutomatic
{
public:
	DRX_DECLARE_GTI(CCharge);

	CCharge();
	virtual ~CCharge();

	virtual void Update(float frameTime, uint32 frameId) override;
	virtual void GetMemoryUsage(IDrxSizer * s) const override;

	virtual void Activate(bool activate) override;

	virtual void StartFire() override;
	virtual void StopFire() override;

	virtual bool Shoot(bool resetAnimation, bool autoreload, bool isRemote=false) override;

	virtual void ChargeEffect(bool attach);
	virtual void ChargedShoot();
	
protected:

	int							m_charged;
	bool						m_charging;
	float						m_chargeTimer;
	bool						m_autoreload;

	EntityEffects::TAttachedEffectId m_chargeEffectId;
	float						m_chargedEffectTimer;
};

#endif //__CHARGE_H__
