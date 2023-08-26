// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxTCPServiceFactory.h
//  Version:     v1.00
//  Created:     29/03/2010 by Paul Mikell.
//  Описание: IDrxDRM interface definition
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXTCPSERVICEFACTORY_H__

#define __DRXTCPSERVICEFACTORY_H__

#pragma once

#include "DrxLobby.h"

#if USE_DRX_TCPSERVICE

	<DinrusX/DinrusXNetwork/DrxSocks.h>
	#include "Socket/SocketError.h"

class CDrxTCPService;
typedef _smart_ptr<CDrxTCPService> CDrxTCPServicePtr;

	#define INVALID_SOCKET_INDEX             -1

	#define DRX_TCP_SERVICE_CONFIG_NAME_SIZE 32
	#define DRX_TCP_SERVICE_SERVER_NAME_SIZE 64
	#define DRX_TCP_SERVICE_SERVER_PATH_SIZE 128

class CDrxTCPServiceFactory
{
public:

	static const uint32 k_defaultMaxSockets = 10;

	typedef DrxFixedStringT<DRX_TCP_SERVICE_CONFIG_NAME_SIZE> TServiceConfigName;
	typedef DrxFixedStringT<DRX_TCP_SERVICE_SERVER_NAME_SIZE> TServerName;
	typedef DrxFixedStringT<DRX_TCP_SERVICE_SERVER_PATH_SIZE> TURLPrefix;

	// Описание:
	//	 Default constructor for CDrxTCPServiceFactory.
	/* ctor */ CDrxTCPServiceFactory();

	// Описание:
	//	 Destructor for CDrxTCPServiceFactory.
	virtual /* dtor */ ~CDrxTCPServiceFactory();

	//////////////////////////////////////////////////////////
	// IDrxTCPServiceFactory implementation
	//////////////////////////////////////////////////////////

	// Описание:
	//	 Initialise the IDrxTCPServiceFactory.
	// Arguments:
	//   pTCPServicesFile - XML file of TCP service configurations
	//	 maxSockets - maximum number of sockets
	// Return:
	//	 Result code.
	virtual EDrxLobbyError Initialise(const char* pTCPServicesFile, uint32 maxSockets);

	// Описание:
	//	 Terminate the IDrxTCPServiceFactory.
	// Return:
	//	 Result code.
	virtual EDrxLobbyError Terminate(
	  bool isDestructing);

	// Описание:
	//	 Get an IDrxTCPService.
	// Arguments:
	//	 serviceConfigName - service config name
	// Return:
	//	 Pointer to IDrxTCPService for given config.
	virtual IDrxTCPServicePtr GetTCPService(const TServiceConfigName& serviceConfigName);

	// Описание:
	//	 Get an IDrxTCPService.
	// Arguments:
	//	 serverName - server name
	//   port - port
	//   urlPrefix - URL prefix
	// Return:
	//	 Pointer to IDrxTCPService for given config.
	virtual IDrxTCPServicePtr GetTCPService(const TServerName& serverName, uint16 port, const TURLPrefix urlPrefix);

	// Описание:
	//	 Tick.
	// Arguments:
	//	 tv - time.
	virtual void Tick(
	  CTimeValue tv);

	// Описание:
	//	 Increment reference count.
	// Return:
	//	 Reference count.
	virtual int AddRef();

	// Описание:
	//	 Decrement reference count.
	// Return:
	//	 Reference count.
	virtual int Release();

	//////////////////////////////////////////////////////////

	// Описание:
	//	 Get number of free sockets.
	// Return:
	//	 Number of free sockets.
	uint32 NumFreeSockets();

	// Описание:
	//	 Connect a socket.
	// Arguments:
	//	 addr - address.
	//	 sockIdx - index of socket.
	// Return:
	//	 Result code.
	virtual ESocketError Connect(
	  DRXSOCKADDR_IN& addr,
	  int32& sockIdx);

	// Описание:
	//	 Send data to a socket.
	// Arguments:
	//	 sockIdx - socket index.
	//	 pData - data,
	//	 dataLen - size of data
	// Return:
	//	 Result code.
	virtual ESocketError Send(
	  int32 sockIdx,
	  const char* pData,
	  size_t dataLen,
	  size_t& sent);

	// Описание:
	//	 Receive data from a socket.
	// Arguments:
	//	 sockIdx - socket index.
	//	 pData - data,
	//	 dataLen - size of data
	//	 endOfStream - has the end of the stream been reached,
	// Return:
	//	 Result code.
	virtual ESocketError Recv(
	  int32 sockIdx,
	  char* pData,
	  size_t& dataLen,
	  bool& endOfStream);

	// Описание:
	//	 Close a connection
	// Arguments:
	//	 sockIdx - index of socket
	// Return:
	//	 Result code.
	virtual ESocketError CloseConnection(
	  int32& sockIdx);

	// Описание:
	//	 Can a service start resolving?
	// Return:
	//	 true if the service can start resolving, otherwise false
	virtual bool CanStartResolving();

protected:

	struct STCPServiceConfig
	{
		TServiceConfigName name;
		TServerName        server;
		TURLPrefix         urlPrefix;
		uint16             port;
	};

	typedef std::vector<CDrxTCPServicePtr, stl::STLGlobalAllocator<CDrxTCPServicePtr>> TServiceVector;

	// Описание:
	//	 Create an IDrxTCPService.
	// Arguments:
	//	 serviceConfigName - service config name
	// Return:
	//	 Pointer to IDrxTCPService for given server & port.
	virtual CDrxTCPServicePtr CreateTCPService(const TServiceConfigName& serviceConfigName);
	// Описание:
	//	 Create an IDrxTCPService.
	// Arguments:
	//	 serverName - server name
	//   port - port
	//   urlPrefix - URL prefix
	// Return:
	//	 Pointer to IDrxTCPService for given server & port.
	virtual CDrxTCPServicePtr CreateTCPService(const TServerName& serverName, uint16 port, const TURLPrefix urlPrefix);

	virtual void              ReserveServiceConfigs(uint32 numServices);
	virtual void              ReadServiceConfig(XmlNodeRef node);

	std::vector<STCPServiceConfig> m_configs;

	// Services.
	TServiceVector m_services;

private:

	// used to work out deltas from last tick
	CTimeValue m_lastTickTimer;

	// Number of open sockets.
	uint32 m_numSockets;

	// Maximum number of sockets.
	uint32 m_maxSockets;

	// Sockets.
	DRXSOCKET* m_pSockets;

	// Services mutex.
	DrxMutex m_servicesMutex;

	// Reference count.
	volatile int m_cnt;
};

typedef _smart_ptr<CDrxTCPServiceFactory> CDrxTCPServiceFactoryPtr;

#endif // USE_DRX_TCPSERVICE
#endif // __DRXTCPSERVICEFACTORY_H__
