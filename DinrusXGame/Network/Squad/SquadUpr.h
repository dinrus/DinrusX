// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Session handler for squads (similar to parties from other popular online shooters)

-------------------------------------------------------------------------
History:
- 05:03:2010 : Created By Ben Parbury

*************************************************************************/
#ifndef ___SQUAD_H___
#define ___SQUAD_H___

#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/DinrusXGame/GameMechanismBase.h>
#include <DinrusX/DinrusXGame/Network/Squad/ISquadEventListener.h>
#include <DinrusX/DinrusXGame/Network/Lobby/AutoLockData.h>
#include <DinrusX/DinrusXGame/Network/Lobby/SessionNames.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameUserPackets.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameLobby.h>
#include <DinrusX/DinrusXGame/Network/LobbyTaskQueue.h>

#define SQUADMGR_MAX_SQUAD_SIZE			(MAX_PLAYER_LIMIT)

#define SQUADMGR_DBG_ADD_FAKE_RESERVATION		(0 && !defined(_RELEASE))  // BE CAREFUL COMMITTING THIS! (must be 0!)

#define SQUADMGR_NUM_STORED_KICKED_SESSION 8

class CSquadUpr : public CGameMechanismBase,
											public IHostMigrationEventListener,
											public IGameWarningsListener
{
private:
	typedef std::vector<ISquadEventListener*> TSquadEventListenersVec;

public:
	enum EGameSessionChange
	{
		eGSC_JoinedNewSession,
		eGSC_LeftSession,
		eGSC_LobbyMerged,
		eGSC_LobbyMigrated,
	};

	enum ESessionSlotType
	{
		eSST_Public,
		eSST_Private,
	};

	CSquadUpr();
	virtual ~CSquadUpr();

	// Squad Event Listeners
	void RegisterSquadEventListener(ISquadEventListener *pListener);
	void UnregisterSquadEventListener(ISquadEventListener *pListener);

	void EventListenerSquaddieAdded(DrxUserID userId);
	void EventListenerSquaddieRemoved(DrxUserID userId);
	void EventListenerSquaddieUpdated(DrxUserID userId);
	void EventListenerSquadLeaderChanged(DrxUserID userId);
	void EventListenerSquadEvent(ISquadEventListener::ESquadEventType eventType);
	// ~Squad Event Listeners

	virtual void Update(float dt);

	inline bool HaveSquadMates() const { return m_nameList.Size() > 1; }
	inline bool InCharge() const { return m_squadLeader; }
	inline bool InSquad() const { return m_squadHandle != DrxSessionInvalidHandle; }
	inline bool IsLeavingSquad() const { return m_leavingSession; }
	inline int GetSquadSize() const { return m_nameList.Size(); }
	inline DrxSessionHandle GetSquadSessionHandle() { return m_squadHandle; }
	inline DrxUserID GetSquadLeader() { return m_squadLeaderId; }

	bool IsSquadMateByUserId(DrxUserID userId);
	
	void GameSessionIdChanged(EGameSessionChange eventType, DrxSessionID gameSessionId);
	void ReservationsFinished(EReservationResult result);
	void LeftGameSessionInProgress();

	void JoinGameSession(DrxSessionID gameSessionId, bool bIsMatchmakingSession);
	void TellMembersToLeaveGameSession();
	void RequestLeaveSquad();

	void Enable(bool enable, bool allowCreate);
	DrxUserID GetUserIDFromChannelID(int channelId);

	void LocalUserDataUpdated();

#if !defined(_RELEASE)
	static void CmdCreate(IConsoleCmdArgs* pCmdArgs);
	static void CmdLeave(IConsoleCmdArgs* pCmdArgs);
	static void CmdKick(IConsoleCmdArgs* pCmdArgs);
#endif

	void SendSquadPacket(GameUserPacketDefinitions packetType, SDrxMatchMakingConnectionUID connectionUID = DrxMatchMakingInvalidConnectionUID);
	static void HandleCustomError(const char* dialogName, const char* msgPreLoc, const bool deleteSession, const bool returnToMainMenu);

	// IHostMigrationEventListener
	virtual EHostMigrationReturn OnInitiate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnDisconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnDemoteToClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnPromoteToServer(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnReconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnFinalise(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnTerminate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual void OnComplete(SHostMigrationInfo& hostMigrationInfo) {}
	virtual EHostMigrationReturn OnReset(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	// ~IHostMigrationEventListener

	// IGameWarningsListener
	virtual bool OnWarningReturn(THUDWarningId id, const char* returnValue);
	virtual void OnWarningRemoved(THUDWarningId id) {}
	// ~IGameWarningsListener

	void SetMultiplayer(bool multiplayer);
	bool GetSquadCommonDLCs(uint32 &commonDLCs);

	void InviteAccepted(DrxSessionID id);
	void JoinInvite();
	inline void SetInvitePending(bool yesNo) { m_pendingInvite = yesNo; }

	void OnGameSessionStarted();
	void OnGameSessionEnded();

	bool SquadsSupported();
	void SessionChangeSlotType(ESessionSlotType type);

	void KickPlayer(DrxUserID userId);
	bool AllowedToJoinSession(DrxSessionID sessionId);
	void RemoveFromBannedList(DrxSessionID sessionId);

	bool IsEnabled();

	const SSessionNames *GetSessionNames() const
	{
		return &m_nameList;
	}

private:

	struct SKickedSession
	{
		DrxSessionID m_sessionId;
		CTimeValue m_timeKicked;
	};

	typedef DrxFixedArray<SKickedSession, SQUADMGR_NUM_STORED_KICKED_SESSION> TKickedSessionsArray;

	struct SPendingGameJoin
	{
		DrxSessionID	m_sessionID;
		uint32				m_playlistID;
		int						m_restrictRank;
		int						m_requireRank;
		bool					m_isMatchmakingGame;
		bool					m_isValid;

		SPendingGameJoin()
		{
			Invalidate();
		}

		void Set(DrxSessionID sessionID, bool isMatchmaking, uint32 playlistID, int restrictRank, int requireRank, bool isValid)
		{
			m_sessionID = sessionID;
			m_playlistID = playlistID;
			m_restrictRank = restrictRank;
			m_isMatchmakingGame = isMatchmaking;
			m_requireRank = requireRank;
			m_isValid = isValid;
		}

		void Invalidate()
		{
			Set(DrxSessionInvalidID, false, 0, 0, 0, false);
		}

		bool IsValid()
		{
			return m_isValid;
		}
	};

	TSquadEventListenersVec   m_squadEventListeners;

	SSessionNames m_nameList;

	TKickedSessionsArray m_kickedSessions;
	string m_kickedSessionsUsername;

	DrxSessionHandle m_squadHandle;

	DrxSessionID m_currentGameSessionId;
	DrxSessionID m_requestedGameSessionId;
	DrxSessionID m_inviteSessionId;

	DrxUserID m_squadLeaderId;
	DrxUserID m_pendingKickUserId;

	DrxLobbyTaskID m_currentTaskId;

	ELobbyState	m_leaderLobbyState;

	ESessionSlotType m_slotType;
	ESessionSlotType m_requestedSlotType;
	ESessionSlotType m_inProgressSlotType;

	SPendingGameJoin m_pendingGameJoin;

	float m_inviteJoiningTime;

	bool m_squadLeader;
	bool m_isNewSquad;
	bool m_bMultiplayerGame;
	bool m_pendingInvite;
	bool m_bSessionStarted;
	bool m_bGameSessionStarted;
	bool m_sessionIsInvalid;
	bool m_leavingSession;

	CLobbyTaskQueue m_taskQueue;

	struct SFlashSquadPlayerInfo
	{
		SFlashSquadPlayerInfo()
		{
			m_conId = 0;
			m_rank = 0;
			m_reincarnations = 0;
			m_isLocal = false;
			m_isSquadLeader = false;
		}
		
		DrxFixedStringT<DISPLAY_NAME_LENGTH> m_nameString;
		uint32 m_conId;
		uint8 m_rank;
		uint8 m_reincarnations;
		bool m_isLocal;
		bool m_isSquadLeader;
	};

	static void ReportError(EDrxLobbyError);

	void SetSquadHandle(DrxSessionHandle handle);

	void ReadSquadPacket(SDrxLobbyUserPacketData** ppPacketData);

	void OnSquadLeaderChanged();
	void JoinUser(SDrxUserInfoResult* user);
	void UpdateUser(SDrxUserInfoResult* user);
	void LeaveUser(SDrxUserInfoResult* user);

	void TaskFinished();
	bool CallbackReceived(DrxLobbyTaskID taskId, EDrxLobbyError result);

	void DeleteSession();
	void CleanUpSession();


	EDrxLobbyError DoCreateSquad(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoJoinSquad(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoLeaveSquad(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoUpdateLocalUserData(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoStartSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoEndSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoSessionChangeSlotType(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoSessionSetLocalFlags(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);


	void JoinSessionFinished(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle hdl);
	void SquadSessionDeleted(DrxLobbyTaskID taskID, EDrxLobbyError error);
	void SessionChangeSlotTypeFinished(DrxLobbyTaskID taskID, EDrxLobbyError error);

	void SquadJoinGame(DrxSessionID sessionID, bool isMatchmakingGame, uint32 playlistID, int restrictRank, int requireRank);

	static void CreateCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, void* arg);
	static void JoinCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, uint32 ip, uint16 port, void* arg);
	static void DeleteCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void SessionChangeSlotTypeCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void UserPacketCallback(UDrxLobbyEventData eventData, void *userParam);
	static void OnlineCallback(UDrxLobbyEventData eventData, void *userParam);
	static void JoinUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void LeaveUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void UpdateUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void UpdateLocalUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void UpdateOfflineState(CSquadUpr *pSquadUpr);
	static void SessionStartCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void SessionEndCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void SessionClosedCallback(UDrxLobbyEventData eventData, void *userParam);
	static void SessionSetLocalFlagsCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, uint32 flags, void* pArg);
	static void ForcedFromRoomCallback(UDrxLobbyEventData eventData, void *pArg);
	
	static void TaskStartedCallback(CLobbyTaskQueue::ESessionTask task, void *pArg);
};

#endif
