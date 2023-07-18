// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/************************************************************************

A null implementation of the Dynamic Response System, that does nothing. Just a stub, if a real implementation was not provided or failed to initialize

/************************************************************************/

#pragma once

#include <DinrusX/DinrusXDynRespSys/IDynamicResponseSystem.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseCondition.h>
#include <DinrusX/CoreX/Serialization/ClassFactory.h>

namespace NullDRS
{
	class CDialogLineDatabase final : public DRS::IDialogLineDatabase
	{
		virtual bool Save(const char* szFilePath) override { return false; }
		virtual uint32 GetLineSetCount() const override { return 0; }
		virtual DRS::IDialogLineSet* GetLineSetByIndex(uint32 index) override { return nullptr; }
		virtual DRS::IDialogLineSet* GetLineSetById(const CHashedString& lineID) override { return nullptr; }
		virtual DRS::IDialogLineSet* InsertLineSet(uint32 index) override { return nullptr; }
		virtual bool RemoveLineSet(uint32 index) override { return false; }
		virtual bool ExecuteScript(uint32 index) override { return false; }
		virtual void Serialize(Serialization::IArchive& ar) override {}
		virtual void SerializeLinesHistory(Serialization::IArchive& ar) override {}
	};

//////////////////////////////////////////////////////////

	class CSpeakerUpr final : public DRS::ISpeakerUpr
	{
		virtual DRS::ISpeakerUpr::IListener::eLineEvent StartSpeaking(DRS::IResponseActor* pActor, const CHashedString& lineID) override { return DRS::ISpeakerUpr::IListener::eLineEvent_SkippedBecauseOfFaultyData; }
		virtual bool IsSpeaking(const DRS::IResponseActor* pActor, const CHashedString& lineID = CHashedString::GetEmpty(), bool bCheckQueuedLinesAsWell = false) const override { return false; }
		virtual bool CancelSpeaking(const DRS::IResponseActor* pActor, int maxPrioToCancel = -1, const CHashedString& lineID = CHashedString::GetEmpty(), bool bCancelQueuedLines = true) override { return false; }
		virtual bool AddListener(DRS::ISpeakerUpr::IListener* pListener) override { return false; }
		virtual bool RemoveListener(DRS::ISpeakerUpr::IListener* pListener) override { return false; }
		virtual void SetCustomLipsyncProvider(DRS::ISpeakerUpr::ILipsyncProvider* pProvider) override {}
	};

//////////////////////////////////////////////////////////

	class CResponseUpr final : public DRS::IResponseUpr
	{
		virtual bool AddListener(DRS::IResponseUpr::IListener* pNewListener, DRS::SignalInstanceId onlySignalWithID = DRS::s_InvalidSignalId) override { return false; }
		virtual bool RemoveListener(DRS::IResponseUpr::IListener* pListenerToRemove) override { return false; }
		virtual DynArray<const char *> GetRecentSignals(eSignalFilter filter = eSF_All) override { return DynArray<const char*>(); }
		virtual void SerializeResponse(Serialization::IArchive& ar, const DynArray<stack_string>& signalNames, DRS::IResponseActor* pActorForEvaluation = nullptr) override {}
		virtual void SerializeRecentResponse(Serialization::IArchive& ar, const DynArray<stack_string>& signalNames, int maxElemets = -1) override {}
		virtual void SerializeVariableChanges(Serialization::IArchive& ar, const stack_string& variableName = "ALL", int maxElemets = -1) override {}
		virtual bool AddResponse(const stack_string& signalName) override { return false; }
		virtual bool RemoveResponse(const stack_string& signalName) override { return false; }
	};

//////////////////////////////////////////////////////////

	class CVariableCollection final : public DRS::IVariableCollection
	{
	public:
		virtual DRS::IVariable*      CreateVariable(const CHashedString& name, int initialValue) override { return nullptr; }
		virtual DRS::IVariable*      CreateVariable(const CHashedString& name, float initialValue) override { return nullptr; }
		virtual DRS::IVariable*      CreateVariable(const CHashedString& name, bool initialValue) override { return nullptr; }
		virtual DRS::IVariable*      CreateVariable(const CHashedString& name, const CHashedString& initialValue) override { return nullptr; }
		virtual bool                 SetVariableValue(const CHashedString& name, int newValue, bool createIfNotExisting = true, float resetTime = -1.0f) override { return true; }
		virtual bool                 SetVariableValue(const CHashedString& name, float newValue, bool createIfNotExisting = true, float resetTime = -1.0f) override { return true; }
		virtual bool                 SetVariableValue(const CHashedString& name, bool newValue, bool createIfNotExisting = true, float resetTime = -1.0f) override { return true; }
		virtual bool                 SetVariableValue(const CHashedString& name, const CHashedString& newValue, bool createIfNotExisting = true, float resetTime = -1.0f) override { return true; }
		virtual DRS::IVariable*      GetVariable(const CHashedString& name) const override { return nullptr; }
		virtual const CHashedString& GetName() const override { return m_name; }
		virtual void                 SetUserString(const char* szUserString) override {}
		virtual const char*          GetUserString() const override { return nullptr; }
		virtual void                 Serialize(Serialization::IArchive& ar) override {}

