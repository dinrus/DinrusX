// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXMATCHMAKING_H__
#define __DRXMATCHMAKING_H__

#pragma once

<DinrusX/CoreX/Lobby/IDrxMatchMaking.h>
#include "DrxLobby.h"
<DinrusX/DinrusXNetwork/NetAddress.h>
#include "DrxHostMigration.h"

#if USE_DRX_MATCHMAKING

	#define NET_HOST_HINT_DEBUG     0
	#define USE_DEFAULT_BAN_TIMEOUT (0.0f)

///////////////////////////////////////////////////////////////////////////
// When all references to SDrxSessionUserData and EDrxSessionUserDataType have been removed from the engine this can go.
typedef SDrxLobbyUserData     SDrxSessionUserData;
typedef EDrxLobbyUserDataType EDrxSessionUserDataType;
const EDrxSessionUserDataType eCSUDT_Int64 = eCLUDT_Int64;
const EDrxSessionUserDataType eCSUDT_Int32 = eCLUDT_Int32;
const EDrxSessionUserDataType eCSUDT_Int16 = eCLUDT_Int16;
const EDrxSessionUserDataType eCSUDT_Int8 = eCLUDT_Int8;
const EDrxSessionUserDataType eCSUDT_Float64 = eCLUDT_Float64;
const EDrxSessionUserDataType eCSUDT_Float32 = eCLUDT_Float32;
const EDrxSessionUserDataType eCSUDT_Int64NoEndianSwap = eCLUDT_Int64NoEndianSwap;
///////////////////////////////////////////////////////////////////////////

class CDrxLobbyPacket;
class CDrxSharedLobbyPacket;

const uint32 DrxMatchMakingConnectionTimeOut = 10000;
const uint32 DrxMatchMakingHostMigratedConnectionConfirmationWindow = 3000; // 3 second window to receive client confirmations before pruning

	#define MAX_MATCHMAKING_SESSION_USER_DATA 64
	#define MAX_MATCHMAKING_TASKS             8
	#define MAX_MATCHMAKING_PARAMS            7
	#define GAME_RECV_QUEUE_SIZE              16

	#define HOST_MIGRATION_TASK_ID_FLAG       (0x10000000)

	#define INVALID_USER_ID                   (0)

// Dedicated servers allocated with DrxDedicatedServerArbitrator have this as their m_uid in SDrxMatchMakingConnectionUID
	#define DEDICATED_SERVER_CONNECTION_UID 0

struct SDrxLobbySessionHandle
{
};

typedef DrxLobbyID<SDrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> DrxLobbySessionHandle;
	#define DrxLobbyInvalidSessionHandle DrxLobbySessionHandle()

typedef uint32 DrxMatchMakingTaskID;
const DrxMatchMakingTaskID DrxMatchMakingInvalidTaskID = 0xffffffff;

struct SDrxMatchMakingConnectionID
{
};

typedef DrxLobbyID<SDrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> DrxMatchMakingConnectionID;
	#define DrxMatchMakingInvalidConnectionID DrxMatchMakingConnectionID()

struct SDrxSharedSessionID : public SDrxSessionID
{
	void* operator new(size_t size) throw()
	{
		CDrxLobby* pLobby = (CDrxLobby*)CDrxLobby::GetLobby();

		if (pLobby)
		{
			TMemHdl hdl = pLobby->MemAlloc(size);

			if (hdl != TMemInvalidHdl)
			{
				SDrxSharedSessionID* id = (SDrxSharedSessionID*)pLobby->MemGetPtr(hdl);

				id->hdl = hdl;

				return id;
			}
		}

		return NULL;
	}

	void operator delete(void* p)
	{
		if (p)
		{
			CDrxLobby* pLobby = (CDrxLobby*)CDrxLobby::GetLobby();

			if (pLobby)
			{
				pLobby->MemFree(((SDrxSharedSessionID*)p)->hdl);
			}
		}
	}

	TMemHdl hdl;
};

	#if NETWORK_HOST_MIGRATION

#include "Context/INetContextState.h"

		#if ENABLE_HOST_MIGRATION_STATE_CHECK
#define HMSCD_MAX_ENTITY_NAME_LENGTH 16

enum EHostMigrationStateCheckDataType
{
	eHMSCDT_NID
};

