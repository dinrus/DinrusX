// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   UILobbyMP.h
//  Version:     v1.00
//  Created:     08/06/2012 by Michiel Meesters.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UILOBBYMP_H_
#define __UILOBBYMP_H_

#include <DinrusX/DinrusXGame/IUIGameEventSystem.h>
#include <DinrusX/DinrusXSys/Scaleform/IFlashUI.h>
#include <DinrusX/CoreX/Lobby/IDrxStats.h>
#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameLobbyData.h>
#include <DinrusX/CoreX/Lobby/IDrxFriends.h>

class CUILobbyMP 
	: public IUIGameEventSystem
	, public IUIModule
{
public:
	CUILobbyMP();

	// IUIGameEventSystem
	UIEVENTSYSTEM( "UILobbyMP" );
	virtual void InitEventSystem() override;
	virtual void UnloadEventSystem() override;

	// IUIModule
	virtual void Reset() override;
	// ~IUIModule

	// UI functions
	void InviteAccepted();
	void SearchCompleted();
	void SearchStarted();
	void UpdateNatType();
	void ServerFound(SDrxSessionSearchResult session, string sServerName);
	void PlayerListReturn(const SUIArguments& players, const SUIArguments& playerids);
	void ReadLeaderBoard(int leaderboardIdx, int mode, int nrOfEntries);
	void WriteLeaderBoard(int leaderboardIdx, const char* values);
	void RegisterLeaderBoard(string leaderboardName, int leaderBoardIdx, int nrOfColumns);
	void ShowLoadingDialog(const  char* sLoadingDialog);
	void HideLoadingDialog(const  char* sLoadingDialog);
	void LeaderboardEntryReturn(const SUIArguments& customColumns);
	void InviteFriends(int containerIdx, int friendIdx);
	void SendUserStat(SUIArguments& arg);

	//Callback when session is found
	static void MatchmakingSessionSearchCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, SDrxSessionSearchResult* session, void* arg);
	static void ReadLeaderBoardCB(DrxLobbyTaskID TaskID, EDrxLobbyError Error, SDrxStatsLeaderBoardReadResult *Result, void *Arg);
	static void RegisterLeaderboardCB(DrxLobbyTaskID TaskID, EDrxLobbyError Error, void *Arg);
	static void WriteLeaderboardCallback(DrxLobbyTaskID TaskID, EDrxLobbyError Error, void *Arg);

	static void ReadUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, SDrxLobbyUserData* pData, uint32 numData, void* pArg);

	static void GetFriendsCB(DrxLobbyTaskID taskID, EDrxLobbyError error, SFriendInfo* pFriendInfo, uint32 numFriends, void* pArg);
	static void InviteFriendsCB(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);

private:
	// UI events
	void GetUserStats();
	void SearchGames(bool bLan);
	void AwardTrophy(int trophy);
	void JoinGame(unsigned int sessionID);
	void HostGame(bool bLan, string sMapPath, string sGameRules);
	void SetMultiplayer(bool bIsMultiplayer);
	void LockController( bool bLock );
	void LeaveGame();
	void GetPlayerList();
	void MutePlayer(int playerId, bool mute);
	void GetFriends(int containerIdx);

private:
	enum EUIEvent
	{
		eUIE_ServerFound = 0,
		eUIE_PlayerListReturn,
		eUIE_PlayerIdListReturn,
		eUIE_InviteAccepted,
		eUIE_SearchStarted,
		eUIE_SearchCompleted,
		eUIE_NatTypeUpdated,
		eUIE_ShowLoadingDialog,
		eUIE_HideLoadingDialog,
		eUIE_LeaderboardEntryReturn,
		eUIE_GetFriendsCompleted,
		eUIE_UserStatRead,
	};

	SUIEventReceiverDispatcher<CUILobbyMP> m_eventDispatcher;
	SUIEventSenderDispatcher<EUIEvent> m_eventSender;
	IUIEventSystem* m_pUIEvents;
	IUIEventSystem* m_pUIFunctions;
	std::vector<SDrxSessionSearchResult> m_FoundServers;
};


#endif // __UILOBBYMP_H_
