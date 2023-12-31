// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Player ability manager.
-------------------------------------------------------------------------
History:
- 14:04:2014: Created by Jean Geffroy
*************************************************************************/

#ifndef __MANUAL_FRAME_STEP_H__
#define __MANUAL_FRAME_STEP_H__

#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesModuleRMIListener.h>

class CManualFrameStepUpr
	: public IInputEventListener
	, public IGameRulesModuleRMIListener
{
public:
	CManualFrameStepUpr();
	virtual ~CManualFrameStepUpr();

	bool Update();
	
	bool IsEnabled() const;
	void Enable(bool enable);
	void Toggle();
	void RequestFrames(uint8 numFrames);

	// IInputEventListener
	virtual bool OnInputEvent(const SInputEvent& inputEvent);
	// ~IInputEventListener

	// IGameRulesModuleRMIListener
	virtual void OnSingleEntityRMI(CGameRules::SModuleRMIEntityParams params);
	virtual void OnDoubleEntityRMI(CGameRules::SModuleRMITwoEntityParams params){}
	virtual void OnEntityWithTimeRMI(CGameRules::SModuleRMIEntityTimeParams params){}
	virtual void OnSvClientActionRMI(CGameRules::SModuleRMISvClientActionParams params, EntityId fromEid){}
	// ~IGameRulesModuleRMIListener

private:
	int         m_rmiListenerIdx;
	int32       m_framesLeft;
	float       m_previousFixedStep;
};

#endif
