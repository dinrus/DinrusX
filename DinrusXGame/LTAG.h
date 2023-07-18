// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------

Description: LTAG Implementation

-------------------------------------------------------------------------
History:
- 16:09:09	: Created by Benito Gangoso Rodriguez

*************************************************************************/
#pragma once

#ifndef _LTAG_H_
#define _LTAG_H_

#include <DinrusX/DinrusXGame/Weapon.h>
#include <DinrusX/DinrusXGame/HandGrenades.h>

class CLTag :	public CWeapon
{
public:

	CLTag();
	virtual ~CLTag();

	virtual void Reset();
	virtual void FullSerialize( TSerialize ser );
	virtual void OnSelected(bool selected);
	
	virtual void StartFire(const SProjectileLaunchParams& launchParams);

	virtual void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value);

	virtual void GetMemoryUsage(IDrxSizer * s) const
	{
		s->AddObject(this, sizeof(*this));
		CWeapon::GetInternalMemoryUsage(s); // collect memory of parent class
	}
	virtual void ProcessEvent(SEntityEvent& event);
	//virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );
	//virtual void Select(bool select);
	virtual void StartChangeFireMode();

protected:

	virtual void AnimationEvent(ICharacterInstance *pCharacter, const AnimEventInstance &event);

private:
	void UpdateGrenades();
	void HideGrenadeAttachment(ICharacterInstance* pWeaponCharacter, const char* attachmentName, bool hide);

	typedef CWeapon inherited;

	bool OnActionSwitchFireMode(EntityId actorId, const ActionId& actionId, int activationMode, float value);
};

#endif // _LTAG_H_