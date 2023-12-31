// Copyright 2018-2023 DinrusPro / Dinrus Group. ���� ������.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Rock

-------------------------------------------------------------------------
History:
- 5:5:2006   15:24 : Created by M�rcio Martins

*************************************************************************/
#ifndef __CHAFF_H__
#define __CHAFF_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <DinrusX/DinrusXGame/Projectile.h>
#include <DinrusX/CoreX/Containers/VectorSet.h>

#define MAX_SPAWNED_CHAFFS	16

class CChaff : public CProjectile
{
public:
	CChaff();
	virtual ~CChaff();

	virtual void HandleEvent(const SGameObjectEvent &);

	virtual Vec3 GetPosition(void);

	static VectorSet<CChaff*> s_chaffs;
};


#endif // __CHAFF_H__