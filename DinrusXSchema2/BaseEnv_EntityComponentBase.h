// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	class CEntityComponentBase
	{
	public:

		CEntityComponentBase();

		bool Init(const Schematyc2::SComponentParams& params);
		Schematyc2::IObject& GetObject() const;
		IEntity& GetEntity() const;
		EntityId GetEntityId() const;

	private:

		Schematyc2::IObject* m_pObject;
		IEntity*            m_pEntity;
	};
}