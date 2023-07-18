// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/DinrusXSchema/GUID.h>

class CSchematycEntityDrsComponent final : public IEntityComponent, DRS::IResponseUpr::IListener, DRS::ISpeakerUpr::IListener
{
public:

	struct SResponseStartedSignal
	{
		static void ReflectType(Schematyc::CTypeDesc<SResponseStartedSignal>& typeInfo);

		int   m_signalId;
	};
	struct SResponseFinishedSignal
	{
		static void ReflectType(Schematyc::CTypeDesc<SResponseFinishedSignal>& typeInfo);

		int   m_signalId;
		int   m_result;  //ProcessingResult_NoResponseDefined, ProcessingResult_ConditionsNotMet, ProcessingResult_Done, ProcessingResult_Canceled	
	};
	struct SLineStartedSignal
	{
		static void ReflectType(Schematyc::CTypeDesc<SLineStartedSignal>& typeInfo);

		Schematyc::CSharedString  m_text;
		Schematyc::CSharedString  m_speakerName;
		//animation, audioTrigger... do we need these as well?
	};
	struct SLineEndedSignal
	{
		static void ReflectType(Schematyc::CTypeDesc<SLineEndedSignal>& typeInfo);

		Schematyc::CSharedString  m_text;
		Schematyc::CSharedString  m_speakerName;
		bool    m_bWasCanceled;
		//animation, audioTrigger... do we need these as well?
	};

	CSchematycEntityDrsComponent() = default;
	virtual ~CSchematycEntityDrsComponent() = default;

	//IEntityComponent
	virtual void Initialize() override;
	virtual void OnShutDown() override;
	// ~IEntityComponent

	// DRS::IResponseUpr::IListener
	virtual void OnSignalProcessingStarted(SSignalInfos& signal, DRS::IResponseInstance* pStartedResponse) override;
	virtual void OnSignalProcessingFinished(SSignalInfos& signal, DRS::IResponseInstance* pFinishedResponse, eProcessingResult outcome) override;
	// ~DRS::IResponseUpr::IListener

	// DRS::ISpeakerUpr::IListener
	virtual void OnLineEvent(const DRS::IResponseActor* pSpeaker, const CHashedString& lineID, DRS::ISpeakerUpr::IListener::eLineEvent lineEvent, const DRS::IDialogLine* pLine) override;
	// ~DRS::ISpeakerUpr::IListener

	static void ReflectType(Schematyc::CTypeDesc<CSchematycEntityDrsComponent>& desc);
	static void Register(Schematyc::IEnvRegistrar& registrar);

private:
	template <typename SIGNAL> inline void OutputSignal(const SIGNAL& signal)
	{
		if (GetEntity()->GetSchematycObject())
			GetEntity()->GetSchematycObject()->ProcessSignal(signal, m_guid);
	}

	void SendSignal(const Schematyc::CSharedString& signalName, const Schematyc::CSharedString& contextFloatName, float contextFloatValue, const Schematyc::CSharedString& contextStringName, const Schematyc::CSharedString& contextStringValue);
	
	void SetFloatVariable(const Schematyc::CSharedString& collectionName, const Schematyc::CSharedString& variableName, float value);
	void SetStringVariable(const Schematyc::CSharedString& collectionName, const Schematyc::CSharedString& variableName, const Schematyc::CSharedString& value);
	void SetIntVariable(const Schematyc::CSharedString& collectionName, const Schematyc::CSharedString& variableName, int value);

	DRS::IVariableCollection* GetVariableCollection(const Schematyc::CSharedString& collectionName);

	Schematyc::CSharedString m_nameOverride;
	Schematyc::CSharedString m_globalVariableCollectionToUse;

	IEntityDynamicResponseComponent* m_pDrsEntityComp;
};
