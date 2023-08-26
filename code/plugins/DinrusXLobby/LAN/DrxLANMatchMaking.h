// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "DrxMatchMaking.h"
#include "DrxSharedLobbyPacket.h"

#if USE_LAN

	#define MAX_LAN_SEARCH_SERVERS 128
	#define MAX_LAN_SEARCHES       2

class DrxSharedLobbyPacket;

// If the size and/or layout of SDrxLANSessionID changes, its GetSizeInPacket,
// WriteToPacket, and ReadFromPacket must be changed to reflect this. Most
// importantly, these functions must not include size of SDrxLANSessionID's vtab
// pointer, serialize the vtab pointer into a packet, or deserialize the vtab
// pointer out of a packet.

struct SDrxLANSessionID : public SDrxSharedSessionID
{
	DrxLobbySessionHandle m_h;
	uint32                m_ip;
	uint16                m_port;

	SDrxLANSessionID() :
		m_h(DrxLobbyInvalidSessionHandle),
		m_ip(0),
		m_port(0)
	{
	}

	virtual bool operator==(const SDrxSessionID& other)
	{
		return (m_h == ((const SDrxLANSessionID&)other).m_h) &&
		       (m_ip == ((const SDrxLANSessionID&)other).m_ip) &&
		       (m_port == ((const SDrxLANSessionID&)other).m_port);
	};
	virtual bool operator<(const SDrxSessionID& other)
	{
		return ((m_h < ((const SDrxLANSessionID&)other).m_h) ||
		        ((m_h == ((const SDrxLANSessionID&)other).m_h) && (m_ip < ((const SDrxLANSessionID&)other).m_ip)) ||
		        ((m_h == ((const SDrxLANSessionID&)other).m_h) && (m_ip == ((const SDrxLANSessionID&)other).m_ip) && (m_port < ((const SDrxLANSessionID&)other).m_port)));
	};
	SDrxLANSessionID& operator=(const SDrxLANSessionID& other)
	{
		m_h = other.m_h;
		m_ip = other.m_ip;
		m_port = other.m_port;
		return *this;
	}

	static uint32 GetSizeInPacket()
	{
		return DrxLobbyPacketLobbySessionHandleSize + DrxLobbyPacketUINT32Size + DrxLobbyPacketUINT16Size;
	};

	void WriteToPacket(CDrxLobbyPacket* pPacket) const
	{
		CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;

		pSPacket->WriteLobbySessionHandle(m_h);
		pSPacket->WriteUINT32(m_ip);
		pSPacket->WriteUINT16(m_port);
	};

	void ReadFromPacket(CDrxLobbyPacket* pPacket)
	{
		CDrxSharedLobbyPacket* pSPacket = (CDrxSharedLobbyPacket*)pPacket;

		m_h = pSPacket->ReadLobbySessionHandle();
		m_ip = pSPacket->ReadUINT32();
		m_port = pSPacket->ReadUINT16();
	};
	bool IsFromInvite() const
	{
		return true;
	}

	void AsCStr(char* pOutString, int inBufferSize) const
	{
		drx_sprintf(pOutString, inBufferSize, "%d :%d", m_ip, m_port);
	}
};

