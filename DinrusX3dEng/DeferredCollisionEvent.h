// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DeferredCollisionEvent.h
//  Version:     v1.00
//  Created:     12/08/2010 by Christopher Bolte
//  Compilers:   Visual Studio.NET
// -------------------------------------------------------------------------
//  History:
////////////////////////////////////////////////////////////////////////////

#ifndef _3DENGINE_DEFERREDCOLLISIONEVENT_H_
#define _3DENGINE_DEFERREDCOLLISIONEVENT_H_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include <DinrusX/DinrusXPhys/IDeferredCollisionEvent.h>

// Implementation class for the DeferredPhysicsEvent Upr
class CDeferredPhysicsEventUpr : public IDeferredPhysicsEventUpr, public DinrusX3dEngBase
{
public:
	virtual ~CDeferredPhysicsEventUpr() {}

	virtual void                   DispatchDeferredEvent(IDeferredPhysicsEvent* pEvent);
	virtual int                    HandleEvent(const EventPhys* pEvent, IDeferredPhysicsEventUpr::CreateEventFunc, IDeferredPhysicsEvent::DeferredEventType);

	virtual void                   RegisterDeferredEvent(IDeferredPhysicsEvent* pDeferredEvent);
	virtual void                   UnRegisterDeferredEvent(IDeferredPhysicsEvent* pDeferredEvent);

	virtual void                   ClearDeferredEvents();

	virtual void                   Update();

	virtual IDeferredPhysicsEvent* GetLastCollisionEventForEntity(IPhysicalEntity* pPhysEnt);

private:
	std::vector<IDeferredPhysicsEvent*> m_activeDeferredEvents; // list of all active deferred events, used for cleanup and statistics
	bool                                m_bEntitySystemReset;   // means all entity ptrs in events are stale
};

#endif // _3DENGINE_DEFERREDCOLLISIONEVENT_H_
