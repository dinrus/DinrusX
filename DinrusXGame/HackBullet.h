// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _HACK_BULLET_H_
#define _HACK_BULLET_H_

#include <DinrusX/DinrusXGame/Projectile.h>



class CHackBullet : public CProjectile
{
private:
	virtual void HandleEvent(const SGameObjectEvent& event);
};



#endif
