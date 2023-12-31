// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------

Description: Automatic shotgun firemode. It works like the shotgun one, spawning
             several pellets on a single shot, but doesn't require 'pump' action
						 and it has a 'single' magazine reload

-------------------------------------------------------------------------
History:
- 14:09:09   Benito Gangoso Rodriguez

*************************************************************************/

#pragma once

#ifndef _AUTOMATIC_SHOTGUN_H_
#define _AUTOMATIC_SHOTGUN_H_

#include <DinrusX/DinrusXGame/Shotgun.h>

class CAutomaticShotgun : public CShotgun
{
public:
	DRX_DECLARE_GTI(CAutomaticShotgun);

	CAutomaticShotgun();
	virtual ~CAutomaticShotgun();

	virtual void Update(float frameTime, uint32 frameId) override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Activate(bool activate) override;
	virtual void StartReload(int zoomed) override;
	virtual void EndReload(int zoomed) override;

	virtual void CancelReload() override;
	virtual bool CanCancelReload() override;

	void GetMemoryUsage(IDrxSizer * s) const override
	{
		s->AddObject(this, sizeof(*this));	
		CShotgun::GetInternalMemoryUsage(s);		// collect memory of parent class
	}

private:
	float m_rapidFireCountdown;
	bool m_firing;
};

#endif