// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   VTOLVehicleUpr.h
//  Version:     v1.00
//  Created:     02/09/2011
//  Компиляторы:   mingw-w64-clang-x86_64
//  Description: Handles entities being attached to paths in multiplayer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __MPPATHFOLLOWINGMANAGER_H
#define __MPPATHFOLLOWINGMANAGER_H

#include <DinrusX/DinrusXGame/WaypointPath.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>

struct IMPPathFollowingListener
{
	virtual void OnPathCompleted(EntityId attachedEntityId) = 0;
	virtual ~IMPPathFollowingListener() {};
};

struct IMPPathFollower
{
	typedef uint8 MPPathIndex;

	virtual void OnAttachRequest(const struct SPathFollowingAttachToPathParameters& params, const CWaypointPath* pPath) = 0;
	virtual void OnUpdateSpeedRequest(EntityId attachEntityId, float speed) = 0;
	virtual ~IMPPathFollower() {};
};

class CMPPathFollowingUpr : public IEntityEventListener
{
public:
	CMPPathFollowingUpr();
	~CMPPathFollowingUpr();

	//Note: Currently supports one listener per map entry.
	void RegisterClassFollower(uint16 classId, IMPPathFollower* pFollower);
	void UnregisterClassFollower(uint16 classId);

	bool RegisterPath(EntityId pathEntityId);
	void UnregisterPath(EntityId pathEntityId);

	void RegisterListener(EntityId listenToEntityId, IMPPathFollowingListener* pListener);
	void UnregisterListener(EntityId listenToEntityId);

	const CWaypointPath* GetPath(EntityId pathEntityId, IMPPathFollower::MPPathIndex& outIndex) const;

	void RequestAttachEntityToPath(const struct SPathFollowingAttachToPathParameters& params);

	void RequestUpdateSpeed(uint16 classId, EntityId attachEntityId, float newSpeed);

	void NotifyListenersOfPathCompletion(EntityId pathFollowingEntityId);

	// IEntityEventListener
	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event );
	// ~IEntityEventListener

#ifndef _RELEASE
	void Update();
#endif //_RELEASE

private:
	typedef std::map<uint16, IMPPathFollower*> PathFollowers;
	PathFollowers m_PathFollowers;

	struct SPathEntry
	{
		SPathEntry(EntityId pathEntityId) : pathId(pathEntityId)
		{
		}

		EntityId pathId;
		CWaypointPath path;
	};

	typedef std::vector<SPathEntry> Paths;
	Paths m_Paths;

	typedef std::map<EntityId, IMPPathFollowingListener*> PathListeners;
	PathListeners m_PathListeners;
};

#endif