// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/IDrxMannequin.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXAnimation/IAnimationPoseModifier.h>
#include <DrxExtension/ClassWeaver.h>

#include <DinrusX/DinrusXGame/ProceduralContextAim.h>

class CProceduralClipAiming
: public TProceduralContextualClip<CProceduralContextAim, SNoProceduralParams>
{
public:
	virtual void OnEnter( float blendTime, float duration, const SNoProceduralParams& params )
	{
		m_context->SetBlendInTime( blendTime );
		m_context->UpdateProcClipAimingRequest( true );
	}

	virtual void OnExit( float blendTime )
	{
		m_context->SetBlendOutTime( blendTime );
		m_context->UpdateProcClipAimingRequest( false );
	}

	virtual void Update( float timePassed )
	{
		m_context->UpdateProcClipAimingRequest( true );
	}
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipAiming, "Aiming");
