// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef ___GAME_BROWSER_H___
#define ___GAME_BROWSER_H___

#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>
#include <DinrusX/DinrusXGame/GameMechanismBase.h>

#if IMPLEMENT_PC_BLADES
#include <DinrusX/DinrusXGame/Network/Lobby/GameServerLists.h>
#endif

#if defined( DEDICATED_SERVER )
#define DEDI_VERSION	( 1 )
#endif

#define MAX_PRESENCE_STRING_SIZE 63
#define MATCHMAKING_SESSION_PASSWORD_MAX_LENGTH (32)

#if ! defined(RELEASE)
#define USE_SESSION_SEARCH_SIMULATOR
#endif

//////////////////////////////////////////////////////////////////////////
//Pre-declarations
class CSessionSearchSimulator;


class CGameBrowser
{
public :
	CGameBrowser();
	~CGameBrowser();
	void Init( void );
	void StartSearchingForServers(DrxMatchmakingSessionSearchCallback cb = CGameBrowser::MatchmakingSessionSearchCallback);
	EDrxLobbyError StartSearchingForServers(SDrxSessionSearchParam* param, DrxMatchmakingSessionSearchCallback cb, void* cbArg, const bool bFavouriteIdSearch);
	void CancelSearching(bool feedback = true);
	void FinishedSearch(bool feedback, bool finishedSearch);

	void Update(const float dt);

	const ENatType GetNatType() const;
	const char * GetNatTypeString() const;

	// Callback (public as also used by CGame in SP)
	static void ConfigurationCallback(EDrxLobbyService service, SConfigurationParams *requestedParams, uint32 paramCount);
	static void InitialiseCallback(EDrxLobbyService service, EDrxLobbyError error, void* arg);

	static void InitLobbyServiceType();

#if IMPLEMENT_PC_BLADES
	void StartFavouriteIdSearch( const CGameServerLists::EGameServerLists serverList, uint32 *pFavouriteIds, uint32 numFavouriteIds );
#endif

protected:
	enum EDelayedSearchType
	{
		eDST_None = 0,
		eDST_Full,
		eDST_FavouriteId
	};

	bool DoFavouriteIdSearch();
	bool CanStartSearch();

#if defined(USE_SESSION_SEARCH_SIMULATOR)
	CSessionSearchSimulator* m_pSessionSearchSimulator;
#endif //defined(USE_SESSION_SEARCH_SIMULATOR)

	ENatType m_NatType;
	DrxLobbyTaskID m_searchingTask;
	EDelayedSearchType m_delayedSearchType;
	
#if IMPLEMENT_PC_BLADES
	uint32 m_searchFavouriteIds[CGameServerLists::k_maxServersStoredInList];
	uint32 m_currentSearchFavouriteIdIndex;
	uint32 m_numSearchFavouriteIds;

	CGameServerLists::EGameServerLists m_currentFavouriteIdSearchType;
#endif

	float m_lastSearchTime;

	bool m_bFavouriteIdSearch;

	void SetNatType(ENatType natType) { m_NatType = natType; }

	// Callbacks
	static void GetNatTypeCallback(UDrxLobbyEventData eventData, void *userParam);
	static void MatchmakingSessionSearchCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, SDrxSessionSearchResult* session, void* arg);

private:

#if DRX_PLATFORM_ORBIS
	static const char* GetGameModeStringFromId(int32 id);
	static const char* GetMapStringFromId(int32 id);
	static void LocalisePresenceString(DrxFixedStringT<MAX_PRESENCE_STRING_SIZE> &out, const char* stringId);
	static void LocaliseInGamePresenceString(DrxFixedStringT<MAX_PRESENCE_STRING_SIZE> &out, const char* stringId, const int32 gameModeId, const int32 mapId);
	static bool CreatePresenceString(DrxFixedStringT<MAX_PRESENCE_STRING_SIZE> &out, SDrxLobbyUserData *pData, uint32 numData);

public:
	static void UnpackRecievedInGamePresenceString(DrxFixedStringT<MAX_PRESENCE_STRING_SIZE> &out, const DrxFixedStringT<MAX_PRESENCE_STRING_SIZE>& inString);
#endif
};

#endif // ___GAME_LOBBY_H___
