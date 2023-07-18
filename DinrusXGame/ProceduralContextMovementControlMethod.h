// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD__H__
#define __PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD__H__

#include <IDrxMannequin.h>

#include <DrxExtension/DrxCreateClassInstance.h>
#include <DrxExtension/ClassWeaver.h>
#include <DrxExtension/IDrxFactoryRegistryImpl.h>
#include <DrxExtension/RegFactoryNode.h>

#include <IAnimatedCharacter.h>

#include <DinrusX/DinrusXGame/ProceduralContextHelpers.h>


#define PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD_NAME "ProceduralContextMCM"


class CProceduralContextMovementControlMethod
	: public IProceduralContext
{
public:
	PROCEDURAL_CONTEXT( CProceduralContextMovementControlMethod, PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD_NAME, 0x80140507bdc64be4, 0xa24190f322270e82 );

	// IProceduralContext
	virtual void Update( float timePassedSeconds ) override;
	// ~IProceduralContext

	uint32 RequestMovementControlMethod( const EMovementControlMethod horizontal, const EMovementControlMethod vertical );
	void CancelRequest( const uint32 requestId );

private:
	IAnimatedCharacter* GetAnimatedCharacter() const;

private:
	struct SMCMRequest
	{
		uint32 id;
		EMovementControlMethod horizontal;
		EMovementControlMethod vertical;
	};

	typedef ProceduralContextHelpers::CRequestList< SMCMRequest > TMCMRequestList;
	TMCMRequestList m_requestList;
};


#endif
