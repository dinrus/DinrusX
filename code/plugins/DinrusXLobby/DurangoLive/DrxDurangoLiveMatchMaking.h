// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "DrxMatchMaking.h"
#include "Platform.h"

// XXX - ATG
class ChatIntegrationLayer;

#if USE_DURANGOLIVE && USE_DRX_MATCHMAKING

	#include "DrxDurangoLiveLobbyPacket.h"

struct SDrxDurangoLiveSessionID : public SDrxSharedSessionID
{
	GUID m_sessionID;

	SDrxDurangoLiveSessionID(GUID sessionId) : m_sessionID(sessionId)
	{
	}

	virtual bool operator==(const SDrxSessionID& other)
	{
		return !!IsEqualGUID(m_sessionID, reinterpret_cast<SDrxDurangoLiveSessionID const*>(&other)->m_sessionID);
	}
	virtual bool operator<(const SDrxSessionID& other)
	{
		return (memcmp(&m_sessionID, &reinterpret_cast<SDrxDurangoLiveSessionID const*>(&other)->m_sessionID, sizeof(m_sessionID)) < 0);
	}
	bool IsFromInvite() const
	{
		return false;
	}

	void AsCStr(char* pOutString, int inBufferSize) const
	{
		pOutString[0] = 0;
	}
};

class CDrxDurangoLiveMatchMaking : public CDrxMatchMaking
{
public:
	CDrxDurangoLiveMatchMaking(CDrxLobby* pLobby, CDrxLobbyService* pService, EDrxLobbyService serviceType);

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();
	void                   Tick(CTimeValue tv);

