// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAnimation/IAnimationPoseModifier.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>

class CPoseMatching :
	public IAnimationPoseMatching
{
public:
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IAnimationPoseModifier)
	DRXINTERFACE_ADD(IAnimationPoseMatching)
	DRXINTERFACE_END()

	DRXGENERATE_CLASS_GUID(CPoseMatching, "AnimationPoseModifier_PoseMatching", "18318a27-2246-464e-a4b7-adffa51a9508"_drx_guid)

	CPoseMatching();
	virtual ~CPoseMatching() {}

	// IAnimationPoseMatching
public:
	virtual void SetAnimations(const uint* pAnimationIds, uint count) override;
	virtual bool GetMatchingAnimation(uint& animationId) const override;

	// IAnimationPoseModifier
public:
	virtual bool Prepare(const SAnimationPoseModifierParams& params) override;
	virtual bool Execute(const SAnimationPoseModifierParams& params) override;
	virtual void Synchronize() override;

	void         GetMemoryUsage(IDrxSizer* pSizer) const override
	{
		pSizer->AddObject(this, sizeof(*this));
	}

private:
	uint        m_jointStartIndex;
	const uint* m_pAnimationIds;
	uint        m_animationCount;

	int         m_animationMatchId;
};
