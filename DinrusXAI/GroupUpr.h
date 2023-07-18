// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __GroupUpr_h__
#define __GroupUpr_h__

#include <DinrusX/DinrusXAI/Group.h>

class CGroupUpr
{
public:
	CGroupUpr();
	~CGroupUpr();

	void                  Reset(EObjectResetType type);
	void                  Update(float updateTime);

	void                  AddGroupMember(const GroupID& groupID, tAIObjectID objectID);
	void                  RemoveGroupMember(const GroupID& groupID, tAIObjectID objectID);

	Group&                GetGroup(const GroupID& groupID);
	const Group&          GetGroup(const GroupID& groupID) const;

	uint32                GetGroupMemberCount(const GroupID& groupID) const;

	Group::NotificationID NotifyGroup(const GroupID& groupID, tAIObjectID senderID, const char* name);

	void                  Serialize(TSerialize ser);

private:
	typedef std::unordered_map<GroupID, Group, stl::hash_uint32> Groups;
	Groups                m_groups;

	Group::NotificationID m_notifications;
};

#endif //__GroupUpr_h__
