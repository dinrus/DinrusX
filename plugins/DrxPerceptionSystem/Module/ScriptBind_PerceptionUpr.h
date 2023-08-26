// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXScript/IScriptSystem.h>

class CScriptBind_PerceptionUpr : public CScriptableBase
{
public:
	CScriptBind_PerceptionUpr(ISystem *pSystem);
	virtual ~CScriptBind_PerceptionUpr(void) {}

	virtual void GetMemoryUsage(IDrxSizer *pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
	}

	//! <code>Perception.SetPriorityType(AIObjectType)</code>
	//! <description>set the assesment multiplier factor for the given AIObject type.</description>
	//!		<param name="AIObjectType">AIObject type</param>
	int SetPriorityObjectType(IFunctionHandler* pH, int type);

	//! <code>Perception.SoundStimulus(position, radius, threat, interest, entityId)</code>
	//! <description>Generates a sound stimulus in the AI system with the given parameters.</description>
	//!		<param name="position">Position of the origin of the sound event</param>
	//!		<param name="radius">Radius in which this sound event should be heard</param>
	//!		<param name="type">Type of the sound event </param>
	//!		<param name="entityId">EntityId of the entity who generated this sound event</param>
	int SoundStimulus(IFunctionHandler* pH);
	
private:
	ISystem* m_pSystem;
	IScriptSystem* m_pScriptSystem;
};
