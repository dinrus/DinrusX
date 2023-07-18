// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAnimation/IDrxAnimation.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>
#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXAnimation/IAnimationPoseModifier.h>

struct IAnimationPoseData;

class CPoseModifierStack :
	public IAnimationPoseModifier
{
public:
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IAnimationPoseModifier)
	DRXINTERFACE_END()

	DRXGENERATE_CLASS_GUID(CPoseModifierStack, "AnimationPoseModifier_PoseModifierStack", "af9efa2d-fec0-4de4-a166-3950bde6a3c6"_drx_guid)

	CPoseModifierStack();
	virtual ~CPoseModifierStack() {}

public:
	void Clear() { m_modifiers.clear(); }
	bool Push(IAnimationPoseModifierPtr instance);

	// IAnimationPoseModifier
public:
	virtual bool Prepare(const SAnimationPoseModifierParams& params) override;
	virtual bool Execute(const SAnimationPoseModifierParams& params) override;
	virtual void Synchronize() override;

	void         GetMemoryUsage(IDrxSizer* pSizer) const override {}

private:
	std::vector<IAnimationPoseModifierPtr> m_modifiers;
};

DECLARE_SHARED_POINTERS(CPoseModifierStack);

//

class CPoseModifierSetup :
	public IAnimationPoseModifierSetup
{
	DRXINTERFACE_BEGIN()
		DRXINTERFACE_ADD(IAnimationPoseModifierSetup)
	DRXINTERFACE_END()

	DRXGENERATE_CLASS_GUID(CPoseModifierSetup, "PoseModifierSetup", "18b8cca7-6db9-47cc-84dd-1f003e97cbee"_drx_guid)

	virtual ~CPoseModifierSetup() {}

private:
	struct Entry
	{
		Entry() : enabled(true) {}

		IAnimationPoseModifierPtr instance;
		bool                      enabled;

		void                      Serialize(Serialization::IArchive& ar);
	};

public:
	bool                  Create(CPoseModifierSetup& setup);
	CPoseModifierStackPtr GetPoseModifierStack() { return m_pPoseModifierStack; }

private:
	bool CreateStack();

	// IAnimationSerializable
public:
	virtual void Serialize(Serialization::IArchive& ar) override;

	// IAnimationPoseModifierSetup
public:
	virtual IAnimationPoseModifier* GetEntry(int index) override;
	virtual int GetEntryCount() override;

private:
	std::vector<Entry>    m_modifiers;
	CPoseModifierStackPtr m_pPoseModifierStack;
};

DECLARE_SHARED_POINTERS(CPoseModifierSetup);
