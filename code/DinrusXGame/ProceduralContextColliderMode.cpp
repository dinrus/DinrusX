// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/ProceduralContextColliderMode.h>


DRXREGISTER_CLASS( CProceduralContextColliderMode );


//////////////////////////////////////////////////////////////////////////
CProceduralContextColliderMode::CProceduralContextColliderMode()
{
}


//////////////////////////////////////////////////////////////////////////
CProceduralContextColliderMode::~CProceduralContextColliderMode()
{
}


//////////////////////////////////////////////////////////////////////////
void CProceduralContextColliderMode::Update( float timePassedSeconds )
{
	IAnimatedCharacter* pAnimatedCharacter = GetAnimatedCharacter();
	IF_UNLIKELY ( pAnimatedCharacter == NULL )
	{
		return;
	}

	EColliderMode mode = eColliderMode_Undefined;

	const size_t requestCount = m_requestList.GetCount();
	for ( size_t i = 0; i < requestCount; ++i )
	{
		const SColliderModeRequest& request = m_requestList.GetRequest( i );
		mode = ( request.mode != eColliderMode_Undefined ) ? request.mode : mode;
	}

	pAnimatedCharacter->RequestPhysicalColliderMode( mode, eColliderModeLayer_Animation, "ProcContextUpdate" );
}


//////////////////////////////////////////////////////////////////////////
uint32 CProceduralContextColliderMode::RequestColliderMode( const EColliderMode colliderMode )
{
	SColliderModeRequest request;
	request.mode = colliderMode;

	return m_requestList.AddRequest( request );
}


//////////////////////////////////////////////////////////////////////////
void CProceduralContextColliderMode::CancelRequest( const uint32 cancelRequestId )
{
	m_requestList.RemoveRequest( cancelRequestId );
}


//////////////////////////////////////////////////////////////////////////
IAnimatedCharacter* CProceduralContextColliderMode::GetAnimatedCharacter() const
{
	const EntityId entityId = m_entity->GetId();
	IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor( entityId );
	IAnimatedCharacter* pAnimatedCharacter = ( pActor != NULL ) ? pActor->GetAnimatedCharacter() : NULL;
	return pAnimatedCharacter;
}