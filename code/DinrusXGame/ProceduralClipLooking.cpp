// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/IDrxMannequin.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXAnimation/IAnimationPoseModifier.h>
#include <DrxExtension/ClassWeaver.h>

#include <DinrusX/DinrusXGame/ProceduralContextLook.h>

class CProceduralClipLooking
	: public TProceduralContextualClip< CProceduralContextLook, SNoProceduralParams >
{
public:
		virtual void OnEnter( float blendTime, float duration, const SNoProceduralParams& params )
	{
		m_context->SetBlendInTime( blendTime );
		m_context->UpdateProcClipLookingRequest( true );
	}

	virtual void OnExit( float blendTime )
	{
		m_context->SetBlendOutTime( blendTime );
		m_context->UpdateProcClipLookingRequest( false );
	}

	virtual void Update( float timePassed )
	{
		m_context->UpdateProcClipLookingRequest( true );
	}
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipLooking, "Looking");
