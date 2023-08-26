// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXDEDICATEDSERVER_H__
#define __DRXDEDICATEDSERVER_H__

#pragma once

#include "DrxLobby.h"
#include "DrxMatchMaking.h"

#define DEDICATED_SERVER_IS_FREE_SEND_INTERVAL 30000

#if USE_DRX_DEDICATED_SERVER

class CDrxSharedLobbyPacket;

class CDrxDedicatedServer : public CDrxMatchMaking
{
public:
	CDrxDedicatedServer(CDrxLobby* pLobby, CDrxLobbyService* pService, EDrxLobbyService serviceType);

	virtual EDrxLobbyError SessionRegisterUserData(SDrxLobbyUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)                                                       { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionCreate(uint32* users, int numUsers, uint32 flags, SDrxSessionData* data, DrxLobbyTaskID* taskID, DrxMatchmakingSessionCreateCallback cb, void* cbArg)                           { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionMigrate(DrxSessionHandle h, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg) { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionUpdate(DrxSessionHandle h, SDrxLobbyUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)                                             { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionUpdateSlots(DrxSessionHandle h, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)                                         { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionQuery(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingSessionQueryCallback cb, void* cbArg)                                                                           { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionGetUsers(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingSessionGetUsersCallback cb, void* cbArg)                                                                     { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionEnd(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)                                                                                         { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionStart(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)                                                                                       { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionSetLocalUserData(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg)                             { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionDelete(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)                                                                                      { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionSearch(uint32 user, SDrxSessionSearchParam* param, DrxLobbyTaskID* taskID, DrxMatchmakingSessionSearchCallback cb, void* cbArg)                                                 { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionJoin(uint32* users, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* taskID, DrxMatchmakingSessionJoinCallback cb, void* cbArg)                                     { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)                                                                          { return eCLE_ServiceNotSupported; }
	virtual DrxSessionID   SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h)                                                                                                                         { return DrxSessionInvalidID; }
	virtual uint32         GetSessionIDSizeInPacket() const                                                                                                                                                       { return 0; }
	virtual EDrxLobbyError WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const                                                                                                         { return eCLE_Success; }
	virtual DrxSessionID   ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const                                                                                                                                { return DrxSessionInvalidID; }
	virtual TNetAddress    GetHostAddressFromSessionHandle(DrxSessionHandle h);

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();
	void                   Tick(CTimeValue tv);
	void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);

	#if NETWORK_HOST_MIGRATION
	virtual void           HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual void           HostMigrationFinalise(DrxLobbySessionHandle h)              {}
	virtual EDrxLobbyError HostMigrationServer(SHostMigrationInfo* pInfo)              { return eCLE_ServiceNotSupported; }
	virtual bool           GetNewHostAddress(char* address, SHostMigrationInfo* pInfo) { return false; }
	#endif

protected:
	virtual uint64             GetSIDFromSessionHandle(DrxLobbySessionHandle h);
	DrxMatchMakingConnectionID AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, uint32 numUsers);
	void                       StartArbitratorNameLookup();
	void                       TickArbitratorNameLookup(CTimeValue tv);
	void                       TickFree(CTimeValue tv);
	void                       TickAllocated(CTimeValue tv);
	void                       DispatchUserPacket(TMemHdl mh, uint32 length);
	void                       ProcessDedicatedServerIsFreeAck(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ProcessAllocateDedicatedServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       DispatchFreeDedicatedServer();
	void                       FreeDedicatedServer();
	void                       ProcessFreeDedicatedServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ProcessSessionRequestJoinServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ProcessUpdateSessionID(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);

	struct  SSession : public CDrxMatchMaking::SSession
	{
		typedef CDrxMatchMaking::SSession PARENT;
		virtual const char* GetLocalUserName(uint32 localUserIndex) const { return NULL; }
		virtual void        Reset();

		struct SLConnection : public CDrxMatchMaking::SSession::SLConnection
		{
		};

		struct SRConnection : public CDrxMatchMaking::SSession::SRConnection
		{
		};

		uint64       sid;
		TNetAddress  allocToAddr;
		CTimeValue   checkFreeStartTime;
		uint32       cookie;
		SLConnection localConnection;
		DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;
	};

	struct  STask : public CDrxMatchMaking::STask
	{
	};

	DrxLobbyIDArray<SSession, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;
	STask                 m_task[MAX_MATCHMAKING_TASKS];

	CTimeValue            m_arbitratorLastRecvTime;
	CTimeValue            m_arbitratorNextSendTime;

	DrxLobbySessionHandle m_allocatedSession;
	CNameRequestPtr       m_pNameReq;

	CDrxLobby*            m_pLobby;
};

#endif // USE_DRX_DEDICATED_SERVER
#endif // __DRXDEDICATEDSERVER_H__
