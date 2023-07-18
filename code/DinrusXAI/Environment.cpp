// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   AIEnvironment.cpp
//  Created:     29/02/2008 by Matthew
//  Описание: Simple global struct for accessing major module pointers
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/Environment.h>

#include <DinrusX/DinrusXAI/ObjectContainer.h>

#include <DinrusX/DinrusXAI/GoalOpFactory.h>
#include <DinrusX/DinrusXAI/StatsUpr.h>
#include <DinrusX/DinrusXAI/TacticalPointSystem.h>
#include <DinrusX/DinrusXAI/TargetTrackUpr.h>
#include <DinrusX/DinrusXAI/NullAIDebugRenderer.h>
#include <DinrusX/DinrusXAI/NavigationSystem.h>
#include <DinrusX/DinrusXAI/BehaviorTreeGraft.h>
#include <DinrusX/DinrusXAI/FormationUpr.h>

static CNullAIDebugRenderer nullAIRenderer;

SAIEnvironment::SAIEnvironment()
	: pActorLookUp(NULL)
	, pGoalOpFactory(NULL)
	, pObjectContainer(NULL)
	, pTacticalPointSystem(NULL)
	, pTargetTrackUpr(NULL)
	, pStatsUpr(NULL)
	, pPipeUpr(NULL)
	, pAIActionUpr(NULL)
	, pSmartObjectUpr(NULL)
	, pCommunicationUpr(NULL)
	, pCoverSystem(NULL)
	, pNavigationSystem(NULL)
	, pBehaviorTreeUpr(NULL)
	, pGraftUpr(NULL)
	, pAuditionMap(NULL)
	, pVisionMap(NULL)
	, pFactionMap(NULL)
	, pFactionSystem(NULL)
	, pGroupUpr(NULL)
	, pCollisionAvoidanceSystem(NULL)
	, pRayCaster(NULL)
	, pIntersectionTester(NULL)
	, pMovementSystem(NULL)
	, pSequenceUpr(NULL)
	, pAIObjectUpr(NULL)
	, pPathfinderNavigationSystemUser(NULL)
	, pMNMPathfinder(NULL)
	, pNavigation(NULL)
	, pClusterDetector(NULL)
	, pFormationUpr(NULL)
	, pWorld(NULL)
{
	SetDebugRenderer(0);
	SetNetworkDebugRenderer(0);

#ifdef DRXAISYS_DEBUG
	pRecorder = NULL;
	pBubblesSystem = NULL;
#endif //DRXAISYS_DEBUG
}

SAIEnvironment::~SAIEnvironment()
{
}

void SAIEnvironment::ShutDown()
{
	SAFE_DELETE(pActorLookUp);
	SAFE_DELETE(pFactionMap);
	SAFE_DELETE(pFactionSystem);
	SAFE_DELETE(pGoalOpFactory);
	SAFE_DELETE(pStatsUpr);
	SAFE_DELETE(pTacticalPointSystem);
	SAFE_DELETE(pTargetTrackUpr);
	SAFE_DELETE(pObjectContainer);
	SAFE_DELETE(pFormationUpr);
}

IAIDebugRenderer* SAIEnvironment::GetDebugRenderer()
{
	return pDebugRenderer;
}

IAIDebugRenderer* SAIEnvironment::GetNetworkDebugRenderer()
{
	return pNetworkDebugRenderer;
}

void SAIEnvironment::SetDebugRenderer(IAIDebugRenderer* pAIDebugRenderer)
{
	pDebugRenderer = pAIDebugRenderer ? pAIDebugRenderer : &nullAIRenderer;
}

void SAIEnvironment::SetNetworkDebugRenderer(IAIDebugRenderer* pAINetworkDebugRenderer)
{
	pNetworkDebugRenderer = pAINetworkDebugRenderer ? pAINetworkDebugRenderer : &nullAIRenderer;
}

#ifdef DRXAISYS_DEBUG

CAIRecorder* SAIEnvironment::GetAIRecorder()
{
	return pRecorder;
}

void SAIEnvironment::SetAIRecorder(CAIRecorder* pAIRecorder)
{
	pRecorder = pAIRecorder;
}

#endif //DRXAISYS_DEBUG

SAIEnvironment gAIEnv;
