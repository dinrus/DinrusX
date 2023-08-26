// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/StdAfxBaseEnv.h>
#include <DinrusX/DinrusXSchema2/BaseEnv_EntityComponentBase.h>

#include <DinrusX/DinrusXEntity/IEntitySystem.h>

namespace SchematycBaseEnv
{
	CEntityComponentBase::CEntityComponentBase()
		: m_pObject(nullptr)
		, m_pEntity(nullptr)
	{}

	bool CEntityComponentBase::Init(const Schematyc2::SComponentParams& params)
	{
		m_pObject = &params.object;
		m_pEntity = gEnv->pEntitySystem->GetEntity(params.object.GetEntityId().GetValue());
		return m_pEntity != nullptr;
	}

	Schematyc2::IObject& CEntityComponentBase::GetObject() const
	{
		DRX_ASSERT(m_pObject);
		return *m_pObject;
	}

	IEntity& CEntityComponentBase::GetEntity() const
	{
		DRX_ASSERT(m_pEntity);
		return *m_pEntity;
	}

	EntityId CEntityComponentBase::GetEntityId() const
	{
		DRX_ASSERT(m_pEntity);
		return m_pEntity->GetId();
	}
}