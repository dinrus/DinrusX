// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXMovie/AnimNode.h>

class CAudioNode final : public CAnimNode
{
public:
	CAudioNode(const int id);
	static void            Initialize();

	virtual EAnimNodeType  GetType() const override { return eAnimNodeType_Audio; }

	virtual void           Animate(SAnimContext& animContext) override;
	virtual void           CreateDefaultTracks() override {}

	virtual void           OnStart() override;
	virtual void           OnReset() override;
	virtual void           OnStop() override;

	virtual unsigned int   GetParamCount() const override;
	virtual CAnimParamType GetParamType(unsigned int index) const override;

protected:
	virtual bool GetParamInfoFromType(const CAnimParamType& paramId, SParamInfo& info) const override;

private:
	void ApplyAudioSwitchKey(DrxAudio::ControlId audioSwitchId, DrxAudio::SwitchStateId audioSwitchStateId);
	void ApplyAudioParameterValue(DrxAudio::ControlId audioParameterId, const float value);
	void ApplyAudioTriggerKey(DrxAudio::ControlId audioTriggerId, const bool bPlay = true);
	void ApplyAudioFileKey(const string& filePath, const bool bLocalized);

	std::vector<int>              m_audioSwitchTracks;
	std::vector<float>            m_audioParameterTracks;
	std::vector<SAudioInfo>       m_audioTriggerTracks;
	std::vector<SAudioInfo>       m_audioFileTracks;
	std::vector<SAudioTriggerKey> m_activeAudioTriggers;
};
