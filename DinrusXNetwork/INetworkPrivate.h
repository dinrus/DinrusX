// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __INETWORKPRIVATE_H__
#define __INETWORKPRIVATE_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Config.h>
#include <DinrusX/DinrusXNetwork/INetwork.h>
#include <DinrusX/DinrusXNetwork/INetworkMember.h>
#include <DinrusX/DinrusXNetwork/objcnt.h>
#include <DinrusX/DinrusXNetwork/NetCVars.h>
#include <DinrusX/DinrusXNetwork/ISocketIOUpr.h>
#include <DinrusX/DinrusXNetwork/NetResolver.h>

struct INetworkPrivate : public INetwork
{
public:
	virtual SObjectCounters&     GetObjectCounters() = 0;
	virtual ISocketIOUpr&    GetExternalSocketIOUpr() = 0;
	virtual ISocketIOUpr&    GetInternalSocketIOUpr() = 0;
	virtual CNetAddressResolver* GetResolver() = 0;
	virtual const CNetCVars&     GetCVars() = 0;
	virtual bool                 ConvertAddr(const TNetAddress& addrIn, DRXSOCKADDR_IN* pSockAddr) = 0;
	virtual bool                 ConvertAddr(const TNetAddress& addrIn, DRXSOCKADDR* pSockAddr, int* addrLen) = 0;
	virtual uint8                FrameIDToHeader(uint8 id) = 0;
	virtual void BroadcastNetDump(ENetDumpType) = 0;
};

struct INetNubPrivate : public INetNub
{
public:
	virtual void         DisconnectChannel(EDisconnectionCause cause, DrxSessionHandle session, const char* reason) = 0;
	virtual INetChannel* GetChannelFromSessionHandle(DrxSessionHandle session) = 0;
};

#endif // __INETWORKPRIVATE_H__
