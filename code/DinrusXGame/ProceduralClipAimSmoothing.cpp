// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/IDrxMannequin.h>
#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/DinrusXAnimation/IAnimationPoseModifier.h>
#include <DrxExtension/ClassWeaver.h>
#include <DrxSerialization/IArchive.h>

#include <DinrusX/DinrusXGame/ProceduralContextAim.h>


struct SAimSmoothingParams 
	: public IProceduralParams
{
	SAimSmoothingParams()
		: smoothTimeSeconds(0.1f)
		, maxYawDegreesPerSecond(360.0f)
		, maxPitchDegreesPerSecond(360.0f)
	{
	}

	virtual void Serialize(Serialization::IArchive& ar)
	{
		ar(smoothTimeSeconds, "SmoothTimeSeconds", "Smooth Time Seconds");
		ar(maxYawDegreesPerSecond, "MaxYawDegreesPerSecond", "Max Yaw Degrees Per Second");
		ar(maxPitchDegreesPerSecond, "MaxPitchDegreesPerSecond", "Max Pitch Degrees Per Second");
	}

	float smoothTimeSeconds;
	float maxYawDegreesPerSecond;
	float maxPitchDegreesPerSecond;
};

class CProceduralClipAimSmoothing
	: public TProceduralContextualClip<CProceduralContextAim, SAimSmoothingParams>
{
public:
	CProceduralClipAimSmoothing()
		: m_requestId( 0 )
	{
	}
	
	virtual void OnEnter( float blendTime, float duration, const SAimSmoothingParams& params )
	{
		m_requestId = m_context->RequestPolarCoordinatesSmoothingParameters( Vec2( DEG2RAD( params.maxYawDegreesPerSecond ), DEG2RAD( params.maxPitchDegreesPerSecond ) ), params.smoothTimeSeconds );
	}

	virtual void OnExit( float blendTime )
	{
		m_context->CancelPolarCoordinatesSmoothingParameters( m_requestId );
	}

	virtual void Update( float timePassed )
	{
	}

private:
	uint32 m_requestId;
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipAimSmoothing, "AimSmoothing");