// Copyright 2018-2023 DinrusPro / Dinrus Group. ÐÍÖÏ Äèíðóñ.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: C++ Item Implementation

-------------------------------------------------------------------------
History:
- 11:9:2004   15:00 : Created by Márcio Martins

*************************************************************************/
#ifndef __AUTOMATIC_H__
#define __AUTOMATIC_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <DinrusX/DinrusXGame/Single.h>

class CAutomatic : public CSingle
{
public:
	DRX_DECLARE_GTI(CAutomatic);
	
	CAutomatic();
	virtual ~CAutomatic();

	// CSingle
	virtual void StartReload(int zoomed) override;

	virtual void GetMemoryUsage(IDrxSizer * s) const override;
	void GetInternalMemoryUsage(IDrxSizer * s) const;
	virtual void Update(float frameTime, uint32 frameId) override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	// ~CSingle

private:
	
	typedef CSingle BaseClass;

};


#endif //__AUTOMATIC_H__