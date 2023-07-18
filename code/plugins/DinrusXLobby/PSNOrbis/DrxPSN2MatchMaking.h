// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_MATCHMAKING_H__
#define __DRXPSN2_MATCHMAKING_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		#include "DrxMatchMaking.h"
		#include "DrxPSN2Support.h"
		<DinrusX/np.h>

		#if USE_DRX_MATCHMAKING

			#define MAX_PSN_LOCAL_USERS     (1)
			#define MAX_PSN_ROOM_MEMBERS    (16)  // enough for c2
			#define MAX_ATTRIB_IDS          (10)  // 8 searchable ints + 2 binary packets
			#define MAX_PSN_QUERYABLE_ROOMS (1)   // query API only allows for one room request at present
			#define MAX_PSN_QUERYABLE_ATTRS (10)  // Enough for all defined external room attributes

			#define MAX_WORLDS_TO_SEARCH    (10)
			#define MAX_ROOMS_TO_STORE      (50)  // Originally set to SCE_NP_MATCHING2_RANGE_FILTER_MAX (=20), which is the maximum number of results that
                                            // can be returned by a single PSN search. However, since we now do a search on every world,
                                            // this number could be increased, maybe to 50 to match Xbox 360?
			#define MAX_WAITTIME_FOR_QOS (10000)  // Don't wait more than 10 seconds for qos results from a task

struct SSynchronisedSessionID
{
	SceNpMatching2RoomId   m_roomId;
	SceNpMatching2WorldId  m_worldId;
	uint32                 m_gameType;
	SceNpMatching2ServerId m_serverId;
};

struct SDrxPSNSessionID : public SDrxSharedSessionID
{
	// Theoretically, a session will always now have a m_sessionId, but not always a m_sessionInfo.
	// SessionIDs from invites/join via XMB will have m_sessionId only.
	// SessionIDs from matchmaking SessionCreate/SessionJoin will first have m_sessionInfo, then will get m_sessionId slightly later in the create/join task.
	// (but by the time the session is returned to game code, it should have both).
	// SessionIds returned from SessionQuery or SessionSearch will NOT have a m_sessionId.
	SceNpSessionId         m_sessionId;           // PSN Advertisement session info
	SSynchronisedSessionID m_sessionInfo;         // PSN Matching2 room info
	bool                   m_fromInvite;

	SDrxPSNSessionID()
		: m_fromInvite(false)
	{
		memset(&m_sessionInfo, 0, sizeof(m_sessionInfo));
		memset(&m_sessionId, 0, sizeof(m_sessionId));
	}

	bool IsSessionIdEqual(const SDrxSessionID& other)
	{
		return (memcmp(&m_sessionId, &((const SDrxPSNSessionID&)other).m_sessionId, sizeof(m_sessionId)) == 0);
	}
	bool IsSessionInfoEqual(const SDrxSessionID& other)
	{
		return (memcmp(&m_sessionInfo.m_roomId, &((const SDrxPSNSessionID&)other).m_sessionInfo.m_roomId, sizeof(m_sessionInfo.m_roomId)) == 0);
	}

	virtual bool operator==(const SDrxSessionID& other)
	{
		return (IsSessionIdEqual(other) && IsSessionInfoEqual(other));
	}
	virtual bool operator<(const SDrxSessionID& other)
	{
		return ((strcmp(m_sessionId.data, ((const SDrxPSNSessionID&)other).m_sessionId.data) < 0) ||
		        (m_sessionInfo.m_serverId < ((const SDrxPSNSessionID&)other).m_sessionInfo.m_serverId) ||
		        ((m_sessionInfo.m_serverId == ((const SDrxPSNSessionID&)other).m_sessionInfo.m_serverId) && (m_sessionInfo.m_worldId < ((const SDrxPSNSessionID&)other).m_sessionInfo.m_worldId)) ||
		        ((m_sessionInfo.m_serverId == ((const SDrxPSNSessionID&)other).m_sessionInfo.m_serverId) && (m_sessionInfo.m_worldId == ((const SDrxPSNSessionID&)other).m_sessionInfo.m_worldId) && (m_sessionInfo.m_roomId < ((const SDrxPSNSessionID&)other).m_sessionInfo.m_roomId)));
	}
	virtual bool IsFromInvite() const
	{
		return m_fromInvite;
	}
	SDrxPSNSessionID& operator=(const SDrxPSNSessionID& other)
	{
		m_sessionInfo.m_roomId = other.m_sessionInfo.m_roomId;
		m_sessionInfo.m_worldId = other.m_sessionInfo.m_worldId;
		m_sessionInfo.m_serverId = other.m_sessionInfo.m_serverId;
		m_sessionInfo.m_gameType = other.m_sessionInfo.m_gameType;

		memcpy(&m_sessionId, &other.m_sessionId, sizeof(m_sessionId));

		m_fromInvite = other.m_fromInvite;

		return *this;
	}