class CDrxLANMatchMaking : public CDrxMatchMaking
{
public:
	CDrxLANMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, EDrxLobbyService serviceType);

	EDrxLobbyError         Initialise();
	void                   Tick(CTimeValue tv);

	void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);

	virtual EDrxLobbyError SessionRegisterUserData(SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
	#if DRX_PLATFORM_DURANGO
	virtual EDrxLobbyError SetINetworkingUser(Live::State::INetworkingUser* user) override { return eCLE_Success; }
	#endif
	virtual EDrxLobbyError SessionCreate(uint32* users, int numUsers, uint32 flags, SDrxSessionData* data, DrxLobbyTaskID* taskID, DrxMatchmakingSessionCreateCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionMigrate(DrxSessionHandle h, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionUpdate(DrxSessionHandle h, SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionUpdateSlots(DrxSessionHandle h, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionQuery(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionQueryCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionGetUsers(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetUsersCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionStart(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionEnd(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionDelete(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionSearch(uint32 user, SDrxSessionSearchParam* param, DrxLobbyTaskID* taskID, DrxMatchmakingSessionSearchCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionJoin(uint32* users, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* taskID, DrxMatchmakingSessionJoinCallback cb, void* cbArg);
	virtual EDrxLobbyError SessionSetLocalUserData(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual DrxSessionID   SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h);
	virtual DrxSessionID   GetSessionIDFromConsole(void);
	virtual EDrxLobbyError GetSessionIDFromIP(const char* const pAddress, DrxSessionID* const pSessionID);

	virtual uint32         GetSessionIDSizeInPacket() const;
	virtual EDrxLobbyError WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const;
	virtual DrxSessionID   ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const;
	virtual void           Kick(const DrxUserID* pUserID, EDisconnectionCause cause);

	virtual EDrxLobbyError SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	#if NETWORK_HOST_MIGRATION
	virtual void           HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual EDrxLobbyError HostMigrationServer(SHostMigrationInfo* pInfo);
	virtual bool           GetNewHostAddress(char* address, SHostMigrationInfo* pInfo);
	#endif

private:
	enum ETask
	{
		eT_SessionRegisterUserData = eT_MatchMakingPlatformSpecificTask,
		eT_SessionCreate,
		eT_SessionMigrate,
		eT_SessionUpdate,
		eT_SessionUpdateSlots,
		eT_SessionQuery,
		eT_SessionGetUsers,
		eT_SessionStart,
		eT_SessionEnd,
		eT_SessionDelete,
		eT_SessionSearch,
		eT_SessionJoin,
		eT_SessionSetLocalUserData,

		// ^^New task IDs *must* go before host migration tasks^^
		eT_SessionMigrateHostStart = HOST_MIGRATION_TASK_ID_FLAG,
		eT_SessionMigrateHostServer,
		eT_SessionMigrateHostClient,
		eT_SessionMigrateHostFinish
	};

	struct SDrxLANSearchHandle
	{
	};

	typedef DrxLobbyID<SDrxLANSearchHandle, MAX_LAN_SEARCHES> DrxLANSearchHandle;
	#define DrxLANInvalidSearchHandle DrxLANSearchHandle()

	EDrxLobbyError             StartTask(ETask task, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg);
	void                       StartTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       StopTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       EndTask(DrxMatchMakingTaskID mmTaskID);
	EDrxLobbyError             CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32 createFlags, int numUsers);
	DrxMatchMakingConnectionID AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, uint32 ip, uint16 port, uint32 numUsers, const char* name, uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES], bool isDedicated);
	virtual void               FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	virtual uint64             GetSIDFromSessionHandle(DrxLobbySessionHandle h);
	EDrxLobbyError             CreateSessionSearchHandle(DrxLANSearchHandle* h);
	EDrxLobbyError             SetSessionUserData(DrxLobbySessionHandle h, SDrxSessionUserData* data, uint32 numData);
	EDrxLobbyError             SetSessionData(DrxLobbySessionHandle h, SDrxSessionData* data);
	size_t                     CalculateServerDataSize(DrxLobbySessionHandle h);
	size_t                     CalculateServerDataSize() const;
	void                       SendServerData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ServerDataToGame(DrxMatchMakingTaskID mmTaskID, uint32 ip, uint16 port, TMemHdl params, uint32 length);
	void                       ProcessServerData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	DrxLobbySessionHandle      FindSessionFromServerID(DrxLobbySessionHandle h);

	void                       StartSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessSessionRequestJoin(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       ProcessSessionAddRemoteConnections(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);

	void                       StartSessionDelete(DrxMatchMakingTaskID mmTaskID);

	void                       SendSessionQuery(CTimeValue tv, uint32 index, bool broadcast);
	void                       SetLocalUserName(DrxLobbySessionHandle h, uint32 localUserIndex);
	virtual const char*        GetConnectionName(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const;
	void                       StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessLocalUserData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID);

	TNetAddress                GetHostAddressFromSessionHandle(DrxSessionHandle h);

	#if NETWORK_HOST_MIGRATION
	void           HostMigrationServerNT(DrxMatchMakingTaskID mmTaskID);
	EDrxLobbyError HostMigrationClient(DrxLobbySessionHandle h, DrxMatchMakingTaskID hostTaskID);
	void           TickHostMigrationClientNT(DrxMatchMakingTaskID mmTaskID);
	void           ProcessHostMigrationFromServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void           ProcessHostMigrationFromClient(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void           TickHostMigrationFinishNT(DrxMatchMakingTaskID mmTaskID);
	void           HostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	void           TickHostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	void           ProcessHostMigrationStart(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	#endif
	void           SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void           InitialUserDataEvent(DrxLobbySessionHandle h);

	void           ResetTask(DrxMatchMakingTaskID mmTaskID);
	void           DispatchRoomOwnerChangedEvent(TMemHdl mh);

	bool           ParamFilter(uint32 nParams, const SDrxSessionSearchData* pParams, const SDrxSessionSearchResult* pResult);

	virtual uint64 GetConnectionUserID(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const;

	struct  SRegisteredUserData
	{
		SDrxSessionUserData data[MAX_MATCHMAKING_SESSION_USER_DATA];
		uint32              num;
	} m_registeredUserData;

	struct  SSession : public CDrxMatchMaking::SSession
	{
		typedef CDrxMatchMaking::SSession PARENT;

		virtual const char* GetLocalUserName(uint32 localUserIndex) const;
		virtual void        Reset();

		SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA];
		SDrxSessionData     data;
		SDrxLANSessionID    id;
		uint64              sid;
		uint32              numFilledSlots;

		struct SLConnection : public CDrxMatchMaking::SSession::SLConnection
		{
			char  name[DRXLOBBY_USER_NAME_LENGTH];
			uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
		}                     localConnection;

		struct SRConnection : public CDrxMatchMaking::SSession::SRConnection
		{
			char   name[DRXLOBBY_USER_NAME_LENGTH];
			uint8  userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
			uint32 ip;
			uint16 port;
	#if NETWORK_HOST_MIGRATION
			bool   m_migrated;
	#endif
			bool   m_isDedicated;                                                 // Remote connection is a dedicated server
		};

		DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;
	};

	struct  SSearch
	{
		SDrxLANSessionID servers[MAX_LAN_SEARCH_SERVERS];
		uint32           numServers;
		bool             used;
	};

	struct  STask : public CDrxMatchMaking::STask
	{
		DrxLANSearchHandle search;
		uint32             ticks;
	};

	DrxLobbyIDArray<SSearch, DrxLANSearchHandle, MAX_LAN_SEARCHES>             m_search;
	DrxLobbyIDArray<SSession, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;
	STask m_task[MAX_MATCHMAKING_TASKS];
};

#endif // USE_LAN
