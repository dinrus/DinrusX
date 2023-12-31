// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxTCPServiceFactory.cpp
//  Version:     v1.00
//  Created:     29/03/2010 by Paul Mikell.
//  Описание: CDrxTCPServiceFactory member definitions
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DrxTCPServiceFactory.h"
#include "DrxTCPService.h"

#if USE_DRX_TCPSERVICE

	#if DRX_PLATFORM_WINDOWS || DRX_PLATFORM_DURANGO
typedef TIMEVAL DRXTIMEVAL;
	#elif DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID || DRX_PLATFORM_APPLE || DRX_PLATFORM_ORBIS
typedef timeval DRXTIMEVAL;
	#else
		#error Unsupported platform
	#endif

// Описание:
//	 Default constructor for CDrxTCPServiceFactory.
/* ctor */
CDrxTCPServiceFactory::CDrxTCPServiceFactory()
	: m_services(),
	m_maxSockets(0),
	m_numSockets(0),
	m_pSockets(0),
	m_cnt(0)
{
}

// Описание:
//	 Destructor for CDrxTCPServiceFactory.
/* dtor */
CDrxTCPServiceFactory::~CDrxTCPServiceFactory()
{
	Terminate(true);
	SAFE_DELETE_ARRAY(m_pSockets);
}

// Описание:
//	 Initialise the IDrxTCPServiceFactory.
// Arguments:
//   pTCPServicesFile - XML file of TCP service configurations
//	 maxSockets - maximum number of sockets
// Return:
//	 Result code.
EDrxLobbyError CDrxTCPServiceFactory::Initialise(const char* pTCPServicesFile, uint32 maxSockets)
{
	XmlNodeRef root = GetISystem()->LoadXmlFromFile(pTCPServicesFile);

	if ((root != NULL) && (!strcmp(root->getTag(), "TCPServices")))
	{
		uint32 numConfigs = root->getChildCount();

		ReserveServiceConfigs(numConfigs);

		for (uint32 i = 0; i < numConfigs; ++i)
		{
			XmlNodeRef child = root->getChild(i);

			if (!strcmp(child->getTag(), "TCPService"))
			{
				ReadServiceConfig(child);
			}
		}
	}

	m_maxSockets = maxSockets;
	m_numSockets = 0;
	m_pSockets = new DRXSOCKET[m_maxSockets];
	assert(m_pSockets);

	for (uint32 i = 0; i < m_maxSockets; ++i)
	{
		m_pSockets[i] = (DRXSOCKET)DRX_INVALID_SOCKET;
	}

	return eCLE_Success;
}

// Описание:
//	 Initialise the IDrxTCPServiceFactory.
// Return:
//	 Result code.
EDrxLobbyError
CDrxTCPServiceFactory::Terminate(
  bool isDestructing)
{
	m_servicesMutex.Lock();
	{
		for (size_t i = 0; i < m_services.size(); ++i)
		{
			m_services[i]->Terminate(false);
		}

		m_services.clear();
	}
	m_servicesMutex.Unlock();

	return eCLE_Success;
}

IDrxTCPServicePtr CDrxTCPServiceFactory::GetTCPService(const TServiceConfigName& serviceConfigName)
{
	CDrxTCPServicePtr pResult = 0;

	m_servicesMutex.Lock();
	{
		for (size_t i = 0; i < m_services.size(); ++i)
		{
			if (m_services[i]->IsService(serviceConfigName))
			{
				pResult = m_services[i];
				break;
			}
		}

		if (!pResult)
		{
			pResult = CreateTCPService(serviceConfigName);

			if (pResult)
			{
				m_services.push_back(pResult);
			}
		}
	}
	m_servicesMutex.Unlock();

	return pResult.get();
}

IDrxTCPServicePtr CDrxTCPServiceFactory::GetTCPService(const TServerName& serverName, uint16 port, const TURLPrefix urlPrefix)
{
	CDrxTCPServicePtr pResult = 0;

	m_servicesMutex.Lock();
	{
		for (size_t i = 0; i < m_services.size(); ++i)
		{
			if (m_services[i]->IsService(serverName, port, urlPrefix))
			{
				pResult = m_services[i];
				break;
			}
		}

		if (!pResult)
		{
			pResult = CreateTCPService(serverName, port, urlPrefix);

			if (pResult)
			{
				m_services.push_back(pResult);
			}
		}
	}
	m_servicesMutex.Unlock();

	return pResult.get();
}

// Описание:
//	 Tick.
// Arguments:
//	 tv - time.
void
CDrxTCPServiceFactory::Tick(
  CTimeValue tv)
{
	size_t i = 0;
	IDrxTCPServicePtr pCurr = 0;
	CTimeValue delta;

	if (m_lastTickTimer.GetValue() != 0)
	{
		delta = tv - m_lastTickTimer;
	}

	for (;; )
	{
		m_servicesMutex.Lock();
		{
			if (i < m_services.size())
			{
				pCurr = m_services[i];
			}
			else
			{
				pCurr = 0;
			}
		}
		m_servicesMutex.Unlock();

		if (!pCurr)
		{
			break;
		}

		pCurr->Tick(delta);

		++i;
	}

	m_lastTickTimer = tv;
}

