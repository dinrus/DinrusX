// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef ___GAME_LOBBY_H___
#define ___GAME_LOBBY_H___

#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>
#include <DinrusX/DinrusXGame/GameLobbyData.h>
#include <DinrusX/DinrusXGame/GameUserPackets.h>
#include <DinrusX/DinrusXGame/AutoLockData.h>
#include <DinrusX/DinrusXGame/SessionNames.h>
#include <DinrusX/CoreX/String/UnicodeIterator.h>
#include <DinrusX/CoreX/Containers/DrxFixedArray.h>

#include <DinrusX/DinrusXGame/Network/LobbyTaskQueue.h>
#include <DinrusX/DinrusXGame/Audio/AudioSignalPlayer.h>
#include <DinrusX/DinrusXGame/PlayerProgression.h>
#include <DinrusX/DinrusXGame/TeamBalancing.h>
#include <DinrusX/DinrusXGame/UI/HUD/ErrorHandling.h>

struct SGameStartParams;
struct IFlashPlayer;
class CGameLobbyUpr;
class CDrxLobbyPacket;
class CGameLobbyCVars;

#define MAX_RESERVATIONS		(MAX_PLAYER_LIMIT - 1)

#define MAX_CHATMESSAGE_LENGTH 128
#define NUM_CHATMESSAGES_STORED 32
#define CHAT_MESSAGE_POSTFIX "> "  // this gets used for the lobby chat and the ingame chat

#define DETAILED_SESSION_INFO_MOTD_SIZE (256)		// allow 256 bytes for detailed session info Message Of The Day
#define DETAILED_SESSION_INFO_URL_SIZE	(256)		// allow 256 bytes for detailed session info data URL
#define DETAILED_SESSION_MAX_PLAYERS		(MAX_PLAYER_LIMIT) // number of players
#define DETAILED_SESSION_MAX_CUSTOMS		(32)		// technically, should match however many items are in the CPlayerlistUpr::m_custom vector, but I'm going to force it due to packet size limitations.

#define SPECTATE_MAX_ALLOWED (MAX_PLAYER_LIMIT - 2)

#define ENABLE_CHAT_MESSAGES 1

#define MAX_USER_DATAS 32

#if !defined(_RELEASE)
#define ENSURE_ON_MAIN_THREAD \
	if (CGameLobby::s_mainThreadHandle != DrxGetCurrentThreadId()) \
{ \
	DrxLogAlways("*** FIX ME - NOT ON MAIN THREAD ***"); \
	/* *((char*)NULL) = 0; */ \
}
#else
#define ENSURE_ON_MAIN_THREAD
#endif

#if ! defined(DEDICATED_SERVER)
#define TRACK_MATCHMAKING
#endif

//typedef void (*GameLobbyJoinCallback)(EDrxLobbyError error, void* arg);

enum EOnlineAttributeTaskType
{
	eOATT_Invalid = 0,
	eOATT_ReadUserData,
	eOATT_WriteUserData,
	eOATT_WriteToLeaderboards,
};

enum EOnlineAttributeTaskStatus
{
	eOATS_NotStarted = 0,
	eOATS_InProgress,
	eOATS_Success,
	eOATS_Failed,
};

enum EDetailedSessionInfoResponseFlags
{
	eDSIRF_Basic = 0,
	eDSIRF_IncludePlayers = BIT(0),
	eDSIRF_IncludeCustomFields = BIT(1),

	eDSIRF_All = eDSIRF_Basic | eDSIRF_IncludePlayers | eDSIRF_IncludeCustomFields,
};

struct SOnlineAttributeTask
{
	DrxUserID	m_user;
	DrxLobbyTaskID m_task;
	EOnlineAttributeTaskStatus m_status;
	EOnlineAttributeTaskType m_type;

	SOnlineAttributeTask()
	{
		m_user = DrxUserInvalidID;
		m_task = DrxLobbyInvalidTaskID;
		m_status = eOATS_NotStarted;
		m_type = eOATT_Invalid;
	}

	SOnlineAttributeTask(DrxUserID userID, DrxLobbyTaskID taskID, EOnlineAttributeTaskType taskType, EOnlineAttributeTaskStatus taskStatus)
	{
		m_user = userID;
		m_task = taskID;
		m_type = taskType;
		m_status = taskStatus;
	}
};

struct SDetailedServerInfo
{
	//-- Request
	DrxSessionID												m_sessionId;
	DrxLobbyTaskID											m_taskID;
	EDetailedSessionInfoResponseFlags		m_flags;

	//-- Response
	char																m_motd[DETAILED_SESSION_INFO_MOTD_SIZE];
	char																m_url[DETAILED_SESSION_INFO_URL_SIZE];
	char																m_names[MAX_PLAYER_LIMIT][DRXLOBBY_USER_NAME_LENGTH];
	uint16															m_customs[DETAILED_SESSION_MAX_CUSTOMS];
	uint16															m_namesCount;
};