struct SHostMigrationStateCheckDataNID
{
	uint32 aspectsEnabled;
	uint16 id;
	uint16 salt;
	char   name[HMSCD_MAX_ENTITY_NAME_LENGTH];
	bool   allocated;
	bool   controlled;
	bool   owned;
	uint8  spawnType;
};

struct SHostMigrationStateCheckData
{
	EHostMigrationStateCheckDataType type;

	union
	{
		SHostMigrationStateCheckDataNID nid;
	};
};
		#endif

struct SHostMigrationInfo_Private : public SHostMigrationInfo
{
	SHostMigrationInfo_Private(void)
		: m_startTime(0.0f)
		, m_logTime(0.0f)
		, m_sessionMigrated(false)
		, m_matchMakingFinished(false)
		, m_newHostAddressValid(false)
		, m_lobbyHMStatus(0)
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
		, m_stateCheckDone(false)
		#endif
	{
	}

	void Reset(void)
	{
		m_sessionMigrated = false;
		m_matchMakingFinished = false;
		m_newHostAddressValid = false;
		m_lobbyHMStatus = 0;
		m_taskID = DrxMatchMakingInvalidTaskID;
	}

	CTimeValue                   m_startTime;
	CTimeValue                   m_logTime;
	TNetAddress                  m_newHostAddress;

	_smart_ptr<INetContextState> m_storedContextState;

	uint32                       m_lobbyHMStatus;

	bool                         m_sessionMigrated;
	bool                         m_matchMakingFinished;
	bool                         m_newHostAddressValid;
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
	bool                         m_stateCheckDone;
		#endif
};

	#endif

struct SDrxSessionInfoRequester
{
	TNetAddress          addr;
	uint32               cookie;
	DrxMatchMakingTaskID mmTaskID;
};

struct SBannedUser
{
	SBannedUser() : m_userID(DrxUserInvalidID), m_userName(""), m_bannedUntil(), m_permanent(false), m_resolving(false), m_duplicate(false) {};

	DrxUserID        m_userID;
	DrxStringT<char> m_userName;
	CTimeValue       m_bannedUntil;
	bool             m_permanent;
	bool             m_resolving;
	bool             m_duplicate;
};

typedef std::deque<SBannedUser> TBannedUser;