// Описание:
//	 Increment reference count.
// Return:
//	 Reference count
int
CDrxTCPServiceFactory::AddRef()
{
	return DrxInterlockedIncrement(&m_cnt);
}

// Описание:
//	 Decrement reference count.
// Return:
//	 Reference count
int
CDrxTCPServiceFactory::Release()
{
	const int nCount = DrxInterlockedDecrement(&m_cnt);

	assert(nCount >= 0);

	if (nCount == 0)
	{
		delete this;
	}
	else
	{
		if (nCount < 0)
		{
			assert(0);
			DrxFatalError("Deleting Reference Counted Object Twice");
		}
	}

	return nCount;
}

// Описание:
//	 Get number of free sockets.
// Return:
//	 Number of free sockets.
uint32
CDrxTCPServiceFactory::NumFreeSockets()
{
	return m_maxSockets - m_numSockets;
}

// Описание:
//	 Connect a socket.
// Arguments:
//	 addr - address.
//	 sockIdx - index of socket.
// Return:
//	 Result code.
ESocketError
CDrxTCPServiceFactory::Connect(
  DRXSOCKADDR_IN& addr,
  int32& sockIdx)
{
	if (!NumFreeSockets())
	{
		return eSE_MiscFatalError;
	}

	for (sockIdx = 0; sockIdx < (int32)m_maxSockets; ++sockIdx)
	{
		if (m_pSockets[sockIdx] == DRX_INVALID_SOCKET)
		{
			break;
		}
	}

	m_pSockets[sockIdx] = DrxSock::socket(AF_INET, SOCK_STREAM, 0);
	if (m_pSockets[sockIdx] < 0)
	{
		m_pSockets[sockIdx] = DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}

	int yes = 1;
	if (DrxSock::setsockopt(m_pSockets[sockIdx], SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int)) < 0)
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}
	#if DRX_PLATFORM_APPLE
	// Disable the activation of SIGPIPE on error and let the socket return
	// EPIPE instead. Apple based operating systems cannot to this with the
	// send method.
	if (DrxSock::setsockopt(m_pSockets[sockIdx], SOL_SOCKET, SO_NOSIGPIPE, (const char*)&yes, sizeof(int)) < 0)
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = (DRXSOCKET)DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}
	#endif
	if (!DrxSock::MakeSocketNonBlocking(m_pSockets[sockIdx]))
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = (DRXSOCKET)DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}

	// TCP_NODELAY required for Win32 because of high latency connection otherwise
	#if DRX_PLATFORM_WINDOWS

	if (DrxSock::setsockopt(m_pSockets[sockIdx], IPPROTO_TCP, TCP_NODELAY, (const char*)&yes, sizeof(int)) < 0)
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = (DRXSOCKET)DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}

	int recvBufferSize = (256 * 1024);
	if (DrxSock::setsockopt(m_pSockets[sockIdx], SOL_SOCKET, SO_RCVBUF, (const char*)&recvBufferSize, sizeof(int)) < 0)
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = (DRXSOCKET)DRX_INVALID_SOCKET;
		return eSE_MiscFatalError;
	}
	#endif

	// Ignore return value of connect for non-blocking sockets.
	DrxSock::connect(m_pSockets[sockIdx], (DRXSOCKADDR*)&addr, sizeof(addr));

	++m_numSockets;
	return eSE_Ok;
}

// Описание:
//	 Send data to a socket.
// Arguments:
//	 sockIdx - socket index.
//	 pData - data,
//	 dataLen - size of data
// Return:
//	 Result code.

ESocketError
CDrxTCPServiceFactory::Send(
  int32 sockIdx,
  const char* pData,
  size_t dataLen,
  size_t& sent)
{
	ESocketError result;

	DRXTIMEVAL timeout;
	fd_set writable;
	fd_set error;
	int selectResult;
	int sendResult;

	assert((sockIdx < (int32)m_maxSockets) && (sockIdx > -1));

	sent = 0;

	if (m_pSockets[sockIdx] != DRX_INVALID_SOCKET)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		FD_ZERO(&writable);
		FD_SET(m_pSockets[sockIdx], &writable);
		FD_ZERO(&error);
		FD_SET(m_pSockets[sockIdx], &error);
		selectResult = DrxSock::select((int)m_pSockets[sockIdx] + 1, 0, &writable, &error, &timeout); // Ian: SOCKET->int is bad, may lose upper 32 bits

		if (FD_ISSET(m_pSockets[sockIdx], &writable))
		{
			sendResult = DrxSock::send(m_pSockets[sockIdx], pData, dataLen, 0);

			if (sendResult < 0)
			{
				result = eSE_MiscFatalError;
			}
			else
			{
				sent = sendResult;
				result = eSE_Ok;
			}
		}
		else if (FD_ISSET(m_pSockets[sockIdx], &error))
		{
			result = eSE_MiscFatalError;
		}
		else
		{
			result = eSE_Ok;
		}
	}
	else
	{
		result = eSE_MiscFatalError;
	}

	return result;
}

