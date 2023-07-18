// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: DebugGun Implementation

-------------------------------------------------------------------------
History:
- 09:01:2006   14:00 : Created by Michael Rauh

*************************************************************************/
#ifndef __DebugGun_H__
#define __DebugGun_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <DinrusX/DinrusXAct/IItemSystem.h>
#include <DinrusX/DinrusXGame/Weapon.h>


class CDebugGun :
  public CWeapon
{
public:
  CDebugGun();
  void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value);
  void Update(SEntityUpdateContext& ctx, int update);
  void Shoot( bool bPrimary);
	virtual void GetMemoryUsage(IDrxSizer * s) const
	{
		s->AddObject(this, sizeof(*this));		
		s->AddContainer(m_fireModes);
		CWeapon::GetInternalMemoryUsage(s); // collect memory of parent class
	}

  virtual void Select(bool select);

private:
  ICVar* m_pAIDebugDraw;
  int m_aiDebugDrawPrev;
  
  typedef std::pair<string, float> TFmPair;
  std::vector<TFmPair> m_fireModes;    
  int m_fireMode;
};

#endif // __DebugGun_H__