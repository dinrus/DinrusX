// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptRegistry.h>

#include <DinrusX/DinrusXSchema/ScriptSerializers.h>

namespace Schematyc
{
// Forward declare interfaces.
struct IString;
// Forward declare classes.
class CScript;
class CScriptRoot;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CScript)

class CScriptRegistry : public IScriptRegistry
{
private:
	friend class CScript;

	typedef std::unordered_map<DrxGUID, CScriptPtr>                 ScriptsByGuid;
	typedef std::unordered_map<uint32 /* Crc32 hash */, CScriptPtr> ScriptsByFileName;
	typedef std::unordered_map<DrxGUID, IScriptElementPtr>          Elements; // #SchematycTODO : Would it make more sense to store by raw pointer here and make ownership exclusive to script file?
	typedef std::vector<SScriptRegistryChange>                      ChangeQueue;

	struct SSignals
	{
		ScriptRegistryChangeSignal change;
	};

public:

	CScriptRegistry();

	// IScriptRegistry

	// Compatibility interface.
	//////////////////////////////////////////////////

	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual bool Load() override;
	virtual void Save(bool bAlwaysSave = false) override;

	// New interface.
	//////////////////////////////////////////////////

	virtual bool                               IsValidScope(EScriptElementType elementType, IScriptElement* pScope) const override;
	virtual bool                               IsValidName(const char* szName, IScriptElement* pScope, const char*& szErrorMessage) const override;

	virtual IScriptModule*                     AddModule(const char* szName, const char* szFilePath) override;
	virtual IScriptEnum*                       AddEnum(const char* szName, IScriptElement* pScope) override;
	virtual IScriptStruct*                     AddStruct(const char* szName, IScriptElement* pScope) override;
	virtual IScriptSignal*                     AddSignal(const char* szName, IScriptElement* pScope) override;
	virtual IScriptConstructor*                AddConstructor(const char* szName, IScriptElement* pScope) override;
	virtual IScriptFunction*                   AddFunction(const char* szNddfuncme, IScriptElement* pScope) override;
	virtual IScriptInterface*                  AddInterface(const char* szName, IScriptElement* pScope) override;
	virtual IScriptInterfaceFunction*          AddInterfaceFunction(const char* szName, IScriptElement* pScope) override;
	virtual IScriptInterfaceTask*              AddInterfaceTask(const char* szName, IScriptElement* pScope) override;
	virtual IScriptClass*                      AddClass(const char* szName, const SElementId& baseId, const char* szFilePath) override;
	virtual IScriptBase*                       AddBase(const SElementId& baseId, IScriptElement* pScope) override;
	virtual IScriptStateMachine*               AddStateMachine(const char* szName, EScriptStateMachineLifetime lifetime, const DrxGUID& contextGUID, const DrxGUID& partnerGUID, IScriptElement* pScope) override;
	virtual IScriptState*                      AddState(const char* szName, IScriptElement* pScope) override;
	virtual IScriptVariable*                   AddVariable(const char* szName, const SElementId& typeId, const DrxGUID& baseGUID, IScriptElement* pScope) override;
	virtual IScriptTimer*                      AddTimer(const char* szName, IScriptElement* pScope) override;
	virtual IScriptSignalReceiver*             AddSignalReceiver(const char* szName, EScriptSignalReceiverType type, const DrxGUID& signalGUID, IScriptElement* pScope) override;
	virtual IScriptInterfaceImpl*              AddInterfaceImpl(EDomain domain, const DrxGUID& refGUID, IScriptElement* pScope) override;
	virtual IScriptComponentInstance*          AddComponentInstance(const char* szName, const DrxGUID& typeGUID, IScriptElement* pScope) override;
	virtual IScriptActionInstance*             AddActionInstance(const char* szName, const DrxGUID& actionGUID, const DrxGUID& contextGUID, IScriptElement* pScope) override;

	virtual void                               RemoveElement(const DrxGUID& guid) override;

	virtual IScriptElement&                    GetRootElement() override;
	virtual const IScriptElement&              GetRootElement() const override;
	virtual IScriptElement*                    GetElement(const DrxGUID& guid) override;
	virtual const IScriptElement*              GetElement(const DrxGUID& guid) const override;

	virtual bool                               CopyElementsToXml(XmlNodeRef& output, IScriptElement& scope) const override;
	virtual bool                               PasteElementsFromXml(const XmlNodeRef& input, IScriptElement* pScope) override;

	virtual bool                               SaveUndo(XmlNodeRef& output, IScriptElement& scope) const override;
	virtual IScriptElement*                    RestoreUndo(const XmlNodeRef& input, IScriptElement* pScope) override;

	virtual bool                               IsElementNameUnique(const char* szName, IScriptElement* pScope) const override;
	virtual void                               MakeElementNameUnique(IString& name, IScriptElement* pScope) const override;
	virtual void                               ElementModified(IScriptElement& element) override;

	virtual ScriptRegistryChangeSignal::Slots& GetChangeSignalSlots() override;

	virtual IScript*                           GetScriptByGuid(const DrxGUID& guid) const override;
	virtual IScript*                           GetScriptByFileName(const char* szFilePath) const override;

	virtual IScript*                           LoadScript(const char* szFilePath) override;
	virtual void                               SaveScript(IScript& script) override;

	virtual void                               OnScriptRenamed(IScript& script, const char* szFilePath) override;
	// ~IScriptRegistry

private:
	CScript* CreateScript(const char* szFilePath, const IScriptElementPtr& pRoot);
	CScript* GetScript(const DrxGUID& guid);

	void     ProcessInputBlocks(ScriptInputBlocks& inputBlocks, IScriptElement& scope, EScriptEventId eventId);
	void     AddElement(const IScriptElementPtr& pElement, IScriptElement& scope, const char* szFilePath = nullptr);
	void     RemoveElement(IScriptElement& element);
	void     SaveScript(CScript& script);

protected:
	void BeginChange();
	void EndChange();
	void ProcessChange(const SScriptRegistryChange& change);
	void ProcessChangeDependencies(EScriptRegistryChangeType changeType, const DrxGUID& elementGUID);           // #SchematycTODO : Should we be able to queue dependency changes?

	bool IsUniqueElementName(const char* szName, IScriptElement* pScope) const;

private:

	std::shared_ptr<CScriptRoot> m_pRoot;   // #SchematycTODO : Why can't we use std::unique_ptr?
	ScriptsByGuid                m_scriptsByGuid;
	ScriptsByFileName            m_scriptsByFileName;
	Elements                     m_elements;
	uint32                       m_changeDepth;
	ChangeQueue                  m_changeQueue;
	SSignals                     m_signals;
};
} // Schematyc
