// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	class CEntityActionBase : public Schematyc2::IAction
	{
	public:

		CEntityActionBase();

		bool Init(const Schematyc2::SActionParams& params);
		Schematyc2::IObject& GetObject() const;
		IEntity& GetEntity() const;
		inline EntityId GetEntityId() const;

	private:

		Schematyc2::IObject* m_pObject;
		IEntity*            m_pEntity;
	};
}