class CDrxMatchMaking : public CMultiThreadRefCount, public IDrxMatchMaking
{
public:
	CDrxMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, EDrxLobbyService serviceType);

	EDrxLobbyError         Initialise();

	EDrxLobbyError         Terminate();
	void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	void                   OnError(const TNetAddress& addr, ESocketError error, DrxLobbySendID sendID);
	void                   OnSendComplete(const TNetAddress& addr, DrxLobbySendID sendID);
	virtual void           CancelTask(DrxLobbyTaskID lTaskID);
	virtual void           SessionDisconnectRemoteConnectionFromNub(DrxSessionHandle gh, EDisconnectionCause cause);
	virtual void           SessionJoinFromConsole(void);
	virtual DrxSessionID   GetSessionIDFromConsole(void);
	virtual EDrxLobbyError GetSessionAddressFromSessionID(DrxSessionID sessionID, uint32& hostIP, uint16& hostPort) { return eCLE_ServiceNotSupported; };
	virtual EDrxLobbyError SessionTerminateHostHintingForGroup(DrxSessionHandle gh, SDrxMatchMakingConnectionUID* pConnections, uint32 numConnections, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionSetLocalFlags(DrxSessionHandle gh, uint32 flags, DrxLobbyTaskID* pTaskID, DrxMatchmakingFlagsCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionClearLocalFlags(DrxSessionHandle gh, uint32 flags, DrxLobbyTaskID* pTaskID, DrxMatchmakingFlagsCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionGetLocalFlags(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingFlagsCallback pCB, void* pCBArg);
	virtual bool           IsDead() const { return false; }

	virtual EDrxLobbyError SendToServer(CDrxLobbyPacket* pPacket, DrxSessionHandle gh);
	virtual EDrxLobbyError SendToAllClients(CDrxLobbyPacket* pPacket, DrxSessionHandle gh);
	virtual EDrxLobbyError SendToClient(CDrxLobbyPacket* pPacket, DrxSessionHandle gh, SDrxMatchMakingConnectionUID uid);
	virtual EDrxLobbyError SessionSendRequestInfo(CDrxLobbyPacket* pPacket, DrxSessionID id, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionSendRequestInfoCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionSendRequestInfoResponse(CDrxLobbyPacket* pPacket, DrxSessionRequesterID requester);
	virtual EDrxLobbyError SessionSetupDedicatedServer(DrxSessionHandle gh, CDrxLobbyPacket* pPacket, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionSetupDedicatedServerCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionReleaseDedicatedServer(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);

	// The DrxSessionHandle passed to the game consists of
	// the local session handle used to identify the session in other session calls
	// a uid generated by the host to identify the connection used.
	// This id is passed around when the host receives a connection so any peer can identify a connection from this.
	DrxSessionHandle                     CreateGameSessionHandle(DrxLobbySessionHandle h, SDrxMatchMakingConnectionUID uid);
	SDrxMatchMakingConnectionUID         CreateConnectionUID(DrxLobbySessionHandle h);
	DrxLobbySessionHandle                GetSessionHandleFromGameSessionHandle(DrxSessionHandle gh);
	SDrxMatchMakingConnectionUID         GetConnectionUIDFromGameSessionHandle(DrxSessionHandle gh);
	virtual SDrxMatchMakingConnectionUID GetConnectionUIDFromGameSessionHandleAndChannelID(DrxSessionHandle gh, uint16 channelID);
	virtual uint16                       GetChannelIDFromGameSessionHandle(DrxSessionHandle gh);
	SDrxMatchMakingConnectionUID         GetConnectionUIDFromNubAndGameSessionHandle(DrxSessionHandle gh);

	virtual EDrxLobbyService             GetLobbyServiceTypeForNubSession();
	DrxLobbySessionHandle                GetCurrentNetNubSessionHandle(void) const;
	DrxLobbySessionHandle                GetCurrentHostedNetNubSessionHandle(void) const;
	virtual SDrxMatchMakingConnectionUID GetHostConnectionUID(DrxSessionHandle gh);
	virtual EDrxLobbyError               SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg) = 0;

	virtual void                         TerminateHostMigration(DrxSessionHandle gh);
	virtual eHostMigrationState          GetSessionHostMigrationState(DrxSessionHandle gh);
	virtual EDrxLobbyError               GetSessionPlayerPing(SDrxMatchMakingConnectionUID uid, DrxPing* const pPing);
	virtual EDrxLobbyError               GetSessionIDFromSessionURL(const char* const pSessionURL, DrxSessionID* const pSessionID) { return eCLE_ServiceNotSupported; };
	virtual EDrxLobbyError               GetSessionIDFromIP(const char* const pAddr, DrxSessionID* const pSessionID)               { return eCLE_ServiceNotSupported; };

	virtual uint32                       GetTimeSincePacketFromServerMS(DrxSessionHandle gh);
	virtual void                         DisconnectFromServer(DrxSessionHandle gh);
	virtual void                         StatusCmd(eStatusCmdMode mode);
	virtual void                         KickCmd(uint32 cxID, uint64 userID, const char* pName, EDisconnectionCause cause);
	virtual void                         KickCmd(DrxLobbyConnectionID cxID, uint64 userID, const char* pName, EDisconnectionCause cause);
	virtual void                         Kick(const DrxUserID* pUserID, EDisconnectionCause cause)                                                                                          {};
	virtual void                         BanCmd(uint64 userID, float timeout)                                                                                                               {};
	virtual void                         BanCmd(const char* pUserName, float timeout)                                                                                                       {};
	virtual void                         UnbanCmd(uint64 userID)                                                                                                                            {};
	virtual void                         UnbanCmd(const char* pUserName)                                                                                                                    {};
	virtual void                         BanStatus(void)                                                                                                                                    {};
	virtual void                         Ban(const DrxUserID* pUserID, float timeout)                                                                                                       {};
	virtual EDrxLobbyError               SessionSetAdvertisementData(DrxSessionHandle gh, uint8* pData, uint32 dataSize, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCb, void* pCbArg) { return eCLE_ServiceNotSupported; }
	virtual EDrxLobbyError               SessionGetAdvertisementData(DrxSessionID sessionId, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetAdvertisementDataCallback pCb, void* pCbArg)  { return eCLE_ServiceNotSupported; }
	virtual void                         OnBannedUserListChanged()                                                                                                                          {};

	virtual const char*                  GetNameFromGameSessionHandle(DrxSessionHandle gh)                                                                                                  { return ""; }

	#if RESET_CONNECTED_CONNECTION
	void ResetConnection(DrxLobbyConnectionID id);
	#endif

	#if NETWORK_HOST_MIGRATION
	virtual bool              HostMigrationInitiate(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual void              HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause) = 0;
	virtual void              HostMigrationReset(DrxLobbySessionHandle h);
	virtual void              HostMigrationFinalise(DrxLobbySessionHandle h);
	virtual void              HostMigrationWaitForNewServer(DrxLobbySessionHandle h);
	virtual EDrxLobbyError    HostMigrationServer(SHostMigrationInfo* pInfo) = 0;
	virtual bool              IsSessionMigrated(SHostMigrationInfo* pInfo);
	virtual bool              GetNewHostAddress(char* address, SHostMigrationInfo* pInfo) = 0;
	virtual uint32            GetUpstreamBPS(void);
	virtual bool              AreAnySessionsMigrating(void);
	virtual bool              IsSessionMigrating(DrxSessionHandle gh);
	virtual bool              IsNubSessionMigrating() const;
	bool                      IsNubSessionMigratable() const;
	virtual void              HostMigrationStoreNetContextState(INetContextState* pNetContextState);
	virtual INetContextState* HostMigrationGetNetContextState();
	virtual void              HostMigrationResetNetContextState();
	void                      OnReconnectClient(SHostMigrationInfo& hostMigrationInfo);
	bool                      IsHostMigrationFinished(DrxLobbySessionHandle h);
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
	void                      HostMigrationStateCheck(DrxLobbySessionHandle h);
	virtual void              HostMigrationStateCheckAddDataItem(SHostMigrationStateCheckData* pData);
		#endif

protected:
	void                   MarkHostHintInformationDirty(DrxLobbySessionHandle h)         { m_sessions[h]->localFlags |= DRXSESSION_LOCAL_FLAG_HOST_HINT_INFO_DIRTY; }
	void                   MarkHostHintExternalInformationDirty(DrxLobbySessionHandle h) { m_sessions[h]->localFlags |= DRXSESSION_LOCAL_FLAG_HOST_HINT_EXTERNAL_INFO_DIRTY; }
	void                   SendHostHintInformation(DrxLobbySessionHandle h);
	virtual EDrxLobbyError SendHostHintExternal(DrxLobbySessionHandle h)                 { return eCLE_Success; }
	static void            HostHintingOverrideChanged(ICVar* pCVar);
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
	void                   HostMigrationStateCheckInitialise(DrxLobbySessionHandle h);
	void                   HostMigrationStateCheckNT(DrxLobbySessionHandle h);
	void                   HostMigrationStateCheckLogDataItem(SHostMigrationStateCheckData* pData);
	bool                   HostMigrationStateCheckCompareDataItems(SHostMigrationStateCheckData* pData1, SHostMigrationStateCheckData* pData2);
	bool                   HostMigrationStateCheckWriteDataItemToPacket(CDrxLobbyPacket* pPacket, SHostMigrationStateCheckData* pData);
	bool                   HostMigrationStateCheckReadDataItemFromPacket(CDrxLobbyPacket* pPacket, SHostMigrationStateCheckData* pData);
	void                   ProcessHostMigrationStateCheckData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
		#endif
public:
	#endif

protected:
	enum EMMTask
	{
		eT_SessionTerminateHostHintingForGroup,
		eT_SessionModifyLocalFlags,
		eT_SessionSetupDedicatedServer,
		eT_SessionSetupDedicatedServerNameResolve,
		eT_SessionConnectToServer,
		eT_SessionReleaseDedicatedServer,

		eT_MatchMakingPlatformSpecificTask
	};

	#if NETWORK_HOST_MIGRATION
	struct SHostHintInformation
	{
		SHostHintInformation(void) { Reset(); }
		CTimeValue lastSendInternal;
		CTimeValue lastSendExternal;
		uint32     upstreamBPS;
		DrxPing    ping;
		uint8      natType;
		uint8      numActiveConnections;

		enum eConstants
		{
			// PAYLOAD_SIZE should be the sum of the sizes of the bits of the host hint we send to other peers
			PAYLOAD_SIZE = DrxLobbyPacketUINT32Size /* upstreamBPS */ + DrxLobbyPacketUINT16Size /* ping */ + DrxLobbyPacketUINT8Size /* natType */ + DrxLobbyPacketUINT8Size /* numActiveConnections */
		};

		bool operator==(const SHostHintInformation& other) const;
		bool operator!=(const SHostHintInformation& other) const;

		void Reset(void);
		bool SufficientlyDifferent(const SHostHintInformation& other, CTimeValue currentTime);
	};

		#if ENABLE_HOST_MIGRATION_STATE_CHECK
	typedef std::vector<SHostMigrationStateCheckData> THostMigrationStateCheckDatas;
		#endif
	#endif

	struct  SSession
	{
		virtual ~SSession() {};
		virtual const char* GetLocalUserName(uint32 localUserIndex) const = 0;
		virtual void        Reset() = 0;

		struct SLConnection
		{
	#if ENABLE_HOST_MIGRATION_STATE_CHECK
			THostMigrationStateCheckDatas hostMigrationStateCheckDatas;
	#endif
			SDrxMatchMakingConnectionUID  uid;
			DrxPing                       pingToServer;
			uint8                         numUsers;
			bool                          used;
			// CDrxDedicatedServer::SSession uses the same member names for the non-pointer versions of localConnection and remoteConnection. See CDrxDedicatedServer::CDrxDedicatedServer()
			// cppcheck-suppress duplInheritedMember
		}* localConnection;

	#define CMMRC_FLAG_PLATFORM_DATA_VALID 0x00000001       // Flag set if the platform data in a remote connection is valid.

		struct SRConnection
		{
			bool  TimerStarted() { return timerStarted; }
			void  StartTimer()   { timerStarted = true; timer = g_time; }
			void  StopTimer()    { timerStarted = false; }
			int64 GetTimer()     { return g_time.GetMilliSecondsAsInt64() - timer.GetMilliSecondsAsInt64(); }

			CTimeValue                    timer;
			DrxLobbyConnectionID          connectionID;
			SDrxMatchMakingConnectionUID  uid;
	#if NETWORK_HOST_MIGRATION
			SHostHintInformation          hostHintInfo;
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
			THostMigrationStateCheckDatas hostMigrationStateCheckDatas;
		#endif
	#endif

			struct SData
			{
				TMemHdl data;
				uint32  dataSize;
			};

			MiniQueue<SData, GAME_RECV_QUEUE_SIZE> gameRecvQueue;

			DrxPing                                pingToServer;
			uint32 flags;
			uint8  numUsers;
			bool   used;
			bool   timerStarted;
			bool   gameInformedConnectionEstablished;
		};
		// cppcheck-suppress duplInheritedMember
		DrxLobbyIDArray<SRConnection*, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;

	#if NETWORK_HOST_MIGRATION
		SHostMigrationInfo_Private   hostMigrationInfo;
		SRConnection*                newHostPriority[MAX_LOBBY_CONNECTIONS];
		uint32                       newHostPriorityCount;
		SDrxMatchMakingConnectionUID newHostUID;
		SDrxMatchMakingConnectionUID desiredHostUID;
		SHostHintInformation         hostHintInfo;
		SHostHintInformation         outboundHostHintInfo;
	#endif
		CTimeValue                   pingToServerTimer;
		DrxMatchMakingConnectionID   hostConnectionID;
		DrxMatchMakingConnectionID   serverConnectionID;
		uint32                       createFlags;
		uint32                       localFlags;
	};

	struct  STask
	{
		bool  TimerStarted() { return timerStarted; }
		void  StartTimer()   { timerStarted = true; timer = g_time; }
		int64 GetTimer()     { return g_time.GetMilliSecondsAsInt64() - timer.GetMilliSecondsAsInt64(); }

		CTimeValue            timer;
		DrxLobbyTaskID        lTaskID;
		EDrxLobbyError        error;
		DrxMatchMakingTaskID  returnTaskID;
		uint32                startedTask;
		uint32                subTask;
		DrxLobbySessionHandle session;
		void*                 cb;
		void*                 cbArg;
		DrxLobbySendID        sendID;
		EDrxLobbyError        sendStatus;
		TMemHdl               params[MAX_MATCHMAKING_PARAMS];
		uint32                numParams[MAX_MATCHMAKING_PARAMS];
		bool                  used;
		bool                  running;
		bool                  timerStarted;
		bool                  canceled;
	};

	enum eDisconnectSource
	{
		eDS_Local,    // The disconnect has come from this machine.
		eDS_Remote,   // The disconnect has come from the other end of the connection.
		eDS_FromID    // The disconnect has come from the connection given in fromID.
	};

	void                       StartTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       TickBaseTask(DrxMatchMakingTaskID mmTaskID);
	void                       EndTask(DrxMatchMakingTaskID mmTaskID);
	void                       StopTaskRunning(DrxMatchMakingTaskID mmTaskID);

	void                       Tick(CTimeValue tv);
	EDrxLobbyError             StartTask(uint32 etask, bool startRunning, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg);
	void                       StartSubTask(uint32 etask, DrxMatchMakingTaskID mmTaskID);
	void                       FreeTask(DrxMatchMakingTaskID mmTaskID);
	EDrxLobbyError             CreateTaskParamMem(DrxMatchMakingTaskID mmTaskID, uint32 param, const void* pParamData, size_t paramDataSize);
	EDrxLobbyError             CreateTaskParam(DrxMatchMakingTaskID mmTaskID, uint32 param, const void* paramData, uint32 numParams, size_t paramSize);
	EDrxLobbyError             CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32 createFlags, int numUsers);
	virtual void               FreeSessionHandle(DrxLobbySessionHandle h);
	DrxMatchMakingConnectionID AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, uint32 numUsers);
	virtual void               FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	virtual uint64             GetSIDFromSessionHandle(DrxLobbySessionHandle h) = 0;
	bool                       FindLocalConnectionFromUID(SDrxMatchMakingConnectionUID uid, DrxLobbySessionHandle* h);
	bool                       FindConnectionFromUID(SDrxMatchMakingConnectionUID uid, DrxLobbySessionHandle* pH, DrxMatchMakingConnectionID* pID);
	bool                       FindConnectionFromSessionUID(DrxLobbySessionHandle h, SDrxMatchMakingConnectionUID uid, DrxMatchMakingConnectionID* pID);
	DrxMatchMakingTaskID       FindTaskFromSendID(DrxLobbySendID sendID);
	DrxMatchMakingTaskID       FindTaskFromTaskTaskID(uint32 task, DrxMatchMakingTaskID mmTaskID);
	DrxMatchMakingTaskID       FindTaskFromTaskSessionHandle(uint32 task, DrxLobbySessionHandle h);
	DrxMatchMakingTaskID       FindTaskFromLobbyTaskID(DrxLobbyTaskID lTaskID);
	void                       UpdateTaskError(DrxMatchMakingTaskID mmTaskID, EDrxLobbyError error);
	void                       SessionDisconnectRemoteConnectionViaNub(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id, eDisconnectSource source, DrxMatchMakingConnectionID fromID, EDisconnectionCause cause, const char* reason);
	void                       SessionDisconnectRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id, eDisconnectSource source, DrxMatchMakingConnectionID fromID, EDisconnectionCause cause);
	void                       StartSessionPendingGroupDisconnect(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionPendingGroupDisconnect(DrxMatchMakingTaskID mmTaskID);
	void                       StartSessionModifyLocalFlags(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionModifyLocalFlags(DrxMatchMakingTaskID mmTaskID);
	void                       StartSessionSetupDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionSetupDedicatedServerNameResolve(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionSetupDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionSetupDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessRequestSetupDedicatedServerResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       StartSessionReleaseDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       OnSendCompleteSessionReleaseDedicatedServer(DrxMatchMakingTaskID mmTaskID, DrxLobbySendID sendID);
	void                       TickSessionReleaseDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionReleaseDedicatedServer(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessRequestReleaseDedicatedServerResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       SendServerInfo(DrxLobbySessionHandle h, DrxMatchMakingConnectionID connectionID, DrxMatchMakingTaskID mmTaskID, DrxLobbySendID sendIDs[MAX_LOBBY_CONNECTIONS]);
	void                       ProcessSessionServerInfo(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       SessionConnectToServer(DrxLobbySessionHandle h, const TNetAddress& addr, SDrxMatchMakingConnectionUID uid);
	void                       ProcessSessionRequestJoinServerResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       TickSessionConnectToServer(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessD2CHostMigrationStart(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);

	ESocketError               Send(DrxMatchMakingTaskID mmTaskID, CDrxLobbyPacket* pPacket, DrxLobbySessionHandle h, const TNetAddress& to);
	ESocketError               Send(DrxMatchMakingTaskID mmTaskID, CDrxLobbyPacket* pPacket, DrxLobbySessionHandle h, DrxMatchMakingConnectionID connectionID);
	void                       SendToAll(DrxMatchMakingTaskID mmTaskID, CDrxLobbyPacket* pPacket, DrxLobbySessionHandle h, DrxMatchMakingConnectionID skipID);
	void                       SendToAllClientsNT(TMemHdl mh, uint32 length, DrxLobbySessionHandle h);
	void                       SendToServerNT(TMemHdl mh, uint32 length, DrxLobbySessionHandle h);
	void                       SendToClientNT(TMemHdl mh, uint32 length, DrxLobbySessionHandle h, SDrxMatchMakingConnectionUID uid);
	void                       SendServerPing(DrxLobbySessionHandle h);
	void                       ProcessServerPing(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void                       DispatchUserPacket(TMemHdl mh, uint32 length, DrxSessionHandle gh);
	void                       InitialDispatchUserPackets(DrxLobbySessionHandle h, DrxMatchMakingConnectionID connectionID);
	void                       DispatchSessionUserDataEvent(EDrxLobbySystemEvent event, TMemHdl mh);
	void                       SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, SDrxUserInfoResult* pUserInfo);
	TMemHdl                    PacketToMemoryBlock(CDrxLobbyPacket* pPacket, uint32& length);
	CDrxLobbyPacket*           MemoryBlockToPacket(TMemHdl mh, uint32 length);

	void                       DispatchSessionEvent(DrxSessionHandle h, EDrxLobbySystemEvent event);

	void                       ProcessSessionDeleteRemoteConnection(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	static void                MatchmakingSessionJoinFromConsoleCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, uint32 ip, uint16 port, void* pArg);
	virtual const char*        GetConnectionName(SSession::SRConnection* pConnection, uint32 localUserIndex) const   { return NULL; }
	virtual uint64             GetConnectionUserID(SSession::SRConnection* pConnection, uint32 localUserIndex) const { return INVALID_USER_ID; }
	#ifndef _RELEASE
	void                       DrawDebugText();
	#endif
	#if NETWORK_HOST_MIGRATION
	void         ProcessHostHinting(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	bool         ProcessSingleHostHint(DrxLobbySessionHandle h, uint32 uid, SHostHintInformation& hintToUpdate, SHostHintInformation& incomingHint);
	void         SortNewHostPriorities(DrxLobbySessionHandle handle);
	static int   SortNewHostPriorities_Comparator(const void* pLHS, const void* pRHS);
	virtual int  SortNewHostPriorities_ComparatorHelper(const SHostHintInformation& LHS, const SHostHintInformation& RHS);
	virtual void OnHostMigrationFailedToStart(DrxLobbySessionHandle h) {}
		#if HOST_MIGRATION_SOAK_TEST_BREAK_DETECTION
	void         DetectHostMigrationTaskBreak(DrxLobbySessionHandle h, char* pLocation);
		#endif

	CHostMigration        m_hostMigration;
		#if ENABLE_HOST_MIGRATION_STATE_CHECK
	DrxLobbySessionHandle m_hostMigrationStateCheckSession;
		#endif
	#endif

	bool ExpireBans(TBannedUser& bannedUsers, const CTimeValue& tv);

	#if MATCHMAKING_USES_DEDICATED_SERVER_ARBITRATOR
	TNetAddress m_arbitratorAddr;
	#endif

	TBannedUser       m_resolvedBannedUser;
	TBannedUser       m_unresolvedBannedUser;
	uint32            m_connectionUIDCounter;
	// CDrxDedicatedServer uses the same member names for the non-pointer versions of m_sessions and m_task. See CDrxDedicatedServer::CDrxDedicatedServer()
	// cppcheck-suppress duplInheritedMember
	DrxLobbyIDArray<SSession*, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;
	// cppcheck-suppress duplInheritedMember
	STask*            m_task[MAX_MATCHMAKING_TASKS];
	CDrxLobby*        m_lobby;
	CDrxLobbyService* m_pService;
	EDrxLobbyService  m_serviceType;
};

#endif // USE_DRX_MATCHMAKING
#endif // __DRXMATCHMAKING_H__