	void AsCStr(char* pOutString, int inBufferSize) const
	{
		drx_sprintf(pOutString, inBufferSize, "%d - %d - %" PRIu64 " - %s", m_sessionInfo.m_serverId, m_sessionInfo.m_worldId, m_sessionInfo.m_roomId, m_sessionId.data);
	}
};

class CDrxPSNMatchMaking : public CDrxMatchMaking
{
public:
	CDrxPSNMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, CDrxPSNSupport* pSupport, EDrxLobbyService serviceType);
	virtual ~CDrxPSNMatchMaking();

	EDrxLobbyError         Initialise();
	EDrxLobbyError         Terminate();
	void                   Tick(CTimeValue tv);

	void                   OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);

	virtual EDrxLobbyError SessionRegisterUserData(SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg);
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
	virtual void           SessionDisconnectRemoteConnection(DrxSessionHandle gh, const TNetAddress& addr);
	virtual EDrxLobbyError SessionSetLocalUserData(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual DrxSessionID   SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h);
	virtual EDrxLobbyError SessionSetAdvertisementData(DrxSessionHandle gh, uint8* pData, uint32 dataSize, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCb, void* pCbArg);
	virtual EDrxLobbyError SessionGetAdvertisementData(DrxSessionID sessionId, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetAdvertisementDataCallback pCb, void* pCbArg);

	virtual uint32         GetSessionIDSizeInPacket() const;
	virtual EDrxLobbyError WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const;
	virtual DrxSessionID   ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const;

	virtual EDrxLobbyError SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
			#if NETWORK_HOST_MIGRATION
	virtual void           HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual EDrxLobbyError HostMigrationServer(SHostMigrationInfo* pInfo);
	virtual bool           GetNewHostAddress(char* address, SHostMigrationInfo* pInfo);
	virtual EDrxLobbyError SendHostHintExternal(DrxLobbySessionHandle h);
			#endif

	static void            SupportCallback(EDrxPSNSupportCallbackEvent ecb, SDrxPSNSupportCallbackEventData& data, void* pArg);

	TNetAddress            GetHostAddressFromSessionHandle(DrxSessionHandle h);

	virtual EDrxLobbyError CopySessionIDToBuffer(DrxSessionID sessionId, void* pBuffer, size_t& bufferSize);
	virtual EDrxLobbyError CopySessionIDFromBuffer(const void* pBuffer, size_t bufferSize, DrxSessionID& sessionId);
	virtual void           ForceFromInvite(DrxSessionID sessionId);

