// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Containers/VectorMap.h>

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/IObject.h>

namespace Schematyc
{
// Forward declare classes.
class CObject;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CObject)

class CObjectPool
{
private:

	struct SSlot
	{
		ObjectId   objectId;
		CObjectPtr pObject;
	};

	typedef std::vector<SSlot>  Slots;
	typedef std::vector<uint32> FreeSlots;

public:

	bool     CreateObject(const Schematyc::SObjectParams& params, IObject*& pObjectOut);
	IObject* GetObject(ObjectId objectId);
	void     DestroyObject(ObjectId objectId);

	void     SendSignal(ObjectId objectId, const SObjectSignal& signal);
	void     BroadcastSignal(const SObjectSignal& signal);

private:

	bool     AllocateSlots(uint32 slotCount);
	ObjectId CreateObjectId(uint32 slotIdx, uint32 salt) const;
	bool     ExpandObjectId(ObjectId objectId, uint32& slotIdx, uint32& salt) const;
	uint32   IncrementSalt(uint32 salt) const;

private:

	Slots     m_slots;
	FreeSlots m_freeSlots;
};
} // Schematyc