// Описание:
//	 Receive data from a socket.
// Arguments:
//	 sockIdx - socket index.
//	 pData - data,
//	 dataLen - size of data
//	 endOfStream - has the end of the stream been reached,
// Return:
//	 Result code.
ESocketError
CDrxTCPServiceFactory::Recv(
  int32 sockIdx,
  char* pData,
  size_t& dataLen,
  bool& endOfStream)
{
	ESocketError result = eSE_Ok;

	DRXTIMEVAL timeout;
	fd_set sockets;

	assert((sockIdx < (int32)m_maxSockets) && (sockIdx > -1));

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	if (m_pSockets[sockIdx] != DRX_INVALID_SOCKET)
	{
		// Read reply data.

		FD_ZERO(&sockets);
		FD_SET(m_pSockets[sockIdx], &sockets);
		int sr = DrxSock::select((int)m_pSockets[sockIdx] + 1, &sockets, 0, 0, &timeout); // SOCKET->int is bad, may lose upper 32 bits

		if (FD_ISSET(m_pSockets[sockIdx], &sockets))
		{
			int r = DrxSock::recv(m_pSockets[sockIdx], pData, dataLen, 0);

			if (r < 0)
			{
				result = eSE_MiscFatalError;
				dataLen = 0;
				endOfStream = true;
			}
			else if (r == 0)
			{
				result = eSE_Ok;
				dataLen = 0;
				endOfStream = true;
			}
			else
			{
				result = eSE_Ok;
				dataLen = r;
				endOfStream = false;
			}
		}
		else
		{
			result = eSE_Ok;
			dataLen = 0;
			endOfStream = false;
		}
	}
	else
	{
		result = eSE_MiscFatalError;
	}

	return result;
}

// Описание:
//	 Close a connection
// Arguments:
//	 sockIdx - index of socket
// Return:
//	 Result code.
ESocketError
CDrxTCPServiceFactory::CloseConnection(
  int32& sockIdx)
{
	assert((sockIdx < (int32)m_maxSockets) && (sockIdx > -1));

	if (m_pSockets[sockIdx] != DRX_INVALID_SOCKET)
	{
		DrxSock::closesocket(m_pSockets[sockIdx]);
		m_pSockets[sockIdx] = (DRXSOCKET)DRX_INVALID_SOCKET;
		sockIdx = INVALID_SOCKET_INDEX;
		--m_numSockets;
	}

	return eSE_Ok;
}

CDrxTCPServicePtr CDrxTCPServiceFactory::CreateTCPService(const TServiceConfigName& serviceConfigName)
{
	CDrxTCPService* pService = NULL;

	for (std::vector<STCPServiceConfig>::iterator it = m_configs.begin(); it != m_configs.end(); ++it)
	{
		if (it->name == serviceConfigName)
		{
			pService = new CDrxTCPService(this);

			if (pService)
			{
				pService->Initialise(serviceConfigName, it->server.c_str(), it->port, it->urlPrefix);
			}

			break;
		}
	}

	return pService;
}

CDrxTCPServicePtr CDrxTCPServiceFactory::CreateTCPService(const TServerName& serverName, uint16 port, const TURLPrefix urlPrefix)
{
	CDrxTCPService* pService = new CDrxTCPService(this);

	if (pService)
	{
		TServiceConfigName serviceConfigName = "";

		for (std::vector<STCPServiceConfig>::iterator it = m_configs.begin(); it != m_configs.end(); ++it)
		{
			if ((it->server == serverName) && (it->port == port) && (it->urlPrefix == urlPrefix))
			{
				serviceConfigName = it->name;

				break;
			}
		}

		pService->Initialise(serviceConfigName, serverName, port, urlPrefix);
	}

	return pService;
}

void CDrxTCPServiceFactory::ReserveServiceConfigs(uint32 numServices)
{
	m_configs.reserve(numServices);
}

void CDrxTCPServiceFactory::ReadServiceConfig(XmlNodeRef node)
{
	m_configs.push_back(STCPServiceConfig());
	m_configs.back().name = node->getAttr("config");
	m_configs.back().server = node->getAttr("server");
	m_configs.back().urlPrefix = node->getAttr("prefix");
	node->getAttr("port", m_configs.back().port);
}

bool CDrxTCPServiceFactory::CanStartResolving()
{
	return true;
}

#endif // USE_DRX_TCPSERVICE
