// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfxBaseEnv.h>
#include <DinrusX/DinrusXSchema2/BaseEnv_EntityActionBase.h>

#include <DinrusX/DinrusXEntity/IEntitySystem.h>

namespace SchematycBaseEnv
{
	CEntityActionBase::CEntityActionBase()
		: m_pObject(nullptr)
		, m_pEntity(nullptr)
	{}

	bool CEntityActionBase::Init(const Schematyc2::SActionParams& params)
	{
		m_pObject = &params.object;
		m_pEntity = gEnv->pEntitySystem->GetEntity(params.object.GetEntityId().GetValue());
		return m_pEntity != nullptr;
	}

	Schematyc2::IObject& CEntityActionBase::GetObject() const
	{
		DRX_ASSERT(m_pObject);
		return *m_pObject;
	}

	inline IEntity& CEntityActionBase::GetEntity() const
	{
		DRX_ASSERT(m_pEntity);
		return *m_pEntity;
	}

	inline EntityId CEntityActionBase::GetEntityId() const
	{
		DRX_ASSERT(m_pEntity);
		return m_pEntity->GetId();
	}
}