private:

	enum ETask
	{
		eT_SessionRegisterUserData = eT_MatchMakingPlatformSpecificTask,
		eT_SessionMigrate,
		eT_SessionUpdate,
		eT_SessionUpdateSlots,
		eST_WaitingForUpdateRoomRequestCallback,

		eT_SessionStart,
		eT_SessionEnd,

		eT_SessionSearch,
		eST_WaitingForSearchRoomWorldInfo,
		eST_SessionSearchRequest,
		eST_WaitingForSearchRoomRequestCallback,
		eST_SessionSearchKickQOS,
		eST_WaitingForSearchQOSRequestCallback,

		eT_SessionCreate,
		eST_WaitingForCreateRoomWorldInfo,
		eST_SessionCreateRequest,
		eST_WaitingForCreateRoomRequestCallback,
		eST_WaitingForCreateRoomSignalingCallback,
		eST_WaitingForCreateRoomWebApiCallback,

		eT_SessionJoin,
		eST_WaitingForJoinRoomRequestCallback,
		eST_WaitingForJoinRoomSignalingCallback,
		eST_WaitingForJoinRoomWebApiCallback,

		eT_SessionDelete,
		eST_WaitingForLeaveRoomRequestCallback,
		eST_WaitingForLeaveRoomWebApiCallback,

		eST_WaitingForGrantRoomOwnerRequestCallback,
		eT_SessionGetUsers,

		eT_SessionQuery,
		eST_WaitingForQueryRoomRequestCallback,

		eT_SessionUserData,
		eST_WaitingForUserDataRequestCallback,

		eT_SessionSendHostHintExternal,
		eST_WaitingForNewHostHintCallback,

		eT_SessionSetAdvertisementData,
		eST_WaitingForUpdateAdvertisementDataWebApiCallback,

		eT_SessionGetAdvertisementData,
		eST_WaitingForGetAdvertisementDataWebApiCallback,

		// ^^New task IDs *must* go before host migration tasks^^
		eT_SessionMigrateHostStart = HOST_MIGRATION_TASK_ID_FLAG,
		eT_SessionMigrateHostServer
	};

	enum EHostHintTaskBehaviour
	{
		eHHTB_NORMAL = 0,
		eHHTB_REQUIRES_RESTART
	};

	struct SMappingToPSNLobbyData
	{
		uint32 integerField : 1;
		uint32 fieldOffset  : 31;                 // either 0-7 (representing integer field), or 0-511 (representing offset into binary chunk)
	};

	struct  SRegisteredUserData
	{
		SDrxSessionUserData    data[MAX_MATCHMAKING_SESSION_USER_DATA];
		SMappingToPSNLobbyData mapping[MAX_MATCHMAKING_SESSION_USER_DATA];
		uint32                 num;
	};

	enum EDrxPSNMemberInfo
	{
		ePSNMI_None       = 0,
		ePSNMI_Me         = BIT(0),
		ePSNMI_Owner      = BIT(1),
		ePSNMI_MeOwner    = ePSNMI_Me | ePSNMI_Owner,
		ePSNMI_Other      = BIT(2),
		ePSNMI_OtherOwner = ePSNMI_Other | ePSNMI_Owner,
	};

	enum EDrxPSNConnectionState
	{
		ePSNCS_None,
		ePSNCS_Pending,
		ePSNCS_Dead,
		ePSNCS_Active
	};

	struct SSearchInfoData
	{
		SDrxSessionSearchResult                   m_results[MAX_ROOMS_TO_STORE];
		SDrxSessionUserData                       m_resultUserData[MAX_ROOMS_TO_STORE][MAX_MATCHMAKING_SESSION_USER_DATA];

		SceNpMatching2IntSearchFilter             m_intFilterExternal[SCE_NP_MATCHING2_ROOM_SEARCHABLE_INT_ATTR_EXTERNAL_NUM];
		SceNpMatching2AttributeId                 m_attrId[MAX_ATTRIB_IDS];
		SceNpMatching2SearchRoomRequest           m_searchRequest;

		SceNpMatching2SignalingGetPingInfoRequest m_QOSRequest[MAX_ROOMS_TO_STORE];
		SceNpMatching2RequestId                   m_reqId[MAX_ROOMS_TO_STORE];

		SceNpMatching2WorldId                     m_worldIds[MAX_WORLDS_TO_SEARCH];
		uint32 m_numWorlds;
		uint32 m_numResults;
		uint32 m_nIndex;
	};

	struct SCreateParamData
	{
		SceNpMatching2CreateJoinRoomRequestA m_createRequest;
		SceNpMatching2BinAttr               m_binAttrExternal[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM];
		SceNpMatching2IntAttr               m_intAttrExternal[SCE_NP_MATCHING2_ROOM_SEARCHABLE_INT_ATTR_EXTERNAL_NUM];
		SceNpMatching2BinAttr               m_binAttrInternal[SCE_NP_MATCHING2_ROOM_BIN_ATTR_INTERNAL_NUM];
		char                                m_binAttrExternalData[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_MAX_SIZE * SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM];
		char                                m_binAttrInternalData[SCE_NP_MATCHING2_ROOM_BIN_ATTR_INTERNAL_MAX_SIZE * SCE_NP_MATCHING2_ROOM_BIN_ATTR_INTERNAL_NUM];
		DrxWebApiJobId                      m_createJobId;
	};

	struct SUpdateParamData
	{
		SceNpMatching2SetRoomDataExternalRequest m_updateRequest;
		SceNpMatching2BinAttr                    m_binAttrExternal[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM];
		SceNpMatching2IntAttr                    m_intAttrExternal[SCE_NP_MATCHING2_ROOM_SEARCHABLE_INT_ATTR_EXTERNAL_NUM];
		char m_binAttrExternalData[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_MAX_SIZE * SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM];
	};

	struct SJoinParamData
	{
		SceNpMatching2JoinRoomRequestA m_joinRequest;
		DrxWebApiJobId                m_joinJobId;
	};

	struct SLeaveParamData
	{
		SceNpMatching2LeaveRoomRequest m_leaveRequest;
		DrxWebApiJobId                 m_leaveJobId;
	};

	struct SQueryParamData
	{
		SceNpMatching2GetRoomDataExternalListRequest m_queryRequest;
		SceNpMatching2RoomId                         m_roomIds[MAX_PSN_QUERYABLE_ROOMS];
		SceNpMatching2AttributeId                    m_attrTable[MAX_PSN_QUERYABLE_ATTRS];
	};

	struct SUserDataParamData
	{
		SceNpMatching2SetRoomMemberDataInternalRequest m_userDataRequest;
		SceNpMatching2BinAttr                          m_binAttrInternal[SCE_NP_MATCHING2_ROOMMEMBER_BIN_ATTR_INTERNAL_NUM];
		char m_binAttrInternalData[SCE_NP_MATCHING2_ROOMMEMBER_BIN_ATTR_INTERNAL_MAX_SIZE * SCE_NP_MATCHING2_ROOMMEMBER_BIN_ATTR_INTERNAL_NUM];
	};

	struct SHostHintParamData
	{
		SceNpMatching2SetRoomDataInternalRequest m_reqParam;
		SceNpMatching2RoomMemberId               m_memberId[MAX_PSN_ROOM_MEMBERS];
	};

	struct SGrantRoomOwnerParamData
	{
		SceNpMatching2GrantRoomOwnerRequest m_reqParam;
	};

	struct SUpdateAdvertisementDataParamData
	{
		DrxWebApiJobId m_jobId;
	};

	struct SGetAdvertisementDataParamData
	{
		DrxWebApiJobId m_jobId;
	};

	struct  SSession : public CDrxMatchMaking::SSession
	{
		typedef CDrxMatchMaking::SSession PARENT;

		virtual const char* GetLocalUserName(uint32 localUserIndex) const;
		virtual void        Reset();

		SceNpMatching2RoomId       m_roomId;      // ordered by size in current sdk (3.0.0)
		SceNpMatching2WorldId      m_worldId;
		SceNpMatching2ServerId     m_serverId;
		SceNpSessionId             m_sessionId;

		uint32                     m_flags;
		uint32                     m_gameType;
		uint32                     m_gameMode;
		uint32                     m_numPublicSlots;
		uint32                     m_numPrivateSlots;

		SceNpMatching2RoomMemberId m_myMemberId;
		SceNpMatching2RoomMemberId m_ownerMemberId;

		bool IsUsed() { return (localFlags & DRXSESSION_LOCAL_FLAG_USED); }

		struct SRoomMember
		{
			SceNpId                    m_npId;
			in_addr                    m_peerAddr;
			in_port_t                  m_peerPort;
			SceNpMatching2RoomMemberId m_memberId;
			EDrxPSNMemberInfo          m_valid;

			CTimeValue                 m_signalingTimer;
			EDrxPSNConnectionState     m_signalingState;

			uint8                      m_userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
			bool                       m_bHostJoinAck;

			bool IsUsed()                        { return (m_valid != ePSNMI_None); }
			bool IsValid(EDrxPSNMemberInfo info) { return ((m_valid & info) == info); }

		} m_members[MAX_PSN_ROOM_MEMBERS];

		struct SLConnection : public CDrxMatchMaking::SSession::SLConnection
		{
			DWORD users[MAX_PSN_LOCAL_USERS];
			BOOL  privateSlot[MAX_PSN_LOCAL_USERS];
		}                     localConnection;

		struct SRConnection : public CDrxMatchMaking::SSession::SRConnection
		{
			SceNpMatching2RoomMemberId memberId;
			bool                       registered;
		};

		DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;

		DrxMatchMakingTaskID remoteConnectionTaskID;

		bool                 remoteConnectionProcessingToDo;
		bool                 started;
		bool                 unregisteredConnectionsKicked;
	};

	struct  STask : public CDrxMatchMaking::STask
	{
		SceNpMatching2RequestId m_reqId;
	};

	static void                DumpMemberInfo(SSession::SRoomMember* pMember);

	virtual uint64             GetSIDFromSessionHandle(DrxLobbySessionHandle h);

	virtual void               FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void                       ClearRoomMember(DrxLobbySessionHandle sessionHandle, uint32 nMember);
	void                       ClearSessionInfo(DrxLobbySessionHandle sessionHandle);
	DrxMatchMakingConnectionID FindConnectionIDFromRoomMemberID(DrxLobbySessionHandle sessionHandle, SceNpMatching2RoomMemberId memberId);
	SSession::SRoomMember*     FindRoomMemberFromSignalingID(DrxLobbySessionHandle sessionHandle, uint32 id);
	SSession::SRoomMember*     FindRoomMemberFromRoomMemberID(DrxLobbySessionHandle sessionHandle, SceNpMatching2RoomMemberId memberId);

	void                       AddInitialRoomMembers(DrxLobbySessionHandle sessionHandle, const SDrxPSNRoomMemberDataInternalList& pMemberList);
	void                       AddRoomMember(DrxLobbySessionHandle sessionHandle, const SDrxPSNRoomMemberDataInternal& memberInfo, EDrxPSNMemberInfo info);
	void                       RemoveRoomMember(DrxLobbySessionHandle sessionHandle, const SDrxPSNRoomMemberDataInternal& memberInfo);
	void                       ChangeRoomOwner(DrxLobbySessionHandle sessionHandle, SceNpMatching2RoomMemberId prevOwner, SceNpMatching2RoomMemberId newOwner);
	void                       UpdateRoomMember(DrxLobbySessionHandle sessionHandle, const SDrxPSNRoomMemberDataInternal& updatedMemberInfo);

	EDrxPSNConnectionState     CheckSignaling(DrxMatchMakingTaskID mmTaskID);
	void                       UpdateSignaling(DrxMatchMakingTaskID mmTaskID);

	void                       DispatchForcedFromRoomEvent(DrxSessionHandle h, SceNpMatching2Event how, SceNpMatching2EventCause why);
	void                       DispatchRoomOwnerChangedEvent(TMemHdl mh);

	void                       AddVoiceUser(SSession::SRoomMember* pMember);
	void                       ClearVoiceUser(SSession::SRoomMember* pMember);
	void                       UpdateVoiceUsers();

	void                       TickSessionWaitForQOS(DrxMatchMakingTaskID mmTaskID);

	void                       ProcessRequestEvent(SDrxPSNSupportCallbackEventData& data);
	void                       ProcessSignalingEvent(SDrxPSNSupportCallbackEventData& data);
	void                       ProcessRoomEvent(SDrxPSNSupportCallbackEventData& data);
	void                       ProcessErrorEvent(SDrxPSNSupportCallbackEventData& data);
	void                       ProcessWebApiEvent(SDrxPSNSupportCallbackEventData& data);

	EDrxLobbyError             StartTask(ETask etask, bool startRunning, uint32 user, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg);
	void                       StartTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       StopTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void                       EndTask(DrxMatchMakingTaskID mmTaskID);

	EDrxLobbyError             CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32* users, int numUsers, uint32 flags);

	void                       TickSessionGetUsers(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionQuery(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionQuery(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionCreate(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionCreateRequest(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionCreateSignaling(DrxMatchMakingTaskID mmTaskID);
	void                       EventWebApiCreateSession(DrxMatchMakingTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);
	void                       EndSessionCreate(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionMigrate(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionSearchRequest(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionQOSSearch(DrxMatchMakingTaskID mmTaskID);
	void                       EndSessionSearch(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void                       ProcessSessionJoinHostAck(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	bool                       CheckSessionJoinHostAck(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionJoinSignaling(DrxMatchMakingTaskID mmTaskID);
	void                       EventWebApiJoinSession(DrxMatchMakingTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);
	void                       EndSessionJoin(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionUpdate(DrxMatchMakingTaskID mmTaskID);

	void                       StartSessionDelete(DrxMatchMakingTaskID mmTaskID);
	void                       TickSessionDelete(DrxMatchMakingTaskID mmTaskID);
	void                       EventWebApiLeaveSession(DrxMatchMakingTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);

	void                       TickSessionUserData(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionSetAdvertisementData(DrxMatchMakingTaskID mmTaskID);
	void                       EventWebApiUpdateSessionAdvertisement(DrxMatchMakingTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);
	void                       EndSessionSetAdvertisementData(DrxMatchMakingTaskID mmTaskID);

	void                       TickSessionGetAdvertisementData(DrxMatchMakingTaskID mmTaskID);
	void                       EventWebApiGetSessionLinkData(DrxMatchMakingTaskID mmTaskID, SDrxPSNSupportCallbackEventData& data);
	void                       EndSessionGetAdvertisementData(DrxMatchMakingTaskID mmTaskID);

			#if NETWORK_HOST_MIGRATION
	void HostMigrationServerNT(DrxMatchMakingTaskID mmTaskID);
	void TickHostMigrationServerNT(DrxMatchMakingTaskID mmTaskID);
	void ProcessHostMigrationFromServer(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void HostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	void TickHostMigrationStartNT(DrxMatchMakingTaskID mmTaskID);
	void ProcessHostMigrationStart(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void StartSessionSendHostHintExternal(DrxMatchMakingTaskID mmTaskID);
	void TickSessionSendHostHintExternal(DrxMatchMakingTaskID mmTaskID);
	void EndSessionSendHostHintExternal(DrxMatchMakingTaskID mmTaskID);
			#endif

	typedef bool (* EventRequestResponseCallback)(CDrxPSNMatchMaking* _this, CDrxPSNMatchMaking::STask* pTask, uint32 taskId, SDrxPSNSupportCallbackEventData& data);

	bool          ProcessCorrectTaskForRequest(SDrxPSNSupportCallbackEventData& data, ETask subTaskCheck, const char* pRequestString, EventRequestResponseCallback pCB);
	bool          ProcessCorrectTaskForSearch(SDrxPSNSupportCallbackEventData& data, ETask subTaskCheck, const char* pRequestString, EventRequestResponseCallback pCB);

	static bool   EventRequestResponse_SetRoomMemberDataInternal(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_GrantRoomOwner(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static uint32 SortWorlds(const SDrxPSNGetWorldInfoListResponse* pWorldList, SceNpMatching2WorldId* pReturnIdList, uint32 nMaxListSize, bool bBusiestFirst);
	static bool   EventRequestResponse_GetWorldInfoListForCreate(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_GetWorldInfoListForSearch(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_LeaveRoom(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_SetRoomDataExternal(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_SetRoomDataInternal(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_GetRoomDataExternalList(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_SearchRoom(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_SearchQOS(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_JoinRoom(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);
	static bool   EventRequestResponse_CreateRoom(CDrxPSNMatchMaking* _this, STask* pTask, DrxMatchMakingTaskID taskId, SDrxPSNSupportCallbackEventData& data);

	void          GetRoomMemberData(DrxLobbySessionHandle sessionHandle, SDrxUserInfoResult& result, SDrxMatchMakingConnectionUID& uid);
	void          ProcessExternalRoomData(const SDrxPSNRoomDataExternal& roomData, SDrxSessionSearchResult& result, SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA]);
	void          BuildUserSessionParams(SSession* pSession, SDrxSessionUserData* pData, uint32 numData,
	                                     SceNpMatching2BinAttr binAttrExternal[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM],
	                                     SceNpMatching2IntAttr intAttrExternal[SCE_NP_MATCHING2_ROOM_SEARCHABLE_INT_ATTR_EXTERNAL_NUM],
	                                     char binAttrExternalData[SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_MAX_SIZE * SCE_NP_MATCHING2_ROOM_BIN_ATTR_EXTERNAL_NUM]);
	uint32 BuildSearchFilterParams(SDrxSessionSearchData* pData, uint32 numData,
	                               SceNpMatching2IntSearchFilter intFilterExternal[SCE_NP_MATCHING2_ROOM_SEARCHABLE_INT_ATTR_EXTERNAL_NUM]);

	void                SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	void                InitialUserDataEvent(DrxLobbySessionHandle h);

	void                ResetTask(DrxMatchMakingTaskID mmTaskID);

	virtual const char* GetConnectionName(CDrxMatchMaking::SSession::SRConnection* pConnection, uint32 localUserIndex) const;

	SRegisteredUserData m_registeredUserData;

	DrxLobbyIDArray<SSession, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;

	STask           m_task[MAX_MATCHMAKING_TASKS];

	uint32          m_oldNatType;
	CDrxPSNSupport* m_pPSNSupport;
	int             net_enable_psn_hinting;
};

		#endif // USE_DRX_MATCHMAKING
	#endif   // USE_PSN
#endif     // DRX_PLATFORM_ORBIS

#endif // __DRXPSNMATCHMAKING_H__
