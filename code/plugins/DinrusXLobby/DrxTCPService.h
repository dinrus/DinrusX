// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxTCPService.h
//  Version:     v1.00
//  Created:     29/03/2010 by Paul Mikell.
//  Описание: CDrxTCPService class definition
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXTCPSERVICE_H__
#define __DRXTCPSERVICE_H__

#include "DrxLobby.h"
#include "DrxTCPServiceFactory.h"
#include "Socket/NetResolver.h"

#if USE_DRX_TCPSERVICE

class CDrxTCPService : public IDrxTCPService
{
public:
	CDrxTCPService(CDrxTCPServiceFactory* pOwner);
	virtual ~CDrxTCPService();

	//////////////////////////////////////////////////////////
	// IDrxTCPService implementation
	//////////////////////////////////////////////////////////

	// Описание:
	//	 Initialize.
	// Arguments:
	//   serviceConfigName - config name
	//	 serverName - server name
	//	 port - port
	//   urlPrefix - URL prefix
	// Return:
	//	 Result code.
	EDrxTCPServiceResult Initialise(const CDrxTCPServiceFactory::TServiceConfigName& serviceConfigName, const CDrxTCPServiceFactory::TServerName& serverName, uint16 port, const CDrxTCPServiceFactory::TURLPrefix& urlPrefix);

	// Описание:
	//	 Terminate.
	// Arguments:
	//	 isDestructing - is this object's destructor running?
	// Return:
	//	 Result code.
	virtual EDrxTCPServiceResult Terminate(
	  bool isDestructing);

	// Описание:
	//	 Tick.
	// Arguments:
	//	 delta - time delta from last tick
	virtual void Tick(CTimeValue dt);

	// Описание:
	//	 Is this a match for the given service?
	// Arguments:
	//	 serviceConfigName - service config name
	// Return:
	//	 true if it's a match, otherwise false.
	virtual bool IsService(const CDrxTCPServiceFactory::TServiceConfigName& serviceConfigName);

	// Описание:
	//	 Is this a match for the given service?
	// Arguments:
	//	 serverName - server name
	//   port - port
	//   urlPrefix - URL prefix
	// Return:
	//	 true if it's a match, otherwise false.
	virtual bool IsService(const CDrxTCPServiceFactory::TServerName& serverName, uint16 port, const CDrxTCPServiceFactory::TURLPrefix& urlPrefix);

	// Описание:
	//	 Has the address resolved?
	// Return:
	//	 true if the address has resolved, otherwise false.
	virtual bool HasResolved();

	// Описание:
	//	 Is the address resolving?
	// Return:
	//	 true if the address is resolving, otherwise false.
	virtual bool IsResolving();

	// UploadData
	// Queues data for uploading to remote server. Note data will be deleted after upload completes/fails.
	// return			- True if successfully queued, false indicates internal failure (queue full maybe)
	virtual bool UploadData(
	  STCPServiceDataPtr pData);

	// Описание:
	//	 Increment reference count.
	// Return:
	//	 Reference count.
	virtual int AddRef();

	// Описание:
	//	 Decrement reference count.
	// Return:
	//	 Reference count.
	virtual int                            Release();

	virtual uint32                         GetDataQueueLength();
	virtual uint32                         GetDataQueueSize();
	virtual EDrxTCPServiceConnectionStatus GetConnectionStatus();
	virtual const char*                    GetServerName();
	virtual uint16                         GetPort();
	virtual const char*                    GetURLPrefix();

	//////////////////////////////////////////////////////////

	virtual void DiscardQueue();

protected:

	// Описание:
	//	 Get address.
	// Arguments:
	//	 addr - address
	// Return:
	//	 Result code.
	virtual ENameRequestResult GetAddress(
	  DRXSOCKADDR_IN& addr);

	// Описание:
	//	 Connect socket.
	// Arguments:
	//	 sockIdx- socket index
	// Return:
	//	 Result code.
	virtual ESocketError Connect(
	  int32& sockIdx);

	// Описание:
	//	 Send data.
	// Arguments:
	//	 pData - transaction
	// Return:
	//	 Result code.
	virtual ESocketError Send(
	  STCPServiceDataPtr pData);

	// Описание:
	//	 Receive data.
	// Arguments:
	//	 pData - transaction
	//	 endOfStream - has end of stream been reached
	// Return:
	//	 Result code.
	virtual ESocketError Recv(
	  STCPServiceDataPtr pData,
	  bool& endOfStream);

	// Описание:
	//	 Close connection
	// Arguments:
	//	 sockIdx - socket index
	// Return:
	//	 Result code.
	virtual ESocketError CloseConnection(
	  int32& sockIdx);

private:

	// Описание:
	//	 Discard a transaction.
	void DiscardQueueElement();

	// Описание:
	//	 Process a transaction.
	// Return:
	//	 The transaction if we're not finished with it, or NULL if we are.
	STCPServiceDataPtr ProcessTransaction(
	  STCPServiceDataPtr pCurr);

protected:

	enum EResolvingState
	{
		eRS_NotStarted,
		eRS_InProgress,
		eRS_Succeeded,
		eRS_Failed,
		eRS_WaitRetry
	};

	virtual void StartResolve();
	virtual void WaitResolve();
	virtual void RetryResolve();

	EResolvingState m_resolveState;
	CTimeValue      m_resolveLastFailed;

	// Transaction queue.
	std::queue<STCPServiceDataPtr> m_dataQueue;

	// Mutex.
	DrxMutex                                  m_queueMutex;

	EDrxTCPServiceConnectionStatus            m_connectionStatus;

	CDrxTCPServiceFactory::TServiceConfigName m_serviceConfigName;
	CDrxTCPServiceFactory::TServerName        m_serverName;
	CDrxTCPServiceFactory::TURLPrefix         m_urlPrefix;

	DRXINADDR                                 m_addr;
	// Port.
	uint16 m_port;

	uint32 m_dataQueueLength;

private:

	// Number of open sockets.
	uint32 m_numSocketsOpen;

	// Number of sockets we could free this tick.
	uint32 m_numSocketsToFree;

	// Owner.
	_smart_ptr<CDrxTCPServiceFactory> m_pOwner;

	// Current DNS request.
	CNameRequestPtr m_pNameReq;

	// Reference count.
	volatile int m_cnt;

	uint32       m_dataQueueSize;
};

#endif // USE_DRX_TCPSERVICE
#endif // __DRXTCPSERVICE_H__
