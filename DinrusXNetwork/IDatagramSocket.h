// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Containers/DrxListenerSet.h>
#include <DinrusX/DinrusXNetwork/NetAddress.h>
#include <DinrusX/DinrusXSys/TimeValue.h>

#include <DinrusX/DinrusXNetwork/SocketError.h>
#include <DinrusX/DinrusXNetwork/DrxSocks.h>

// We need a listener for DrxLobby and NetNub
#define DEFAULT_NUM_DATAGRAM_LISTENERS 2

enum ESocketFlags
{
	eSF_BroadcastSend    = 0x00000001u,
	eSF_BroadcastReceive = 0x00000002u,
	eSF_BigBuffer        = 0x00000004u,
	eSF_Online           = 0x00000008u,
	eSF_StrictAddress    = 0x00000010u
};

struct IDatagramListener
{
	virtual ~IDatagramListener(){}
	virtual void OnPacket(const TNetAddress& addr, const uint8* pData, uint32 nLength) = 0;
	virtual void OnError(const TNetAddress& addr, ESocketError error) = 0;
};

struct IDatagramSocket : public CMultiThreadRefCount
{
	IDatagramSocket()
	{
		++g_objcnt.abstractSocket;
	}
	virtual ~IDatagramSocket()
	{
		--g_objcnt.abstractSocket;
	}

	virtual void         RegisterListener(IDatagramListener* pListener) = 0;
	virtual void         UnregisterListener(IDatagramListener* pListener) = 0;
	virtual void         GetSocketAddresses(TNetAddressVec& addrs) = 0;
	virtual ESocketError Send(const uint8* pBuffer, size_t nLength, const TNetAddress& to) = 0;
	virtual ESocketError SendVoice(const uint8* pBuffer, size_t nLength, const TNetAddress& to) = 0;
	virtual void         Die() = 0;
	virtual bool         IsDead() = 0;
	virtual void         RegisterBackoffAddress(const TNetAddress& addr) = 0;
	virtual void         UnregisterBackoffAddress(const TNetAddress& addr) = 0;
	virtual DRXSOCKET    GetSysSocket()                         { return DRX_INVALID_SOCKET; }

	virtual void         GetMemoryStatistics(IDrxSizer* pSizer) { /* do nothing */ }
};

TYPEDEF_AUTOPTR(IDatagramSocket);
typedef IDatagramSocket_AutoPtr IDatagramSocketPtr;

class CDatagramSocket : public IDatagramSocket
{
public:
	CDatagramSocket() : m_listeners(DEFAULT_NUM_DATAGRAM_LISTENERS) {}

	virtual void RegisterListener(IDatagramListener* pListener);
	virtual void UnregisterListener(IDatagramListener* pListener);

protected:
	void OnPacket(const TNetAddress& addr, const uint8* pData, uint32 length);
	void OnError(const TNetAddress& addr, ESocketError error);

	typedef CListenerSet<IDatagramListener*> TListeners;

	TListeners m_listeners;
};

extern IDatagramSocketPtr OpenSocket(const TNetAddress& addr, uint32 flags);
