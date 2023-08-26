// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

namespace SchematycBaseEnv
{
	typedef TemplateUtils::CDelegate<void (const EntityId)> EntityMapVisitor;

	class CEntityMap
	{
	public:

		void MarkEntityForDestruction(EntityId entityId);
		void DestroyMarkedEntites();
		void AddObject(EntityId entityId, const Schematyc2::ObjectId& objectId);
		void RemoveObject(EntityId entityId);
		Schematyc2::ObjectId FindObjectId(EntityId entityId) const;
		Schematyc2::IObject* FindObject(EntityId entityId) const;
		void VisitEntities(const EntityMapVisitor& visitor);

	private:

		typedef std::vector<EntityId>                             EntityIds;
		typedef std::unordered_map<EntityId, Schematyc2::ObjectId> EntityObjectIds;

		EntityIds       m_entitiesMarkedForDestruction;
		EntityObjectIds m_entityObjectIds;
	};
}