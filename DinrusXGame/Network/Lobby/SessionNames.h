// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Struct for keeping track of names in sessions
-------------------------------------------------------------------------
History:
- 15:03:2010 : Created By Ben Parbury

*************************************************************************/
#include <DinrusX/CoreX/String/StringUtils.h>
#include <DinrusX/CoreX/Containers/DrxFixedArray.h>

#include <IPlayerProfiles.h>

#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>
#include <DinrusX/DinrusXGame/GameRulesTypes.h>

#pragma once

#define CLAN_TAG_LENGTH		(5)
#define DISPLAY_NAME_LENGTH (DRXLOBBY_USER_NAME_LENGTH + CLAN_TAG_LENGTH + 1)	// +1 for space
#define MAX_ONLINE_STATS_SIZE (1500)
#define MAX_SESSION_NAMES (MAX_PLAYER_LIMIT)

// If you add or remove an enumerate element you should also:
// - Update the numbers so there are no jumps
// - Check that the number of elements doesn't exceed DRXLOBBY_USER_DATA_SIZE_IN_BYTES
enum ELocalUserData
{
	eLUD_SquadId1 = 0,
	eLUD_SquadId2 = 1,
	eLUD_SkillRank1 = 2,
	eLUD_SkillRank2 = 3,
	eLUD_ClanTag1 = 4,
	eLUD_ClanTag2 = 5,
	eLUD_ClanTag3 = 6,
	eLUD_ClanTag4 = 7,
	eLUD_LoadedDLCs = 8,
	eLUD_VoteChoice = 9,
};

struct SSessionNames
{
	struct SSessionName
	{
		static const int MUTE_REASON_WRONG_TEAM			= (1 << 0);
		static const int MUTE_REASON_MANUAL					= (1 << 1);
		static const int MUTE_REASON_NOT_IN_SQUAD		= (1 << 2);
		static const int MUTE_REASON_MUTE_ALL				= (1 << 3);

		static const int MUTE_PLAYER_AUTOMUTE_REASONS = (MUTE_REASON_NOT_IN_SQUAD | MUTE_REASON_MUTE_ALL); // Reasons controlled by the automute

		SSessionName(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated);
		void Set(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated);		
		void SetUserData(const uint8 *userData);
		void GetDisplayName(DrxFixedStringT<DISPLAY_NAME_LENGTH> &displayName) const;
		void GetClanTagName(DrxFixedStringT<CLAN_TAG_LENGTH> &name) const;
		uint16 GetSkillRank() const;

		//---------------------------------------
		DrxUserID m_userId;
		float m_timeWithoutConnection;
		SDrxMatchMakingConnectionUID m_conId;
		char m_name[DRXLOBBY_USER_NAME_LENGTH];
		uint8	m_userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
		uint8 m_teamId;
		uint8 m_rank;
		uint8 m_reincarnations;	// Not currently serialised (if needed add to eGUPD_SetTeamAndRank packet)
		uint8 m_muted;
		bool m_bMustLeaveBeforeServer;
		bool m_isDedicated;
		bool m_bFullyConnected;		// Server only flag used by the GameLobby to determine if players have finished identifying themselves
	};

	const static int k_unableToFind = -1;
	DrxFixedArray<SSessionName, MAX_SESSION_NAMES> m_sessionNames;
	bool m_dirty;		// So the UI knows it needs to be updated

	SSessionNames();
	unsigned int Size() const;
	void Clear();
	int Find(const SDrxMatchMakingConnectionUID &conId) const;
	int FindByUserId(const DrxUserID &userId) const;
	int FindIgnoringSID(const SDrxMatchMakingConnectionUID &conId) const;
	SSessionNames::SSessionName* GetSessionName(const SDrxMatchMakingConnectionUID &conId, bool ignoreSID);
	SSessionNames::SSessionName* GetSessionNameByUserId(const DrxUserID &userId);
	const SSessionNames::SSessionName* GetSessionNameByUserId(const DrxUserID &userId) const;
	int Insert(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated);
	void Remove(const SDrxMatchMakingConnectionUID &conId);
	void Update(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated, bool findByUserId);
	void RemoveBlankEntries();
	void Tick(float dt);
	bool RemoveEntryWithInvalidConnection();
};
