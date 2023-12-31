// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// ----------------------------------------------------------------------------------------
//  File name:   MFXAudioEffect.h
//  Описание: MFX system subclass which takes care of interfacing with audio system
//
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MFX_AUDIO_H_
#define _MFX_AUDIO_H_

#pragma once

#include "MFXEffectBase.h"
#include <DinrusX/CoreX/Audio/IAudioInterfacesCommonData.h>

struct SAudioTriggerWrapper
{
	SAudioTriggerWrapper()
		: m_triggerID(DrxAudio::InvalidControlId)
	{
	}

	void                Init(const char* triggerName);

	DrxAudio::ControlId GetTriggerId() const
	{
		return m_triggerID;
	}

	bool IsValid() const
	{
		return (m_triggerID != DrxAudio::InvalidControlId);
	}

	const char* GetTriggerName() const
	{
#if defined(MATERIAL_EFFECTS_DEBUG)
		return m_triggerName.c_str();
#else
		return "Trigger Unknown";
#endif
	}

	void GetMemoryUsage(IDrxSizer* pSizer) const
	{
#if defined(MATERIAL_EFFECTS_DEBUG)
		pSizer->Add(m_triggerName);
#endif
	}

private:

#if defined(MATERIAL_EFFECTS_DEBUG)
	string m_triggerName;
#endif
	DrxAudio::ControlId m_triggerID;
};

struct SAudioSwitchWrapper
{
	SAudioSwitchWrapper()
		: m_switchID(DrxAudio::InvalidControlId)
		, m_switchStateID(DrxAudio::InvalidSwitchStateId)
	{
	}

	void                Init(const char* switchName, const char* switchStateName);

	DrxAudio::ControlId GetSwitchId() const
	{
		return m_switchID;
	}

	DrxAudio::SwitchStateId GetSwitchStateId() const
	{
		return m_switchStateID;
	}

	bool IsValid() const
	{
		return (m_switchID != DrxAudio::InvalidControlId) && (m_switchStateID != DrxAudio::InvalidSwitchStateId);
	}

	const char* GetSwitchName() const
	{
#if defined(MATERIAL_EFFECTS_DEBUG)
		return m_switchName.c_str();
#else
		return "Switch Unknown";
#endif
	}

	const char* GetSwitchStateName() const
	{
#if defined(MATERIAL_EFFECTS_DEBUG)
		return m_switchStateName.c_str();
#else
		return "Switch State Unknown";
#endif
	}

	void GetMemoryUsage(IDrxSizer* pSizer) const
	{
#if defined(MATERIAL_EFFECTS_DEBUG)
		pSizer->Add(m_switchName);
		pSizer->Add(m_switchStateName);
#endif
	}

private:
#if defined(MATERIAL_EFFECTS_DEBUG)
	string m_switchName;
	string m_switchStateName;
#endif
	DrxAudio::ControlId m_switchID;
	DrxAudio::SwitchStateId m_switchStateID;
};

//////////////////////////////////////////////////////////////////////////

struct SMFXAudioEffectParams
{
	typedef std::vector<SAudioSwitchWrapper> TSwitches;

	void GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(&trigger);
		pSizer->Add(triggerSwitches);
	}

	SAudioTriggerWrapper trigger;
	TSwitches            triggerSwitches;
};

class CMFXAudioEffect :
	public CMFXEffectBase
{
public:
	CMFXAudioEffect();

	// IMFXEffect
	virtual void Execute(const SMFXRunTimeEffectParams& params) override;
	virtual void LoadParamsFromXml(const XmlNodeRef& paramsNode) override;
	virtual void GetResources(SMFXResourceList& resourceList) const override;
	virtual void GetMemoryUsage(IDrxSizer* pSizer) const override;
	//~IMFXEffect

protected:

	SMFXAudioEffectParams m_audioParams;
};

#endif // _MFX_AUDIO_H_
