// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: K-Volt bullet

-------------------------------------------------------------------------
History:
- 13:05:2009   15:00 : Created by Claire Allan

*************************************************************************/
  
#include <DinrusX/DinrusXGame/Projectile.h>

class CKVoltBullet: public CProjectile
{
public:
	CKVoltBullet();
	virtual ~CKVoltBullet();

	// CProjectile
	virtual void HandleEvent(const SGameObjectEvent& event);
	virtual bool Init(IGameObject *pGameObject);
	// ~CProjectile

protected:

	void	Trigger(Vec3 collisionPos);
	void	DamageEnemiesInRange(float range, Vec3 pos, EntityId ignoreId);
	void	ProcessDamage(EntityId targetId, float damage, int hitPartId, Vec3 pos, Vec3 dir);

private:
	typedef CProjectile inherited;

};
