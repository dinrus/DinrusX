// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Containers/DrxListenerSet.h>
#include <DinrusX/DinrusXEntity/IEntityClass.h>
#include <DinrusX/DinrusXSchema/ScopedConnection.h>

#include <DinrusX/DinrusXNetwork/INetwork.h>

namespace Schematyc
{
struct IRuntimeClass;
};

//////////////////////////////////////////////////////////////////////////
// Описание:
//    Standard implementation of the IEntityClassRegistry interface.
//////////////////////////////////////////////////////////////////////////
class CEntityClassRegistry final
	: public IEntityClassRegistry
	  , public INetworkedClientListener
{
public:
	CEntityClassRegistry();
	virtual ~CEntityClassRegistry() override;

	bool RegisterEntityClass(IEntityClass* pClass);
	bool UnregisterEntityClass(IEntityClass* pClass);

	// IEntityClassRegistry
	IEntityClass* FindClass(const char* sClassName) const override;
	IEntityClass* FindClassByGUID(const DrxGUID& guid) const override;
	IEntityClass* GetDefaultClass() const override;

	IEntityClass* RegisterStdClass(const SEntityClassDesc& entityClassDesc) override;
	virtual bool  UnregisterStdClass(const DrxGUID& guid) override;

	void          UnregisterSchematycEntityClass() override;

	void          RegisterListener(IEntityClassRegistryListener* pListener) override;
	void          UnregisterListener(IEntityClassRegistryListener* pListener) override;

	void          LoadClasses(const char* szFilename, bool bOnlyNewClasses = false) override;
	void          LoadArchetypes(const char* libPath, bool reload);

	//////////////////////////////////////////////////////////////////////////
	// Registry iterator.
	//////////////////////////////////////////////////////////////////////////
	void          IteratorMoveFirst() override;
	IEntityClass* IteratorNext() override;
	int           GetClassCount() const override { return m_mapClassName.size(); };

	void          OnGameFrameworkInitialized();
	void          InitializeDefaultClasses();

	void          GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_pDefaultClass);
		pSizer->AddContainer(m_mapClassName);
		pSizer->AddContainer(m_mapClassGUIDs);
	}
	//~IEntityClassRegistry

	// INetworkedClientListener
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

private:
	void LoadArchetypeDescription(const XmlNodeRef& root);
	void LoadClassDescription(const XmlNodeRef& root, bool bOnlyNewClasses);

	void NotifyListeners(EEntityClassRegistryEvent event, const IEntityClass* pEntityClass);

	void RegisterSchematycEntityClass();
	void OnSchematycClassCompilation(const Schematyc::IRuntimeClass& runtimeClass);

private:

	typedef std::map<string, IEntityClass*> ClassNameMap;
	ClassNameMap                       m_mapClassName;

	std::vector<std::vector<EntityId>> m_channelEntityInstances;

	std::map<DrxGUID, IEntityClass*>   m_mapClassGUIDs;

	IEntityClass*                      m_pDefaultClass;

	ISystem*                           m_pSystem;
	ClassNameMap::iterator             m_currentMapIterator;

	typedef CListenerSet<IEntityClassRegistryListener*> TListenerSet;
	TListenerSet                m_listeners;

	Schematyc::CConnectionScope m_connectionScope;
};
