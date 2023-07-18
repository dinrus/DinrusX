// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXNetwork/SerializeFwd.h>

struct IAIObject;
struct IAIActor;

class CPerceptionActor
{
public:
	CPerceptionActor();
	CPerceptionActor(IAIActor* pAIActor);

	void Update(float frameDelta);
	void Reset();

	void Serialize(TSerialize ser);

	void Enable(bool enable);
	bool IsEnabled() const;

	void CheckCloseContact(IAIObject* pTarget);
	bool CloseContactEnabled() { return m_closeContactTimeOut == 0.0f; }

	void SetMeleeRange(float meleeRange) { m_meleeRange = meleeRange; }

private:
	IAIActor* m_pAIActor;

	int m_perceptionDisabled;

	float m_meleeRange;
	float m_closeContactTimeOut; // used to prevent the signal OnCloseContact being sent repeatedly to same object
};