struct SPlayerScores
{
	SDrxMatchMakingConnectionUID				m_playerId;
	int																	m_score;
	float																m_fracTimeInGame;
};


enum ELobbyState
{
	eLS_None,
	eLS_Initializing,	//join game for create game callbacks
	eLS_FindGame,
	eLS_JoinSession,
	eLS_Lobby,
	eLS_PreGame,
	eLS_PostGame,
	eLS_Game,
	eLS_EndSession,
	eLS_GameEnded, //server only
	eLS_Leaving,
};


enum ELobbyVOIPState
{
	eLVS_off = 1,			// No voice available
	eLVS_on,					// Have voice and is not muted
	eLVS_muted,				// Has been muted - manually or by filter
	eLVS_mutedWrongTeam,	// Has been muted because they're on the other team
	eLVS_speaking,		// Voice is not muted and is speaking
};

enum ELobbyAutomaticVOIPType		/* Type of automatic voice muting */
{
	eLAVT_start = 0,
	eLAVT_off = 0,					// No automatic muting
	eLAVT_allButParty,			// Mute all but your squad
	eLAVT_all,							// Mute all
	eLAVT_end,
};

enum ELobbyVoteStatus
{
	eLVS_notVoted						= -1,
	eLVS_awaitingCandidates	=  0,
	eLVS_votedLeft					=  1,
	eLVS_votedRight					=  2,
};

enum ELobbyNetworkedVoteStatus
{
	eLNVS_NotVoted,
	eLNVS_VotedLeft,
	eLNVS_VotedRight,
};

enum EReservationResult
{
	eRR_Fail = 0,
	eRR_Success,
	eRR_NoneNeeded,
};

enum ELobbyEntryType
{
	eLET_Lobby = 0,
	eLET_Squad,
	eLET_Matchmaking,
};

