// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/********************************************************************
   -------------------------------------------------------------------------
   File name: PathfinderNavigationSystemUser.h
   Описание: This class acts as a shell to allow the mnm pathfinder to
               correctly respects the safe update of the navigation
               system.
 *********************************************************************/

#ifndef PATHFINDERNAVIGATIONSYSTEMUSER_H
#define PATHFINDERNAVIGATIONSYSTEMUSER_H

#include <DinrusX/DinrusXAI/INavigationSystem.h>
#include <DinrusX/DinrusXAI/MNMPathfinder.h>

namespace MNM
{
class PathfinderNavigationSystemUser : public INavigationSystemUser
{
public:
	virtual ~PathfinderNavigationSystemUser() {}

	CMNMPathfinder* GetPathfinderImplementation() { return &m_pathfinder; }

	// INavigationSystemUser
	virtual void Reset()                                              { m_pathfinder.Reset(); }
	virtual void UpdateForSynchronousOrAsynchronousReadingOperation() { m_pathfinder.Update(); }
	virtual void UpdateForSynchronousWritingOperations()              {};
	virtual void CompleteRunningTasks()                               { m_pathfinder.WaitForJobsToFinish(); }
	// ~INavigationSystemUser

private:
	CMNMPathfinder m_pathfinder;
};
}

#endif // PATHFINDERNAVIGATIONSYSTEMUSER_H
