// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAnimation/CharacterUpr.h>
#include <DinrusX/DinrusXAnimation/SkeletonPose.h>

namespace OperatorQueue
{
struct SValue
{
	f32    n[4];
	QuatT* p;
};

struct SOp
{
	uint32 joint;
	uint16 target;
	uint16 op;
	SValue value;
};
} // OperatorQueue

class DRX_ALIGN(32) COperatorQueue:
	public IAnimationOperatorQueue
{
public:
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IAnimationPoseModifier)
	DRXINTERFACE_ADD(IAnimationOperatorQueue)
	DRXINTERFACE_END()

	DRXGENERATE_CLASS_GUID(COperatorQueue, "AnimationPoseModifier_OperatorQueue", "ac90f2bc-76a8-43ec-9970-463fb080a520"_drx_guid);

	COperatorQueue();
	virtual ~COperatorQueue() {}

private:
	enum EOpInternal
	{
		eOpInternal_OverrideAbsolute = eOp_Override,
		eOpInternal_OverrideRelative = eOp_OverrideRelative,
		eOpInternal_OverrideWorld    = eOp_OverrideWorld,
		eOpInternal_AdditiveAbsolute = eOp_Additive,
		eOpInternal_AdditiveRelative = eOp_AdditiveRelative,

		eOpInternal_StoreRelative,
		eOpInternal_StoreAbsolute,
		eOpInternal_StoreWorld,

		eOpInternal_ComputeAbsolute,
	};

	enum ETarget
	{
		eTarget_Position,
		eTarget_Orientation,
	};

public:
	virtual void PushPosition(uint32 jointIndex, EOp eOp, const Vec3 &value) override;
	virtual void PushOrientation(uint32 jointIndex, EOp eOp, const Quat &value) override;

	virtual void PushStoreRelative(uint32 jointIndex, QuatT & output) override;
	virtual void PushStoreAbsolute(uint32 jointIndex, QuatT & output) override;
	virtual void PushStoreWorld(uint32 jointIndex, QuatT & output) override;

	virtual void PushComputeAbsolute() override;

	virtual void Clear() override;

	// IAnimationPoseModifier
public:
	virtual bool Prepare(const SAnimationPoseModifierParams &params) override;
	virtual bool Execute(const SAnimationPoseModifierParams &params) override;
	virtual void Synchronize() override;

	void GetMemoryUsage(IDrxSizer* pSizer) const override
	{
		pSizer->AddObject(this, sizeof(*this));
	}

private:
	std::vector<OperatorQueue::SOp> m_ops[2];
	uint32 m_current;
};
