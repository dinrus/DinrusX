// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __CRECENTPLAYERSMGR_H__
#define __CRECENTPLAYERSMGR_H__

//---------------------------------------------------------------------------

#include <DinrusX/DinrusXGame/Network/Squad/ISquadEventListener.h>
#include <DinrusX/DinrusXGame/Network/Lobby/IGameLobbyEventListener.h>
#include <DinrusX/CoreX/Containers/DrxFixedArray.h>
#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/DinrusXGame/Network/FriendUpr/GameFriendData.h>
#include <DinrusX/DinrusXGame/IRecentPlayersMgrEventListener.h>

//---------------------------------------------------------------------------
// CRecentPlayersMgr
//     A class to manage and store potential new friends a player has encountered since running the game
//---------------------------------------------------------------------------
class CRecentPlayersMgr : public ISquadEventListener, public IGameLobbyEventListener
{
public:
	typedef struct SRecentPlayerData
	{
		DrxUserID m_userId;
		TLobbyUserNameString m_name;
		TGameFriendId m_internalId;

		static TGameFriendId __s_friend_internal_ids;

		SRecentPlayerData(DrxUserID &inUserId, const char *inNameString)
		{
			m_userId = inUserId;
			m_name = inNameString;
			m_internalId = __s_friend_internal_ids++;
		}
	};
	
	static const int k_maxNumRecentPlayers=50;
	typedef DrxFixedArray<SRecentPlayerData, k_maxNumRecentPlayers> TRecentPlayersArray;


	CRecentPlayersMgr();
	virtual ~CRecentPlayersMgr();

	void Update(const float inFrameTime);

	void RegisterRecentPlayersMgrEventListener(IRecentPlayersMgrEventListener *pListener);
	void UnregisterRecentPlayersMgrEventListener(IRecentPlayersMgrEventListener *pListener);

	// ISquadEventListener
	virtual void AddedSquaddie(DrxUserID userId);
	virtual void RemovedSquaddie(DrxUserID userId) {}
	virtual void UpdatedSquaddie(DrxUserID userId);
	virtual void SquadLeaderChanged(DrxUserID oldLeaderId, DrxUserID newLeaderId) {}
	virtual void SquadNameChanged(const char *pInNewName) { }
	virtual void SquadEvent(ISquadEventListener::ESquadEventType eventType) { }
	// ~ISquadEventListener

	// IGameLobbyEventListener
	virtual void InsertedUser(DrxUserID userId, const char *userName);
	virtual void SessionChanged(const DrxSessionHandle inOldSession, const DrxSessionHandle inNewSession) { }
	// ~IGameLobbyEventListener

	const TRecentPlayersArray *GetRecentPlayers() const { return &m_players; }
	const SRecentPlayerData *FindRecentPlayerDataFromInternalId(TGameFriendId inInternalId);

	void OnUserChanged(DrxUserID localUserId);

protected:
	typedef std::vector<IRecentPlayersMgrEventListener*> TRecentPlayersFriendsEventListenersVec;
	TRecentPlayersFriendsEventListenersVec m_recentPlayersEventListeners;

	TRecentPlayersArray m_players;
	DrxUserID m_localUserId;

	void AddOrUpdatePlayer(DrxUserID &inUserId, const char *inUserName);
	SRecentPlayerData *FindRecentPlayerDataFromUserId(DrxUserID &inUserId);
	void EventRecentPlayersUpdated();
};
//---------------------------------------------------------------------------

#endif //__CRECENTPLAYERSMGR_H__
