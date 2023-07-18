// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __CommunicationChannelUpr_h__
#define __CommunicationChannelUpr_h__

#pragma once

#include <DinrusX/DinrusXAI/Communication.h>
#include <DinrusX/DinrusXAI/CommunicationChannel.h>

class CommunicationChannelUpr
{
public:
	bool                               LoadChannel(const XmlNodeRef& channelNode, const CommChannelID& parentID);

	void                               Clear();
	void                               Reset();
	void                               Update(float updateTime);

	CommChannelID                      GetChannelID(const char* name) const;
	const SCommunicationChannelParams& GetChannelParams(const CommChannelID& channelID) const;
	CommunicationChannel::Ptr          GetChannel(const CommChannelID& channelID, EntityId sourceId) const;
	CommunicationChannel::Ptr          GetChannel(const CommChannelID& channelID, EntityId sourceId);

private:
	typedef std::map<CommChannelID, SCommunicationChannelParams> ChannelParams;
	ChannelParams m_params;

	typedef std::map<CommChannelID, CommunicationChannel::Ptr> Channels;
	Channels m_globalChannels;

	typedef std::map<int, Channels> GroupChannels;
	GroupChannels m_groupChannels;

	typedef std::map<EntityId, Channels> PersonalChannels;
	PersonalChannels m_personalChannels;
};

#endif //__CommunicationChannelUpr_h__
