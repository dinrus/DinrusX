// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <DinrusX/CoreX/Platform/platform.h>
#include <CryString/CryString.h>
#include "IAudioConnection.h"
#include "ACETypes.h"

namespace ACE
{
class IAudioSystemItem
{
public:
	IAudioSystemItem::IAudioSystemItem()
		: m_name("")
		, m_id(ACE_INVALID_ID)
		, m_type(AUDIO_SYSTEM_INVALID_TYPE)
		, m_bPlaceholder(false)
		, m_bLocalised(false)
		, m_parent(nullptr)
		, m_bConnected(false)
	{
	}

	IAudioSystemItem::IAudioSystemItem(const string& name, CID id, ItemType type)
		: m_name(name)
		, m_id(id)
		, m_type(type)
		, m_bPlaceholder(false)
		, m_bLocalised(false)
		, m_parent(nullptr)
		, m_bConnected(false)
	{
	}

	virtual ~IAudioSystemItem() {}

	// unique id for this control
	CID              GetId() const          { return m_id; }
	void             SetId(CID id)          { m_id = id; }

	virtual ItemType GetType() const        { return m_type; }
	void             SetType(ItemType type) { m_type = type; }

	string           GetName() const        { return m_name; }
	void             SetName(const string& name)
	{
		if (name != m_name)
		{
			m_name = name;
		}
	}

	virtual bool      IsPlaceholder() const                { return m_bPlaceholder; }
	void              SetPlaceholder(bool bIsPlaceholder)  { m_bPlaceholder = bIsPlaceholder; }

	virtual bool      IsLocalised() const                  { return m_bLocalised; }
	void              SetLocalised(bool bIsLocalised)      { m_bLocalised = bIsLocalised; }

	virtual bool      IsConnected() const                  { return m_bConnected; }
	void              SetConnected(bool bConnected)        { m_bConnected = bConnected; }

	size_t            ChildCount() const                   { return m_children.size(); }
	void              AddChild(IAudioSystemItem* pChild)   { m_children.push_back(pChild); pChild->SetParent(this); }
	IAudioSystemItem* GetChildAt(uint index) const         { return m_children[index]; }
	void              SetParent(IAudioSystemItem* pParent) { m_parent = pParent; }
	IAudioSystemItem* GetParent() const                    { return m_parent; }

private:
	CID                            m_id;
	ItemType                       m_type;
	string                         m_name;
	bool                           m_bPlaceholder;
	bool                           m_bLocalised;
	bool                           m_bConnected;
	std::vector<IAudioSystemItem*> m_children;
	IAudioSystemItem*              m_parent;
};
}
