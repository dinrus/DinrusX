// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// Описание: Listener for entity container events
// - 10/02/2016 Created by Dario Sancho

#pragma once

struct IEntityContainerListener
{
	enum eEventType
	{
		eET_EntityAdded = 0,
		eET_EntityRemoved,
		eET_ContainerEmpty,
	};

	struct SChangeInfo
	{
		SChangeInfo() : entityId(0), containerId(0), eventType(eET_EntityAdded), containerSize(0) {}
		EntityId   entityId;
		EntityId   containerId;
		eEventType eventType;
		size_t     containerSize;
	};

	virtual ~IEntityContainerListener() {}
	virtual void OnGroupChange(SChangeInfo& info) = 0;
};