	private:
		const CHashedString m_name = "NullVariableCollection";
		const string        m_userString = "Null";
	};

//////////////////////////////////////////////////////////

	class CResponseActor final : public DRS::IResponseActor
	{
	public:
		virtual const string&                   GetName() const override { return m_name; }
		virtual DRS::IVariableCollection*       GetLocalVariables() override { return static_cast<DRS::IVariableCollection*>(&m_pLocalVariables); }
		virtual const DRS::IVariableCollection* GetLocalVariables() const override { return static_cast<const DRS::IVariableCollection*>(&m_pLocalVariables); }
		virtual EntityId                        GetLinkedEntityID() const override { return INVALID_ENTITYID; }
		virtual IEntity*                        GetLinkedEntity() const override { return nullptr; }
		virtual void                            SetAuxAudioObjectID(DrxAudio::AuxObjectId overrideAuxProxy) override {}
		virtual DrxAudio::AuxObjectId           GetAuxAudioObjectID() const override { return DrxAudio::InvalidAuxObjectId; }
		virtual DRS::SignalInstanceId           QueueSignal(const CHashedString& signalName, DRS::IVariableCollectionSharedPtr pSignalContext = nullptr, DRS::IResponseUpr::IListener* pSignalListener = nullptr) override { return DRS::s_InvalidSignalId; }

	private:
		const string m_name = "NullResponseActor";
		CVariableCollection m_pLocalVariables;
	};

//////////////////////////////////////////////////////////

	typedef std::shared_ptr<CVariableCollection> NullVariableCollectionSharedPtr;

	struct CSystem final : public DRS::IDynamicResponseSystem
	{
	public:
		CSystem() : m_nullVariableCollectionSharedPtr(new CVariableCollection) {}

		virtual bool                                     Init() override { return true; }
		virtual bool                                     ReInit() override { return true; }
		virtual void                                     Update() override {}
		virtual DRS::IVariableCollection*                CreateVariableCollection(const CHashedString& name) override { return &m_nullVariableCollection; }
		virtual void                                     ReleaseVariableCollection(DRS::IVariableCollection* pToBeReleased) override {}
		virtual DRS::IVariableCollection*                GetCollection(const CHashedString& name) override { return &m_nullVariableCollection; }
		virtual DRS::IVariableCollection*                GetCollection(const CHashedString& name, DRS::IResponseInstance* pResponseInstance) override { return &m_nullVariableCollection; }
		virtual DRS::IVariableCollectionSharedPtr        CreateContextCollection() override { return m_nullVariableCollectionSharedPtr; }
		virtual bool                                     CancelSignalProcessing(const CHashedString& signalName, DRS::IResponseActor* pSender, DRS::SignalInstanceId instanceToSkip) override { return true; }
		virtual DRS::IResponseActor*                     CreateResponseActor(const char* szActorName, EntityId entityID = INVALID_ENTITYID, const char* szGlobalVariableCollectionToUse = nullptr) override { return &m_nullActor; }
		virtual bool                                     ReleaseResponseActor(DRS::IResponseActor* pActorToFree) override { return true; }
		virtual DRS::IResponseActor*                     GetResponseActor(const CHashedString& pActorName) override { return nullptr; }
		virtual void                                     Reset(uint32 resetHint = -1) override {}
		virtual DRS::IDataImportHelper*                  GetCustomDataformatHelper() override { return nullptr; }
		virtual void                                     Serialize(Serialization::IArchive& ar) override { static string dummy = "NullResponseSystem"; ar(dummy, "name", "Name"); }
		virtual DRS::ActionSerializationClassFactory&    GetActionSerializationFactory() override { return Serialization::ClassFactory<DRS::IResponseAction>::the(); }
		virtual DRS::ConditionSerializationClassFactory& GetConditionSerializationFactory() override { return Serialization::ClassFactory<DRS::IResponseCondition>::the(); }
		virtual DRS::ISpeakerUpr*                    GetSpeakerUpr() const override { return const_cast<CSpeakerUpr*>(&m_nullSpeakerUpr); }
		virtual DRS::IDialogLineDatabase*                GetDialogLineDatabase() const override { return const_cast<CDialogLineDatabase*>(&m_nullLineDatabase); }
		virtual DRS::IResponseUpr*                   GetResponseUpr() const override { return const_cast<CResponseUpr*>(&m_nullResponseUpr); }
		virtual DRS::ValuesListPtr                       GetCurrentState(uint32 saveHints) const override { return nullptr; }
		virtual void                                     SetCurrentState(const DRS::ValuesList& outCollectionsList) override {}

	private:
		CVariableCollection             m_nullVariableCollection;
		NullVariableCollectionSharedPtr m_nullVariableCollectionSharedPtr;
		CResponseActor                  m_nullActor;
		CResponseUpr                m_nullResponseUpr;
		CSpeakerUpr                 m_nullSpeakerUpr;
		CDialogLineDatabase             m_nullLineDatabase;
	};
}  //namespace NullDRS
