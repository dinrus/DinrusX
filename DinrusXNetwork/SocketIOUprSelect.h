// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SOCKETIOMANAGERSELECT_H__
#define __SOCKETIOMANAGERSELECT_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Config.h>

#if DRX_PLATFORM_WINDOWS || DRX_PLATFORM_DURANGO || DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_ORBIS || DRX_PLATFORM_APPLE
	#define HAS_SOCKETIOMANAGER_SELECT
#endif

#if defined(HAS_SOCKETIOMANAGER_SELECT)

	#if DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE || DRX_PLATFORM_ORBIS
		#define IPROTO_UDPP2P_SAFE 0      // clashes with IPROTO_IP
		#define UDPP2P_VPORT       1
	#endif

	#include <DinrusX/DinrusXNetwork/ISocketIOUpr.h>
	#include <DinrusX/CoreX/Memory/STLPoolAllocator.h>
	#include <DinrusX/DinrusXNetwork/WatchdogTimer.h>
	#include <DinrusX/CoreX/Memory/STLGlobalAllocator.h>
	#include <DinrusX/DinrusXNetwork/DrxSocks.h>

class CSocketIOUprSelect : public CSocketIOUpr
{
public:
	virtual const char* GetName() override { return "Select"; }

	bool                Init();
	CSocketIOUprSelect();
	~CSocketIOUprSelect();

	virtual bool      PollWait(uint32 waitTime) override;
	virtual int       PollWork(bool& performedWork) override;

	virtual SSocketID RegisterSocket(DRXSOCKET sock, int protocol) override;
	virtual void      SetRecvFromTarget(SSocketID sockid, IRecvFromTarget* pTarget) override;
	virtual void      SetConnectTarget(SSocketID sockid, IConnectTarget* pTarget) override;
	virtual void      SetSendToTarget(SSocketID sockid, ISendToTarget* pTarget) override;
	virtual void      SetAcceptTarget(SSocketID sockid, IAcceptTarget* pTarget) override;
	virtual void      SetRecvTarget(SSocketID sockid, IRecvTarget* pTarget) override;
	virtual void      SetSendTarget(SSocketID sockid, ISendTarget* pTarget) override;
	virtual void      RegisterBackoffAddressForSocket(const TNetAddress& addr, SSocketID sockid) override;
	virtual void      UnregisterBackoffAddressForSocket(const TNetAddress& addr, SSocketID sockid) override;
	virtual void      UnregisterSocket(SSocketID sockid) override;

	virtual bool      RequestRecvFrom(SSocketID sockid) override;
	virtual bool      RequestSendTo(SSocketID sockid, const TNetAddress& addr, const uint8* pData, size_t len) override;
	virtual bool      RequestSendVoiceTo(SSocketID sockid, const TNetAddress& addr, const uint8* pData, size_t len) override;

	virtual bool      RequestConnect(SSocketID sockid, const TNetAddress& addr) override;
	virtual bool      RequestAccept(SSocketID sock) override;
	virtual bool      RequestSend(SSocketID sockid, const uint8* pData, size_t len) override;
	virtual bool      RequestRecv(SSocketID sockid) override;

	virtual void      PushUserMessage(int msg) override;

	virtual bool      HasPendingData() override { return (m_pWatchdog && m_pWatchdog->HasStalled()); }

	#if LOCK_NETWORK_FREQUENCY
	virtual void ForceNetworkStart() override { ++m_userMessageFrameID; }
	virtual bool NetworkSleep() override      { return true; }
	#endif

private:
	CWatchdogTimer* m_pWatchdog;

	struct SOutgoingData
	{
		int   nLength;
		uint8 data[MAX_UDP_PACKET_SIZE];
	};
	#if USE_SYSTEM_ALLOCATOR
	typedef std::list<SOutgoingData>                                                                                        TOutgoingDataList;
	#else
	typedef std::list<SOutgoingData, stl::STLPoolAllocator<SOutgoingData, stl::PoolAllocatorSynchronizationSinglethreaded>> TOutgoingDataList;
	#endif
	struct SOutgoingAddressedData : public SOutgoingData
	{
		TNetAddress addr;
	};
	#if USE_SYSTEM_ALLOCATOR
	typedef std::list<SOutgoingAddressedData>                                                                                                 TOutgoingAddressedDataList;
	#else
	typedef std::list<SOutgoingAddressedData, stl::STLPoolAllocator<SOutgoingAddressedData, stl::PoolAllocatorSynchronizationSinglethreaded>> TOutgoingAddressedDataList;
	#endif

	struct SSocketInfo
	{
		SSocketInfo()
		{
			salt = 1;
			isActive = false;
			sock = DRX_INVALID_SOCKET;
			nRecvFrom = nRecv = nListen = 0;
			pRecvFromTarget = NULL;
			pSendToTarget = NULL;
			pConnectTarget = NULL;
			pAcceptTarget = NULL;
			pRecvTarget = NULL;
			pSendTarget = NULL;
		}

		uint16                     salt;
		bool                       isActive;
		DRXSOCKET                  sock;
		int                        nRecvFrom;
		int                        nRecv;
		int                        nListen;
		TOutgoingDataList          outgoing;
		TOutgoingAddressedDataList outgoingAddressed;

		IRecvFromTarget*           pRecvFromTarget;
		ISendToTarget*             pSendToTarget;
		IConnectTarget*            pConnectTarget;
		IAcceptTarget*             pAcceptTarget;
		IRecvTarget*               pRecvTarget;
		ISendTarget*               pSendTarget;

		int32                      protocol;

		bool NeedRead() const  { return nRecv || nRecvFrom || nListen; }
		bool NeedWrite() const { return !outgoing.empty() || !outgoingAddressed.empty(); }
	};
	std::vector<SSocketInfo*, stl::STLGlobalAllocator<SSocketInfo*>> m_socketInfo;

	SSocketInfo* GetSocketInfo(SSocketID id);
	void         WakeUp();

	DRXSOCKADDR_IN m_wakeupAddr;
	DRXSOCKET      m_wakeupSocket;
	DRXSOCKET      m_wakeupSender;

	#if LOCK_NETWORK_FREQUENCY
	volatile uint32 m_userMessageFrameID;
	#endif // LOCK_NETWORK_FREQUENCY
	struct SUserMessage
	{
		int    m_message;
	#if LOCK_NETWORK_FREQUENCY
		uint32 m_frameID;
	#endif // LOCK_NETWORK_FREQUENCY
	};

	fd_set    m_read;
	fd_set    m_write;
	DRXSOCKET m_fdmax;
};

#endif

#endif
