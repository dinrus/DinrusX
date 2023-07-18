// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __PROCEDURAL_CONTEXT_COLLIDER_MODE__H__
#define __PROCEDURAL_CONTEXT_COLLIDER_MODE__H__

#include <IDrxMannequin.h>

#include <DrxExtension/DrxCreateClassInstance.h>
#include <DrxExtension/ClassWeaver.h>
#include <DrxExtension/IDrxFactoryRegistryImpl.h>
#include <DrxExtension/RegFactoryNode.h>

#include <IAnimatedCharacter.h>

#include <DinrusX/DinrusXGame/ProceduralContextHelpers.h>


#define PROCEDURAL_CONTEXT_COLLIDER_MODE_NAME "ProceduralContextColliderMode"


class CProceduralContextColliderMode
	: public IProceduralContext
{
public:
	PROCEDURAL_CONTEXT( CProceduralContextColliderMode, PROCEDURAL_CONTEXT_COLLIDER_MODE_NAME, 0x2857e483964b45e4, 0x8e9e6a481db8c166 );

	// IProceduralContext
	virtual void Update( float timePassedSeconds ) override;
	// ~IProceduralContext

	uint32 RequestColliderMode( const EColliderMode colliderMode );
	void CancelRequest( const uint32 requestId );

private:
	IAnimatedCharacter* GetAnimatedCharacter() const;

private:
	struct SColliderModeRequest
	{
		uint32 id;
		EColliderMode mode;
	};

	typedef ProceduralContextHelpers::CRequestList< SColliderModeRequest > TColliderModeRequestList;
	TColliderModeRequestList m_requestList;
};


#endif
