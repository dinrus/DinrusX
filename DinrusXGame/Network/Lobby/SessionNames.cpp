// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/SessionNames.h>
#include <DinrusX/DinrusXGame/GameCVars.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameLobbyUpr.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameLobbyCVars.h>
#include <DinrusX/DinrusXGame/PersistantStats.h>

//SSessionNames::SSessionName

SSessionNames::SSessionName::SSessionName(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated)
{
	m_teamId = 0;		// Will get set separately
	m_rank = 0;
	m_reincarnations = 0;
	m_muted = 0;
	m_bMustLeaveBeforeServer = false;
	m_bFullyConnected = false;
	Set(userId, conId, name, userData, isDedicated);
	m_timeWithoutConnection = 0.f;
}

//---------------------------------------
void SSessionNames::SSessionName::Set(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated)
{
	m_conId = conId;
	m_userId = userId;
	m_isDedicated = isDedicated;
	cry_strcpy(m_name, name);

	SetUserData(userData);
}

void SSessionNames::SSessionName::SetUserData(const uint8 *userData)
{
	if (userData)
	{
		memcpy(m_userData, userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
	}
	else
	{
		memset(m_userData, 0, sizeof(m_userData));
	}
}

//---------------------------------------
void SSessionNames::SSessionName::GetDisplayName(DrxFixedStringT<DISPLAY_NAME_LENGTH> &displayName) const
{
	DrxFixedStringT<CLAN_TAG_LENGTH> clanTag;

	GetClanTagName(clanTag);

	if (clanTag.size() > 0)
	{
		displayName.Format("%s %s", clanTag.c_str(), m_name);
	}
	else
	{
		displayName = m_name; // don't want the space if there's no clan tag
	}
}

//---------------------------------------
void SSessionNames::SSessionName::GetClanTagName(DrxFixedStringT<CLAN_TAG_LENGTH> &name) const
{
	char clanName[CLAN_TAG_LENGTH] = {0};

	CGameLobbyUpr* pGameLobbyUpr = g_pGame->GetGameLobbyUpr();
	IDrxLobby *pLobby = gEnv->pNetwork->GetLobby();
	IDrxLobbyService *pLobbyService = pLobby ? pLobby->GetLobbyService() : NULL;

	const bool chatRestricted = pGameLobbyUpr ? pGameLobbyUpr->IsChatRestricted() : false;
	const bool isLocalUser = pLobbyService ? (pLobbyService->GetUserID(g_pGame->GetExclusiveControllerDeviceIndex()) == m_userId) : true;

	if(!chatRestricted || isLocalUser)
	{
		clanName[0] = m_userData[eLUD_ClanTag1];
		clanName[1] = m_userData[eLUD_ClanTag2];
		clanName[2] = m_userData[eLUD_ClanTag3];
		clanName[3] = m_userData[eLUD_ClanTag4];
	}

	name = clanName;
}

//---------------------------------------
uint16 SSessionNames::SSessionName::GetSkillRank() const
{
	return m_userData[eLUD_SkillRank1] + (m_userData[eLUD_SkillRank2] << 8);
}

//~SSessionNames::SSessionName

//---------------------------------------
SSessionNames::SSessionNames() : m_dirty(false)
{
}

//---------------------------------------
unsigned int SSessionNames::Size() const
{
	return m_sessionNames.size();
}

void SSessionNames::Clear()
{
	m_dirty = true;
	m_sessionNames.clear();
}

//---------------------------------------
int SSessionNames::Find(const SDrxMatchMakingConnectionUID &conId) const
{
	const int namesCount = m_sessionNames.size();
	for(int i = 0; i < namesCount; i++)
	{
		if(m_sessionNames[i].m_conId == conId)
		{
			return i;
		}
	}

	return k_unableToFind;
}

//---------------------------------------
int SSessionNames::FindByUserId(const DrxUserID &userId) const
{
	if (userId.IsValid())
	{
		const int namesCount = m_sessionNames.size();
		for(int i = 0; i < namesCount; i++)
		{
			if (m_sessionNames[i].m_userId.IsValid() && (m_sessionNames[i].m_userId == userId))
			{
				return i;
			}
		}
	}

	return k_unableToFind;
}

//---------------------------------------
// Shouldn't be needed except that on a LIVE hostmigration the sid changes and we need to match the user
int SSessionNames::FindIgnoringSID(const SDrxMatchMakingConnectionUID &conId) const
{
	const int namesCount = m_sessionNames.size();
	for(int i = 0; i < namesCount; i++)
	{
		if(m_sessionNames[i].m_conId.m_uid == conId.m_uid)
		{
			return i;
		}
	}

	return k_unableToFind;
}

//---------------------------------------
SSessionNames::SSessionName* SSessionNames::GetSessionName(const SDrxMatchMakingConnectionUID &conId, bool ignoreSID)
{
	SSessionNames::SSessionName *pResult = NULL;

	int index = (ignoreSID ? FindIgnoringSID(conId) : Find(conId));
	if (index != SSessionNames::k_unableToFind)
	{
		pResult = &m_sessionNames[index];
	}

	return pResult;
}

//---------------------------------------
SSessionNames::SSessionName* SSessionNames::GetSessionNameByUserId(const DrxUserID &userId)
{
	SSessionNames::SSessionName *pResult = NULL;

	int index = FindByUserId(userId);
	if (index != SSessionNames::k_unableToFind)
	{
		pResult = &m_sessionNames[index];
	}

	return pResult;
}

const SSessionNames::SSessionName* SSessionNames::GetSessionNameByUserId(const DrxUserID &userId) const
{
	const SSessionNames::SSessionName *pResult = NULL;

	int index = FindByUserId(userId);
	if (index != SSessionNames::k_unableToFind)
	{
		pResult = &m_sessionNames[index];
	}

	return pResult;
}

//---------------------------------------
int SSessionNames::Insert(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated)
{
	m_dirty = true;
	DRX_ASSERT(Find(conId) == k_unableToFind);
	m_sessionNames.push_back(SSessionName(userId, conId, name, userData, isDedicated));
	return (m_sessionNames.size() - 1);
}

//---------------------------------------
void SSessionNames::Remove(const SDrxMatchMakingConnectionUID &conId)
{
	m_dirty = true;
	int index = Find(conId);
	DRX_ASSERT(index != k_unableToFind);
	m_sessionNames.removeAt(index);	//swaps last element into ith place
}

//---------------------------------------
void SSessionNames::Update(DrxUserID userId, const SDrxMatchMakingConnectionUID &conId, const char* name, const uint8* userData, bool isDedicated, bool findByUserId)
{
	m_dirty = true;
	int index = !findByUserId ? FindIgnoringSID(conId) : FindByUserId(userId);		// We can be told about SID changes here so we can't rely on it
	if(index == k_unableToFind)
	{
		Insert(userId, conId, name, userData, isDedicated);
	}
	else
	{
		SSessionName &entry = m_sessionNames[index];
		entry.Set(userId, conId, name, userData, isDedicated);
	}
}

//---------------------------------------
void SSessionNames::RemoveBlankEntries()
{
	uint32 numPlayers = m_sessionNames.size();
	for (uint32 i = 0; i < numPlayers; ++ i)
	{
		if (m_sessionNames[i].m_name[0] == 0)
		{
			m_sessionNames.removeAt(i);
			-- i;
			-- numPlayers;
		}
	}
}

void SSessionNames::Tick(float dt)
{
	CGameLobbyCVars *pGameLobbyCVars = CGameLobbyCVars::Get();
	const float maxSessionNameTimeWithoutConnection = pGameLobbyCVars->gl_maxSessionNameTimeWithoutConnection;
	uint32 numPlayers = m_sessionNames.size();
	
	for (uint32 i = 0; i < numPlayers; ++ i)
	{
		SSessionNames::SSessionName *pSessionName = &m_sessionNames[i];
		if (pSessionName->m_conId == DrxMatchMakingInvalidConnectionUID)
		{
			pSessionName->m_timeWithoutConnection += dt;

			if(pSessionName->m_timeWithoutConnection >= maxSessionNameTimeWithoutConnection)
			{
				DrxLog("[SessionNames] Removing user %s due to timeout %.2f", pSessionName->m_name, pSessionName->m_timeWithoutConnection);

				m_sessionNames.removeAt(i);
				m_dirty = true;
				-- i;
				-- numPlayers;

				continue; // probably not the best of ideas to keep going
			}
		}
		else
		{
			pSessionName->m_timeWithoutConnection = 0.f;
		}
	}
}

bool SSessionNames::RemoveEntryWithInvalidConnection()
{
	bool removed = false;
	uint32 numPlayers = m_sessionNames.size();
	for (uint32 i = 0; i < numPlayers; ++ i)
	{
		if (m_sessionNames[i].m_conId == DrxMatchMakingInvalidConnectionUID)
		{
			m_sessionNames.removeAt(i);
			removed = true;
			break;
		}
	}

	return removed;
}