	void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);

	virtual EDrxLobbyError SetINetworkingUser(Live::State::INetworkingUser* user) override;
	virtual EDrxLobbyError SessionRegisterUserData(SDrxSessionUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionCreate(uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionMigrate(DrxSessionHandle gh, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionUpdate(DrxSessionHandle gh, SDrxSessionUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionUpdateSlots(DrxSessionHandle gh, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionQuery(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionQueryCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionGetUsers(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetUsersCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionStart(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionEnd(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionDelete(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionSearch(uint32 user, SDrxSessionSearchParam* pParam, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionSearchCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionJoin(uint32* pUsers, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionJoinCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionSetLocalUserData(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual DrxSessionID   SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h);

	virtual uint32         GetSessionIDSizeInPacket() const;
	virtual EDrxLobbyError WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const;
	virtual DrxSessionID   ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const;

	virtual EDrxLobbyError SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	#if NETWORK_HOST_MIGRATION
	virtual void           HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual void           HostMigrationFinalise(DrxLobbySessionHandle h);
	virtual EDrxLobbyError HostMigrationServer(SHostMigrationInfo* pInfo);
	virtual bool           GetNewHostAddress(char* pAddress, SHostMigrationInfo* pInfo);
	#endif

	TNetAddress GetHostAddressFromSessionHandle(DrxSessionHandle gh);

	// XXX - ATG
	friend class ChatIntegrationLayer;

private:
	enum ETask
	{
		eT_SessionRegisterUserData = eT_MatchMakingPlatformSpecificTask,
		eT_SessionCreate,
		eT_SessionMigrate,
		eT_SessionUpdate,
		eT_SessionUpdateSlots,
		eT_SessionStart,
		eT_SessionGetUsers,
		eT_SessionEnd,
		eT_SessionDelete,
		eT_SessionSearch,
		eT_SessionJoin,
		eT_SessionJoinCreateAssociation,
		eT_SessionRequestJoin,
		eT_SessionQuery,
		eT_SessionSetLocalUserData,

		// ^^New task IDs *must* go before host migration tasks^^
		eT_SessionMigrateHostStart = HOST_MIGRATION_TASK_ID_FLAG,
		eT_SessionMigrateHostServer,
	};

	struct  SRegisteredUserData
	{
		SDrxLobbyUserData data[MAX_MATCHMAKING_SESSION_USER_DATA];
		uint32            num;
	};

	struct  SSession : public CDrxMatchMaking::SSession
	{
		virtual const char* GetLocalUserName(uint32 localUserIndex) const;
		virtual void        Reset();

		struct SLConnection : public CDrxMatchMaking::SSession::SLConnection
		{
			Live::Xuid xuid;
			char       name[DRXLOBBY_USER_NAME_LENGTH];
			uint8      userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
		};

		struct SRConnection : public CDrxMatchMaking::SSession::SRConnection
		{
			enum ERemoteConnectionState
			{
				eRCS_CreatingAssociation,
				eRCS_CreateAssociationSuccess,
				eRCS_CreateAssociationFailed
			};

			Live::Xuid xuid;
			Microsoft::WRL::ComPtr<ABI::Windows::Xbox::Networking::ISecureDeviceAddress>     pSecureDeviceAddress;
			Microsoft::WRL::ComPtr<ABI::Windows::Xbox::Networking::ISecureDeviceAssociation> pSecureDeviceAssociation;
			ERemoteConnectionState state;
			char                   name[DRXLOBBY_USER_NAME_LENGTH];
			uint8                  userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
		};

		GUID         sessionID;
		SLConnection localConnection;
		DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;
	};

	struct  STask : public CDrxMatchMaking::STask
	{
		enum EAsyncTaskState
		{
			eATS_Pending,
			eATS_Success,
			eATS_Failed
		};

		EAsyncTaskState asyncTaskState;
	};

	DrxLobbySessionHandle      FindSessionFromID(GUID const& sessionID);
	void                       StartAsyncActionTask(DrxMatchMakingTaskID mmTaskID, ABI::Windows::Foundation::IAsyncAction* pAsyncAction);
	void                       StartConcurrencyTaskTask(DrxMatchMakingTaskID mmTaskID, concurrency::task<HRESULT> pTask);
	EDrxLobbyError             StartTask(ETask etask, bool startRunning, DrxMatchMakingTaskID* pMMTaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCB, void* pCBArg);
	void                       StartSubTask(ETask etask, DrxMatchMakingTaskID mmTaskID);
	void                       StartTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       StopTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       EndTask(DrxMatchMakingTaskID mmTaskID);
	EDrxLobbyError             CreateSessionHandle(DrxLobbySessionHandle* pH, bool host, uint32 createFlags, int numUsers);
	DrxMatchMakingConnectionID AddRemoteConnection(DrxLobbySessionHandle h, ABI::Windows::Xbox::Networking::ISecureDeviceAddress* pSecureDeviceAddress, ABI::Windows::Xbox::Networking::ISecureDeviceAssociation* pSecureDeviceAssociation, Live::Xuid const xuid);
	virtual void               FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	concurrency::task<HRESULT> FreeRemoteConnectionAsync(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);

	virtual uint64             GetSIDFromSessionHandle(DrxLobbySessionHandle h);

	void                       StartCreateSecureDeviceAssociation(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void                       CreateSecureDeviceAssociationHandler(ABI::Windows::Xbox::Networking::ISecureDeviceAssociation* pSecureDeviceAssociation, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);

	void                       StartSessionCreate(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionCreate(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionCreate(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionSearch(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionUpdate(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionUpdate(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionStart(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionStart(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionEnd(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionEnd(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionJoinCreateAssociation(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionRequestJoin(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessSessionRequestJoin(const TNetAddress& addr, CDrxDurangoLiveLobbyPacket* pPacket);
	void                       ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxDurangoLiveLobbyPacket* pPacket);

	void                       StartSessionDelete(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionDelete(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionGetUsers(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionQuery(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionQuery(DrxMatchMakingTaskID mmTaskID);

	#if NETWORK_HOST_MIGRATION
	void                HostMigrationServerNT(DrxMatchMakingTaskID mmTaskID);
	void                TickHostMigrationServerNT(DrxMatchMakingTaskID mmTaskID);
	void                HostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	void                TickHostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	#endif
	void                LogConnectionStatus();
	void                SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void                ConnectionEstablishedUserDataEvent(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void                InitialUserDataEvent(DrxLobbySessionHandle h);

	void                ResetTask(DrxMatchMakingTaskID mmTaskID);
	virtual const char* GetConnectionName(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const;
	virtual uint64      GetConnectionUserID(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const;

	SRegisteredUserData m_registeredUserData;
	DrxLobbyIDArray<SSession, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;
	STask               m_task[MAX_MATCHMAKING_TASKS];

	Microsoft::WRL::ComPtr<ABI::Windows::Xbox::Networking::ISecureDeviceAssociationTemplate> m_pSecureDeviceAssociationTemplate;
	Live::State::INetworkingUser* m_user;

	EventRegistrationToken        m_AssociationIncoming;
};

#endif // USE_DURANGOLIVE && USE_DRX_MATCHMAKING