class CGameLobby : public IHostMigrationEventListener,
                   public IGameWarningsListener,
									 public ISystemEventListener
{
public:
	const static int SESSION_NAME_LENGTH = 32;

	enum EActiveStatus
	{
		eAS_Lobby,
		eAS_Game,
		eAS_EndGame,
		eAS_StartingGame,
	};

protected:

	struct SVotingChoiceInfo
	{
		void Reset()
		{
			m_levelName = "";
			m_gameRules = "";
			m_numVotes = 0;
		}

		DrxFixedStringT<64> m_levelName;
		DrxFixedStringT<32> m_gameRules;
		uint8 m_numVotes;
	};

	SDrxLobbyUserData m_userData[eLDI_Num];
	SDrxSessionData m_sessionData;

	DrxLobbyTaskID m_currentTaskId;

	ELobbyState m_state;
	ELobbyState m_requestedState;

	DrxSessionHandle m_currentSession;

	DrxSessionID m_pendingConnectSessionId;
	DrxFixedStringT<SESSION_NAME_LENGTH> m_pendingConnectSessionName;
	SDrxMatchMakingConnectionUID m_pendingReservationId;

	DrxFixedStringT<SESSION_NAME_LENGTH> m_currentSessionName;

	SDetailedServerInfo m_detailedServerInfo;

	SSessionNames m_nameList;

	uint32	m_sessionFavouriteKeyId;	// Session's associated user account as an id
	int m_endGameResponses;

	uint32 m_playListSeed;

	float m_leaveGameTimeout;

	SVotingChoiceInfo m_leftVoteChoice;
	SVotingChoiceInfo m_rightVoteChoice;

	uint32 m_dedicatedserverip;
	uint16 m_dedicatedserverport;

	uint8 m_highestLeadingVotesSoFar;

	bool m_leftHadHighestLeadingVotes;
	bool m_votingEnabled;
	bool m_votingClosed;
	bool m_leftWinsVotes;
	bool m_server;
	bool m_sessionHostOnly;
	bool m_connectedToDedicatedServer;
	bool m_sessionUserDataDirty;
	bool m_squadDirty;
	bool m_isLeaving;
	bool m_stateHasChanged;

	// Asymmetric game bools :)
	bool m_isAsymmetricGame;
	bool m_replayMapWithTeamsSwitched;

public:

	CGameLobby(CGameLobbyUpr* pMgr);
	virtual ~CGameLobby();
	void Update( float dt );

	bool ReadyToCheckDLC();
	bool CheckDLCRequirements();

	void CancelSessionInit();
	bool IsCreatingOrJoiningSession();
	bool IsCurrentSessionId( DrxSessionID id );

	bool JoinServer( DrxSessionID sessionId, const char *sessionName, const SDrxMatchMakingConnectionUID &reservationId, bool bRetryIfPassworded );
	void CreateSessionFromSettings( const char *pGameRules, const char *pLevelName );

	bool MergeToServer( DrxSessionID sessionId );
	void FindGameCreateGame();
	void InitGameMatchmaking();

	bool ShouldCallMapCommand( const char * pLevelName, const char *pGameRules );
	void OnMapCommandIssued();
	void OnStartPlaylistCommandIssued();

	bool IsRankedGame() const;

	inline ELobbyState GetState() { return m_state; }

	void SvFinishedGame(const float dt);

#if ENABLE_CHAT_MESSAGES
	static void CmdChatMessage(IConsoleCmdArgs* pCmdArgs);
	static void CmdChatMessageTeam(IConsoleCmdArgs* pCmdArgs);
	void SendChatMessage(bool team, const char* message);
#endif

	static void CmdStartGame(IConsoleCmdArgs* pCmdArgs);
	static void CmdSetMap(IConsoleCmdArgs* pCmdArgs);
	static void CmdSetGameRules(IConsoleCmdArgs* pCmdArgs);
	static void CmdVote(IConsoleCmdArgs* pCmdArgs);

	static const char* GetValidGameRules(const char* gameRules, bool returnBackup=false);
	static DrxFixedStringT<32> GetValidMapForGameRules(const char* inLevel, const char* gameRules, bool returnBackup=false);

	static void SetLocalUserData(uint8 * localUserData);
	int GetNumberOfExpectedClients() const;

	// UI Related

#if 0 // old frontend
	bool IsGameLobbyScreen(EFlashFrontEndScreens screen);
#endif
	const char* GetMapImageName(const char* levelFileName, DrxFixedStringT<128>* pOutLevelImageName);

	void SetChoosingGamemode(const bool bChoosingGamemode) { m_bChoosingGamemode = bChoosingGamemode; }
	void ResetLevelOverride() { m_uiOverrideLevel.clear(); }
	void GetCountDownStageStatusMessage(DrxFixedStringT<64> &statusString);
	void UpdateStatusMessage(IFlashPlayer *pFlashPlayer);

	void SendChatMessagesToFlash(IFlashPlayer *pFlashPlayer);
	void SendUserListToFlash(IFlashPlayer *pFlashPlayer);
	void SendSessionDetailsToFlash(IFlashPlayer *pFlashPlayer, const char* levelOverride=0);
	void UpdateMatchmakingDetails(IFlashPlayer *pFlashPlayer, const char* status);
	void UpdateVotingInfoFlashInfo();
	void UpdateVotingCandidatesFlashInfo();
	void ResetFlashInfos();
	bool CanShowGamercard(DrxUserID userId);
	void ShowGamercardByUserId(DrxUserID userId);
	void GetProgressionInfoByChannel(int channelId, uint8 &rank, uint8 &reincarnations);
	void SetProgressionInfoByChannel(int channelId, uint8 rank, uint8 reincarnations);

	void GetPlayerNameFromChannelId(int channelId, DrxFixedStringT<DRXLOBBY_USER_NAME_LENGTH> &name);
	void GetClanTagFromChannelId(int channelId, DrxFixedStringT<CLAN_TAG_LENGTH> &name);
	void LocalUserDataUpdated();

	bool IsLocalChannelUser(int channelId);
	DrxUserID GetLocalUserId();
	void GetLocalUserDisplayName(DrxFixedStringT<DISPLAY_NAME_LENGTH> &displayName);
	bool GetPlayerDisplayNameFromEntity(EntityId entityId, DrxFixedStringT<DISPLAY_NAME_LENGTH> &displayName);
	bool GetPlayerDisplayNameFromChannelId(int channelId, DrxFixedStringT<DISPLAY_NAME_LENGTH> &displayName);

	bool GetSpectatorStatusFromChannelId(int channelId);
	void SetLocalSpectatorStatus(bool spectator);

	ELobbyAutomaticVOIPType GetVOIPAutoMutingType() { return m_autoVOIPMutingType; }

	DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> GetGUIDFromActorID(EntityId actorId);
	DrxUserID GetUserIDFromChannelID(int channelId);
	int GetTeamByChannelId(int channelId);
	int GetTeamByConnectionUID( SDrxMatchMakingConnectionUID uid );
	SDrxMatchMakingConnectionUID GetConnectionUIDFromChannelID(int channelId);
	DrxPing GetCurrentPingToHost();

	inline void OnSquadChanged() { m_squadDirty = true; }

	void MutePlayerByChannelId(int channelId, bool mute, int reason);
	int GetPlayerMuteReason(DrxUserID userId);
	ELobbyVOIPState GetVoiceState(int channelId);
	void CycleAutomaticMuting();
	void SetAutomaticMutingState(ELobbyAutomaticVOIPType newType);

	void SessionStartFailed(EDrxLobbyError error);
	void SessionEndFailed(EDrxLobbyError error);

	void FindGameMoveSession(DrxSessionID sessionId);
	void FindGameCancelMove();

	void MakeReservations(SSessionNames* nameList, bool squadReservation);
	void SwitchToPrimaryLobby();

	void SetPrivateGame(bool enable);
	inline void SetPasswordedGame(bool enable) { m_passwordGame = enable; }
	void SetMatchmakingGame(bool bMatchmakingGame);

	inline bool IsServer() { return m_server; }
	inline bool IsPrivateGame() const { return m_privateGame; }
	inline bool IsPasswordedGame() const { return  m_passwordGame; }
	inline bool IsOnlineGame() { return (gEnv->pNetwork->GetLobby()->GetLobbyServiceType() == eCLS_Online); }
	inline bool IsMatchmakingGame() { return m_bMatchmakingSession; }
	DrxUserID GetHostUserId();

	inline bool IsCurrentlyInSession() { return m_currentSession != DrxSessionInvalidHandle; }

	inline const char* GetCurrentLevelName() { return m_currentLevelName.c_str(); }
	const char* GetCurrentGameModeName(const char* unknownStr="Unknown");

	const char* GetLoadingLevelName() { return m_loadingLevelName.c_str(); }
	const char* GetLoadingGameModeName() { return m_loadingGameRules.c_str(); }

	const SSessionNames &GetSessionNames() const
	{
		return m_nameList;
	}

	void GetSessionIDAsString(char* pOutString, int inBufferSize);

	SSessionNames::SSessionName* GetSessionNameByChannelId(int channelId);

	void LeaveAfterSquadMembers();

	void OnOptionsChanged();

	// IHostMigrationEventListener
	virtual EHostMigrationReturn OnInitiate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnDisconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnDemoteToClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnPromoteToServer(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnReconnectClient(SHostMigrationInfo& hostMigrationInfo, HMStateType& state) { return IHostMigrationEventListener::Listener_Done; }
	virtual EHostMigrationReturn OnFinalise(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual EHostMigrationReturn OnTerminate(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	virtual void OnComplete(SHostMigrationInfo& hostMigrationInfo) {}
	virtual EHostMigrationReturn OnReset(SHostMigrationInfo& hostMigrationInfo, HMStateType& state);
	// ~IHostMigrationEventListener

	// IGameWarningsListener
	virtual bool OnWarningReturn(THUDWarningId id, const char* returnValue);
	virtual void OnWarningRemoved(THUDWarningId id);
	// ~IGameWarningsListener

	// ISystemEventListener
	virtual void OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam );
	// ~ISystemEventListener

	static void SetLobbyService(EDrxLobbyService lobbyService);
	static THUDWarningId ShowErrorDialog(const EDrxLobbyError error, const char* pDialogName, const char* pDialogParam, IGameWarningsListener* pWarningsListener);
	static bool IsSignInError(EDrxLobbyError error);

	void StartFindGame();
	void SetQuickMatchRanked(bool bIsRanked);
	void SetQuickMatchGameRules(const char *pGameRules);
	void LeaveSession(bool clearPendingTasks, bool bWasUserInitiated);

	void OnHaveLocalPlayer();

	uint16 GetSkillRanking(int channelId);

	void DebugAdvancePlaylist();

	void SetQuitting(bool yesNo) { m_bQuitting = yesNo; }

	void MoveUsers(CGameLobby *pFromLobby);

	eHostMigrationState GetMatchMakingHostMigrationState();
	void TerminateHostMigration();

	uint32 GetSessionFavouriteKeyId() const { return m_sessionFavouriteKeyId; }
	const char* GetSessionName();

	//-- Ask for extended server info
	void RequestDetailedServerInfo(DrxSessionID sessionId, EDetailedSessionInfoResponseFlags flags);

	SDetailedServerInfo* GetDetailedServerInfo();
	void CancelDetailedServerInfoRequest();

	void UpdateDebugString();

	bool AllowCustomiseEquipment();
	void RefreshCustomiseEquipment();

	void AbortLoading();

	struct SChatMessage
	{
		SChatMessage()
		{
			Clear();
		}

		void Set(SDrxMatchMakingConnectionUID inConId, int inTeamId, const char* inMessage)
		{
			if (inMessage)
			{
				if (strlen(inMessage) > MAX_CHATMESSAGE_LENGTH-1)
				{
					// Find the first full code-point that won't fit, then go one before that.
					// Just cutting off the buffer could happen in the middle of a multi-byte sequence.
					Unicode::CIterator<const char *, false> it(inMessage + MAX_CHATMESSAGE_LENGTH - 1);
					--it;
					size_t length = it.GetPosition() - inMessage;
					assert(length < MAX_CHATMESSAGE_LENGTH);
					message.assign(inMessage, length);
				}
				else
				{
					message = inMessage;
				}
			}
			conId = inConId;
			teamId = inTeamId;
		}

		void Clear()
		{
			message.clear();
			conId = DrxMatchMakingInvalidConnectionUID;
			teamId = 0;
		}

		DrxFixedStringT<MAX_CHATMESSAGE_LENGTH> message;
		SDrxMatchMakingConnectionUID conId;
		int teamId;
	};

	ILINE void LocalMessage(const SChatMessage* message) { RecievedChatMessage(message); }

	void QueueSessionUpdate();

	bool CheckRankRestrictions();
	void UpdatePreviousGameScores();

	inline DrxSessionHandle GetCurrentSessionHandle() { return m_currentSession; }

	void RequestLeaveFromMenu();
	bool IsMidGameLeaving() const { return m_isMidGameLeaving; }

	void ChangeMap(const char *pMapName);
	void ChangeGameRules(const char *pGameRulesName);

	bool IsGameStarting() const { return m_countdownStage == eCDS_Started; }

	bool IsBadServer(DrxSessionID sessionId);
	EActiveStatus GetActiveStatus(const ELobbyState currentState) const;
	int32 CalculateAverageSkill();
	int32 GetCurrentLanguageId();

	bool UseLobbyTeamBalancing() const { return m_isTeamGame; }
	bool UsePlayerTeamVisualization() const;
	bool IsGameBalanced() const { return !m_isTeamGame || m_teamBalancing.IsGameBalanced(); }
	void OnPlayerSpawned(int channelId);
	void OnPlayerSwitchedTeam(int channelId, int teamId);

	void ForceBalanceTeams();

	bool IsSessionMigratable();
	bool ShouldMigrateNub();

private:

	struct SFindGameResults
	{
		SFindGameResults(const char* name, DrxSessionID sessionId, float score, bool bIsBadServer)
		{
			memcpy(&m_name, name, MAX_SESSION_NAME_LENGTH);
			m_sessionId = sessionId;
			m_score = score;
			m_isBadServer = (bIsBadServer ? 1 : 0);
		};

		char m_name[MAX_SESSION_NAME_LENGTH];
		DrxSessionID m_sessionId;
		float m_score;
		uint8 m_isBadServer;
	};

	struct SFlashLobbyPlayerInfo
	{
		SFlashLobbyPlayerInfo()
		{
			m_conId=0;
			m_rank=0;
			m_reincarnations = 0;
			m_voiceState=0;
			m_teamId=0;

			m_entryType=eLET_Lobby;
			m_onLocalTeam=false;
			m_isLocal=false;
			m_isSquadMember=false;
			m_isSquadLeader=false;
		}

		DrxFixedStringT<DISPLAY_NAME_LENGTH> m_nameString;
		uint32 m_conId;
		uint8 m_rank;
		uint8 m_reincarnations;
		uint8 m_voiceState;

		int m_teamId;
		ELobbyEntryType m_entryType;
		bool m_onLocalTeam;
		bool m_isLocal;
		bool m_isSquadMember;
		bool m_isSquadLeader;
	};

	struct SSlotReservation
	{
		SDrxMatchMakingConnectionUID  m_con;
		float  m_timeStamp;
	};

	struct SVotingFlashInfo
	{
		DrxFixedStringT<64> votingStatusMessage;

		int  leftNumVotes;
		int  rightNumVotes;
		bool  votingClosed;
		bool  votingDrawn;
		bool  leftWins;
		bool  localHasCandidates;
		bool  localHasVoted;
		bool  localVotedLeft;
#ifndef _RELEASE
		bool  tmpWatchInfoIsSet;
#endif
		void Reset()
		{
			votingStatusMessage.clear();

			leftNumVotes = 0;
			rightNumVotes = 0;
			votingClosed = false;
			votingDrawn = false;
			leftWins = false;
			localHasCandidates = false;
			localHasVoted = false;
			localVotedLeft = false;
#ifndef _RELEASE
			tmpWatchInfoIsSet = false;
#endif
		}
	};

	struct SVotingCandidatesFlashInfo
	{
		DrxFixedStringT<64>  leftLevelMapPath;
		DrxFixedStringT<64>  leftLevelName;
		DrxFixedStringT<32>  leftRulesName;
		DrxFixedStringT<128>  leftLevelImage;
		DrxFixedStringT<64>  rightLevelMapPath;
		DrxFixedStringT<64>  rightLevelName;
		DrxFixedStringT<32>  rightRulesName;
		DrxFixedStringT<128>  rightLevelImage;

#ifndef _RELEASE
		bool  tmpWatchInfoIsSet;
#endif
		void Reset()
		{
			leftLevelMapPath.clear();
			leftLevelName.clear();
			leftRulesName.clear();
			leftLevelImage.clear();
			rightLevelMapPath.clear();
			rightLevelName.clear();
			rightRulesName.clear();
			rightLevelImage.clear();
#ifndef _RELEASE
			tmpWatchInfoIsSet = false;
#endif
		}
	};

	typedef DrxFixedStringT<MAX_CHATMESSAGE_LENGTH> TChatMessageDisplayString;
	struct SFlashChatMessage
	{
		SFlashChatMessage() { Clear(); }

		void Clear()
		{
			m_name.clear();
			m_message.clear();
			m_local = false;
			m_squaddie = false;
		}

		void Set(const char* name, const char* message, bool isLocal, bool isSquaddie)
		{
			Clear();

			m_name = name;
			m_message = message;
			m_local = isLocal;
			m_squaddie = isSquaddie;
		}

		void SendChatMessageToFlash(IFlashPlayer *pFlashPlayer, bool isInit);

		DrxFixedStringT<DISPLAY_NAME_LENGTH> m_name;
		DrxFixedStringT<MAX_CHATMESSAGE_LENGTH> m_message;
		bool m_local;
		bool m_squaddie;
	};


	CLobbyTaskQueue m_taskQueue;

#if GAMELOBBY_USE_COUNTRY_FILTERING
	CTimeValue m_timeSearchStarted;
#endif

#if ENABLE_CHAT_MESSAGES
	SFlashChatMessage					m_chatMessagesArray[NUM_CHATMESSAGES_STORED];
	int												m_chatMessagesIndex;
#endif

	SChatMessage m_chatMessageStore;

	const static int k_maxBadServers = 4;
	typedef DrxFixedArray<DrxSessionID, k_maxBadServers> TBadServersArray;
	TBadServersArray m_badServers;
	int m_badServersHead;

	int m_findGameNumRetries;
	static int s_currentMMSearchID;

	const static int k_maxFoundGames = 20;
	typedef DrxFixedArray<SFindGameResults, k_maxFoundGames> TFindGames;
	TFindGames m_findGameResults;

	char m_joinCommand[128];
	THUDWarningId m_DLCServerStartWarningId;

	float m_startTimer;
	float m_findGameTimeout;
	float m_lastUserListUpdateTime;
	float m_timeTillCallToEnsureBestHost;
	float m_startTimerLength;
	float m_timeTillUpdateSession;

	EActiveStatus m_lastActiveStatus;
	int32 m_lastUpdatedAverageSkill;
	int m_numPlayerJoinResets;

	volatile static bool s_bShouldBeSearching;

	bool m_hasReceivedSessionQueryCallback;
	bool m_hasReceivedStartCountdownPacket;
	bool m_hasReceivedPlaylistSync;
	bool m_hasReceivedMapCommand;
	bool m_gameHadStartedWhenPlaylistRotationSynced;
	bool m_initialStartTimerCountdown;
	bool m_isTeamGame;
	bool m_hasValidGameRules;
	bool m_shouldFindGames;
	bool m_privateGame;
	bool m_passwordGame;
	bool m_bMatchmakingSession;
	bool m_bWaitingForGameToFinish;
	bool m_bMigratedSession;
	bool m_bSessionStarted;
	bool m_bCancelling;
	bool m_bQuitting;
	bool m_bNeedToSetAsElegibleForHostMigration;
	bool m_bPlaylistHasBeenAdvancedThroughConsole;
	bool m_bSkipCountdown;			// Set by gl_startGame command
	bool m_allowRemoveUsers;
	bool m_bServerUnloadRequired;
	bool m_bChoosingGamemode;
	bool m_bHasUserList;
	bool m_bHasReceivedVoteOptions;
	bool m_isMidGameLeaving;
	bool m_bRetryIfPassworded;
	bool m_bAllocatedServer;
	bool m_bIsAutoStartingGame;

	DrxSessionID m_currentSessionId;
	DrxSessionID m_nextSessionId;
	CGameLobbyUpr* m_gameLobbyMgr;

	SSessionNames* m_reservationList;
	bool m_squadReservation;
	SSlotReservation m_slotReservations[MAX_RESERVATIONS];

	ELobbyAutomaticVOIPType m_autoVOIPMutingType;

	ELobbyVoteStatus m_localVoteStatus;
	ELobbyNetworkedVoteStatus m_networkedVoteStatus;

	DrxFixedStringT<128> m_uiOverrideLevel;

	SVotingFlashInfo  m_votingFlashInfo;
	SVotingCandidatesFlashInfo  m_votingCandidatesFlashInfo;

	CAudioSignalPlayer m_lobbyCountdown;

	SDrxMatchMakingConnectionUID m_allocatedServerUID;

	void EnterState(ELobbyState prevState, ELobbyState newState);
	void UpdateState();

	void StartGame();
	bool UpdateLevelRotation();

	void ClearChatMessages();
	void RecievedChatMessage(const SChatMessage* message);
	void SendChatMessageToClients();

	void SvInitialiseRotationAdvanceAtFirstLevel();
	void SvResetVotingForNextElection();
	void SvVoteForLevel(const bool voteLeft);
	void SvCloseVotingAndDecideWinner();
	void AdvanceLevelRotationAccordingToVotingResults();

	const char* GetMapDescription(const char* levelFileName, DrxFixedStringT<64>* pOutLevelDescription);

	void SetCurrentSession(DrxSessionHandle h);
	void SetCurrentId(DrxSessionID id, bool isCreator, bool isMigratedSession);

	void SendPacket(CDrxLobbyPacket *pPacket, GameUserPacketDefinitions packetType, SDrxMatchMakingConnectionUID connectionUID = DrxMatchMakingInvalidConnectionUID);
	void SendPacket(GameUserPacketDefinitions packetType, SDrxMatchMakingConnectionUID connectionUID = DrxMatchMakingInvalidConnectionUID);
	void ReadPacket(SDrxLobbyUserPacketData** ppPacketData);

	void FindGameEnter();

	float GetFindGameTimeout();

	void JoinServerFailed(EDrxLobbyError error, DrxSessionID serverSessionId);
	void InformSquadUprOfSessionId();

	static int BuildReservationsRequestList(SDrxMatchMakingConnectionUID reservationRequests[], const int maxRequests, const SSessionNames*  members);
	EReservationResult DoReservations(const int numReservationsRequested, const SDrxMatchMakingConnectionUID requestedReservations[]);

	void InsertUser(SDrxUserInfoResult* user);
	void UpdateUser(SDrxUserInfoResult* user);
	void RemoveUser(SDrxUserInfoResult* user);
	void GameRulesChanged(const char *pGameRules);
	void SetCurrentLevelName(const char *pCurrentLevelName, bool preCacheLevel = true);

	static bool SortPlayersByTeam(const SFlashLobbyPlayerInfo &elem1, const SFlashLobbyPlayerInfo &elem2);
	uint32 GetSessionCreateFlags() const;

	void SetLobbyTaskId(DrxLobbyTaskID taskId);

	void RecordReceiptOfSessionQueryCallback() { ENSURE_ON_MAIN_THREAD; m_hasReceivedSessionQueryCallback = true; }
	void SetupSessionData();
	void ClearBadServers();

	void UpdatePrivatePasswordedGame();

	bool BidirectionalMergingRequiresCancel(DrxSessionID other);

	// Callbacks
	static void MatchmakingSessionCreateCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, void* arg);
	static void MatchmakingSessionMigrateCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, void* arg);
	static void MatchmakingSessionJoinCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, uint32 ip, uint16 port, void* arg);
	static void MatchmakingSessionDeleteCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void MatchmakingSessionQueryCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, SDrxSessionSearchResult* session, void* arg);
	static void MatchmakingSessionUpdateCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void MatchmakingSessionUserPacketCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionRoomOwnerChangedCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionJoinUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionLeaveUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionUpdateUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionClosedCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionKickedCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionKickedHighPingCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionKickedReservedUserCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingLocalUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void MatchmakingEnsureBestHostCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* arg);
	static void MatchmakingSessionStartCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void MatchmakingSessionEndCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void MatchmakingSessionTerminateHostHintingForGroupCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void MatchmakingSessionSetLocalFlagsCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, DrxSessionHandle h, uint32 flags, void* pArg);
	static void MatchmakingForcedFromRoomCallback(UDrxLobbyEventData eventData, void *pArg);
	static void MatchmakingSessionDetailedInfoRequestCallback(UDrxLobbyEventData eventData, void *pArg);
	static void MatchmakingSessionDetailedInfoResponseCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, CDrxLobbyPacket* pPacket, void* pArg);

	static void MatchmakingSetupDedicatedServerCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, uint32 ip, uint16 port, void* pArg);
	static void MatchmakingReleaseDedicatedServerCallback(DrxLobbyTaskID taskId, EDrxLobbyError error, void *pArg);
	static void MatchmakingDedicatedServerSetup(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingDedicatedServerRelease(UDrxLobbyEventData eventData, void *userParam);
	static void InviteAcceptedCallback(UDrxLobbyEventData eventData, void *userParam);

	static void WriteOnlineAttributeData(CGameLobby *pLobby, CDrxLobbyPacket *pPacket, GameUserPacketDefinitions packetType, SDrxLobbyUserData *pData, int32 numData, SDrxMatchMakingConnectionUID connnectionUID);

	static void TaskStartedCallback(CLobbyTaskQueue::ESessionTask task, void *pArg);

	static void SendChatMessageAllCheckProfanityCallback( DrxLobbyTaskID taskID, EDrxLobbyError error, const char* pString, bool isProfanity, void* pArg );
	static void SendChatMessageTeamCheckProfanityCallback( DrxLobbyTaskID taskID, EDrxLobbyError error, const char* pString, bool isProfanity, void* pArg );
	void SendChatMessageCheckProfanityCallback( const bool team, DrxLobbyTaskID taskID, EDrxLobbyError error, const char* pString, bool isProfanity, void* pArg );

	static void MatchmakingSessionClosed(UDrxLobbyEventData eventData, void *userParam, CErrorHandling::ENonFatalError error);

private:

	bool GetConnectionUIDFromUserID( const DrxUserID userId, SDrxMatchMakingConnectionUID &result );
	bool GetUserIDFromConnectionUID( const SDrxMatchMakingConnectionUID& conId, DrxUserID &result );

	void MutePlayersOnTeam(uint8 teamId, bool mute);
	void MutePlayerBySessionName(SSessionNames::SSessionName *pUser, bool mute, int reason);
	void SetAutomaticMutingStateForPlayer(SSessionNames::SSessionName *pPlayer, ELobbyAutomaticVOIPType newType);

	void OnGameStarting();

	bool ShouldCheckForBestHost();

	void CheckCanLeave();

	void SetState(ELobbyState state);
	void FinishDelete(EDrxLobbyError result);

	bool NetworkCallbackReceived( DrxLobbyTaskID taskId, EDrxLobbyError result );

	void CancelLobbyTask(CLobbyTaskQueue::ESessionTask taskType);
	void CancelAllLobbyTasks();

	void SessionEndCleanup();
	EDrxLobbyError DoCreateSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoMigrateSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoJoinServer(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoDeleteSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoUpdateLocalUserData(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoStartSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId, bool &bTaskStartedOut);
	EDrxLobbyError DoEndSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoQuerySession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoUpdateSession(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoEnsureBestHost(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoTerminateHostHintingForGroup(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId, bool &bTaskStartedOut);
	EDrxLobbyError DoSessionSetLocalFlags(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoSessionDetailedInfo(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId);
	EDrxLobbyError DoSetupDedicatedServer(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId, bool &bTaskStartedOut);
	EDrxLobbyError DoReleaseDedicatedServer(IDrxMatchMaking *pMatchMaking, DrxLobbyTaskID &taskId, bool &bTaskStartedOut);

	void DoUnload();

	void ConnectionFailed(CErrorHandling::ENonFatalError error);

#ifndef _RELEASE
	static void CmdTestTeamBalancing(IConsoleCmdArgs *pArgs);
	static void CmdTestMuteTeam(IConsoleCmdArgs *pArgs);
	static void CmdCallEnsureBestHost(IConsoleCmdArgs *pArgs);
	static void CmdFillReservationSlots(IConsoleCmdArgs *pArgs);

	float m_timeTillAutoLeaveLobby;
	int m_failedSearchCount;
	bool m_migrationStarted;
#endif

	static void CmdAdvancePlaylist(IConsoleCmdArgs *pArgs);
	static void CmdDumpValidMaps(IConsoleCmdArgs *pArgs);

	void SetLocalVoteStatus(ELobbyVoteStatus state);
	ELobbyNetworkedVoteStatus GetVotingStateForPlayer(SSessionNames::SSessionName *pSessionName) const;
	void CheckForVotingChanges(bool bUpdateFlash);
	bool CalculateVotes();
	void ResetLocalVotingData();

	void CheckForSkillChange();
	int MaxNonSplitSquadSize();

	void UpdateRulesAndMapFromVoting();
	void UpdateRulesAndLevel(const char *pGameRules, const char *pLevelName);
	void UpdateVoteChoices();

	bool IsQuitting() { return m_bQuitting; }

	int GetNumPublicSlots() { return m_sessionData.m_numPublicSlots; }
	int GetNumPrivateSlots() { return m_sessionData.m_numPrivateSlots; }

	void PrecachePaks(const char* pLevelName);
	void PrecachePak(const string& fullPath, IResourceUpr& resourceUpr, const bool levelLoadOnly);
	void UnloadPrecachedPaks(const char* reason);
	void ClearLoggedPrecachedPaks(const char* reason);

	//list of level specific pak file names which have been precached
	std::vector<string> m_precachePaks;
	bool m_bAllowPakPrecaching;

	void UpdateTeams();

	bool IsAutoStartingGame()  const;

	bool	AllowForceBalanceTeams() const;

	CTeamBalancing m_teamBalancing;

	DrxFixedStringT<64> m_currentLevelName;
	DrxFixedStringT<32> m_currentGameRules;

	DrxFixedStringT<64> m_loadingLevelName;
	DrxFixedStringT<32> m_loadingGameRules;

	DrxFixedStringT<64> m_pendingLevelName;

	DrxFixedStringT<64> m_cachedLevelName;

	DrxFixedStringT<32> m_quickMatchGameRules;
	bool m_quickMatchRanked;
	bool m_bCanAbortLoading;

	DrxLobbyTaskID m_profanityTask;

	enum ECountdownStage
	{
		eCDS_WaitingForPlayers,
		eCDS_WaitingForBalancedGame,
		eCDS_Started,
	};

	ECountdownStage m_countdownStage;
	CTimeValue m_timeStartedWaitingForBalancedGame;

#if !defined(_RELEASE)
	public:
	static threadID s_mainThreadHandle;
#endif

	static CGameLobbyCVars* s_pGameLobbyCVars;

#if defined(DEDICATED_SERVER)
	struct SPlayerPingInfo
	{
		CTimeValue m_timePingExceededLimit;
		int m_channelId;
		bool m_bPingExceedsLimit;
	};

	typedef DrxFixedArray<SPlayerPingInfo, MAX_PLAYER_LIMIT> TPlayerPingInfoArray;

	TPlayerPingInfoArray m_playerPingInfo;
	float m_timeSinceLastPingCheck;

	void UpdatePingKicker(float frameTime);
#endif
};

#endif // ___GAME_LOBBY_H___
