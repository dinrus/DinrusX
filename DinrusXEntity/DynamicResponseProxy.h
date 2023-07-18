// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef DYNAMICRESPONSESYSTEM_PROXY_H_
#define DYNAMICRESPONSESYSTEM_PROXY_H_

#include <DinrusX/DinrusXDynRespSys/IDynamicResponseSystem.h>

//////////////////////////////////////////////////////////////////////////
// Описание:
//    Implements dynamic response proxy class for entity.
//////////////////////////////////////////////////////////////////////////
class CEntityComponentDynamicResponse final : public IEntityDynamicResponseComponent
{
	DRX_ENTITY_COMPONENT_CLASS_GUID(CEntityComponentDynamicResponse, IEntityDynamicResponseComponent, "CEntityComponentDynamicResponse", "891f8e50-baf8-4e95-b5f9-4f7bc07eb663"_drx_guid);

	CEntityComponentDynamicResponse();
	virtual ~CEntityComponentDynamicResponse();

public:
	//IEntityComponent.h override
	virtual void Initialize() override;

	//////////////////////////////////////////////////////////////////////////
	// IEntityComponent interface implementation.
	//////////////////////////////////////////////////////////////////////////
	virtual void         ProcessEvent(const SEntityEvent& event) override;
	virtual uint64       GetEventMask() const override;
	virtual EEntityProxy GetProxyType() const override { return ENTITY_PROXY_DYNAMICRESPONSE; }
	virtual void         GetMemoryUsage(IDrxSizer* pSizer) const override
	{
		pSizer->AddObject(this, sizeof(*this));
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// IEntityDynamicResponseComponent interface implementation.
	//////////////////////////////////////////////////////////////////////////
	virtual void                      ReInit(const char* szName = nullptr, const char* szGlobalVariableCollectionToUse = nullptr) override;
	virtual DRS::IVariableCollection* GetLocalVariableCollection() const override;
	virtual DRS::IResponseActor*      GetResponseActor() const override;
	//////////////////////////////////////////////////////////////////////////

private:
	DRS::IResponseActor* m_pResponseActor;
};

#endif // DYNAMICRESPONSESYSTEM_PROXY_H_
