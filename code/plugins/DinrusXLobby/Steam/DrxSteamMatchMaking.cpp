// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamMatchMaking.cpp
//  Created:     11/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxMatchMaking
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "Steam/DrxSteamLobby.h" // DrxSteamMatchMaking.h included here

#if USE_STEAM && USE_DRX_MATCHMAKING

	#include "DrxSharedLobbyPacket.h"

////////////////////////////////////////////////////////////////////////////////

// Session create task params
	#define SESSION_CREATE_PARAM_SESSION_DATA 0       // ptr
	#define SESSION_CREATE_PARAM_USER_DATA    1       // ptr

// Session update task params
	#define SESSION_UPDATE_PARAM_USER_DATA     0      // ptr
	#define SESSION_UPDATE_PARAM_NUM_USER_DATA 0      // int

// Session update slots task params
	#define SESSION_UPDATE_SLOTS_PARAM_NUM_PUBLIC  0  // int
	#define SESSION_UPDATE_SLOTS_PARAM_NUM_PRIVATE 1  // int

// Session search task params
	#define SESSION_SEARCH_PARAM_GAME_PARAM     0     // ptr
	#define SESSION_SEARCH_PARAM_GAME_USER_DATA 1     // ptr

////////////////////////////////////////////////////////////////////////////////

	#define SESSION_SEARCH_TIMEOUT (10000)
	#define SESSION_JOIN_TIMEOUT   (10000)

////////////////////////////////////////////////////////////////////////////////

	#define STEAM_GAME_TYPE_RANKED   (1)
	#define STEAM_GAME_TYPE_UNRANKED (0)

// keys
	#define MAX_STEAM_KEY_VALUE_SIZE        (256)
	#define STEAM_KEY_SESSION_NAME          "session_name"
	#define STEAM_KEY_SESSION_GAME_TYPE     "game_type"
	#define STEAM_KEY_SESSION_PUBLIC_SLOTS  "public_slots"
	#define STEAM_KEY_SESSION_PRIVATE_SLOTS "private_slots"
	#define STEAM_KEY_SESSION_USER_DATA     "user_key_"

////////////////////////////////////////////////////////////////////////////////

CDrxSteamMatchMaking::CDrxSteamMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, EDrxLobbyService serviceType)
	: CDrxMatchMaking(lobby, service, serviceType)
	//////////////////////////////////////////////////////////////////////////////
	// Steam callback registration
	, m_SteamOnShutdown(this, &CDrxSteamMatchMaking::OnSteamShutdown)
	, m_SteamOnLobbyDataUpdated(this, &CDrxSteamMatchMaking::OnLobbyDataUpdated)
	, m_SteamOnP2PSessionRequest(this, &CDrxSteamMatchMaking::OnP2PSessionRequest)
	, m_SteamOnLobbyChatUpdate(this, &CDrxSteamMatchMaking::OnLobbyChatUpdate)
	//////////////////////////////////////////////////////////////////////////////
{
	// Make the CDrxMatchMaking base pointers point to our data so we can use the common code in CDrxMatchMaking
	for (uint32 i = 0; i < MAX_MATCHMAKING_SESSIONS; i++)
	{
		CDrxMatchMaking::m_sessions[i] = &m_sessions[i];
		CDrxMatchMaking::m_sessions[i]->localConnection = &m_sessions[i].localConnection;

		for (uint32 j = 0; j < MAX_LOBBY_CONNECTIONS; j++)
		{
			CDrxMatchMaking::m_sessions[i]->remoteConnection[j] = &m_sessions[i].remoteConnection[j];
		}
	}

	for (uint32 i = 0; i < MAX_MATCHMAKING_TASKS; i++)
	{
		CDrxMatchMaking::m_task[i] = &m_task[i];
	}
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::Initialise()
{
	EDrxLobbyError error = CDrxMatchMaking::Initialise();

	return error;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::Tick(CTimeValue tv)
{
	CDrxMatchMaking::Tick(tv);

	for (uint32 i = 0; i < MAX_MATCHMAKING_TASKS; i++)
	{
		STask* pTask = &m_task[i];

	#if ENABLE_DRXLOBBY_DEBUG_TESTS
		if (pTask->used)
		{
			if (m_lobby->DebugTickCallStartTaskRunning(pTask->lTaskID))
			{
				StartTaskRunning(i);
				continue;
			}
			if (!m_lobby->DebugOKToTickTask(pTask->lTaskID, pTask->running))
			{
				continue;
			}
		}
	#endif

		if ((pTask->used) && (pTask->running) && (CheckTaskStillValid(i)))
		{
			switch (pTask->subTask)
			{
			case eT_SessionMigrate:
				TickSessionMigrate(i);
				break;

			case eT_SessionSearch:
				TickSessionSearch(i);
				break;

			case eT_SessionJoin:
				TickSessionJoin(i);
				break;

			//#if NETWORK_HOST_MIGRATION
			//			case eT_SessionMigrateHostStart:
			//				TickHostMigrationStartNT(i);
			//				break;
			//
			//			case eT_SessionMigrateHostServerWaitStart:
			//				TickHostMigrationServerWaitStartNT(i);
			//				break;
			//
			//			case eT_SessionMigrateHostServer:
			//				TickHostMigrationServerNT(i);
			//				break;
			//
			//			case eT_SessionMigrateHostClientWaitStart:
			//				TickHostMigrationClientWaitStartNT(i);
			//				break;
			//
			//			case eT_SessionMigrateHostClient:
			//				TickHostMigrationClientNT(i);
			//				break;
			//
			//			case eT_SessionMigrateHostFinish:
			//				TickHostMigrationFinishNT(i);
			//				break;
			//#endif

			default:
				TickBaseTask(i);
				break;
			}
		}

		if ((pTask->canceled) || (pTask->error != eCLE_Success))
		{
			StopTaskRunning(i);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket)
{
	CDrxSharedLobbyPacket* pSharedPacket = (CDrxSharedLobbyPacket*)pPacket;

	uint32 type = pSharedPacket->StartRead();
	switch (type)
	{
	case eSteamPT_SessionRequestJoinResult:
		ProcessSessionRequestJoinResult(addr, pSharedPacket);
		break;

	case eSteamPT_UserData:
		ProcessLocalUserData(addr, pSharedPacket);
		break;

	default:
		CDrxMatchMaking::OnPacket(addr, pSharedPacket);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////

/*
   void						ProfileSettingsChanged(ULONG_PTR param);
   void						UserSignedOut(uint32 user);
 */

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionRegisterUserData(SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	if (numData < MAX_MATCHMAKING_SESSION_USER_DATA)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionRegisterUserData, false, &mmTaskID, taskID, DrxLobbyInvalidSessionHandle, (void*)cb, cbArg);

		if (error == eCLE_Success)
		{
			for (int i = 0; i < numData; i++)
			{
				m_registeredUserData.data[i] = data[i];
			}
			m_registeredUserData.num = numData;

			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_OutOfSessionUserData;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionRegisterUserData error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionCreate(uint32* users, int numUsers, uint32 flags, SDrxSessionData* data, DrxLobbyTaskID* taskID, DrxMatchmakingSessionCreateCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;
	EDrxLobbyError error = CreateSessionHandle(&h, true, flags, numUsers);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionCreate, false, &mmTaskID, taskID, h, cb, cbArg);

		if (error == eCLE_Success)
		{
			STask* pTask = &m_task[mmTaskID];
			SSession* pSession = &m_sessions[h];

			if (data->m_ranked)
			{
				pSession->gameType = STEAM_GAME_TYPE_RANKED;
			}
			else
			{
				pSession->gameType = STEAM_GAME_TYPE_UNRANKED;
			}
			pSession->gameMode = 0;

			pSession->data.m_numPublicSlots = data->m_numPublicSlots;
			pSession->data.m_numPrivateSlots = data->m_numPrivateSlots;
			pSession->data.m_ranked = data->m_ranked;

			NetLog("[MatchMaking] Created local connection " PRFORMAT_SH " " PRFORMAT_UID, PRARG_SH(h), PRARG_UID(pSession->localConnection.uid));

			error = CreateTaskParam(mmTaskID, SESSION_CREATE_PARAM_SESSION_DATA, data, 1, sizeof(SDrxSessionData));

			if (error == eCLE_Success)
			{
				error = CreateTaskParam(mmTaskID, SESSION_CREATE_PARAM_USER_DATA, data->m_data, data->m_numData, data->m_numData * sizeof(data->m_data[0]));

				if (error == eCLE_Success)
				{
					FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
				}
			}

			if (error != eCLE_Success)
			{
				FreeTask(mmTaskID);
				FreeSessionHandle(h);
			}
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionCreate error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionMigrate(DrxSessionHandle h, uint32* pUsers, int numUsers, uint32 flags, SDrxSessionData* pData, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionCreateCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;

	NetLog("[TODO] CDrxSteamMatchMaking::SessionMigrate()");
	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionMigrate error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionUpdate(DrxSessionHandle h, SDrxSessionUserData* data, uint32 numData, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			DrxMatchMakingTaskID mmTaskID;
			error = StartTask(eT_SessionUpdate, false, &mmTaskID, taskID, lsh, cb, cbArg);

			if (error == eCLE_Success)
			{
				STask* task = &m_task[mmTaskID];
				error = CreateTaskParam(mmTaskID, SESSION_UPDATE_PARAM_USER_DATA, data, numData, numData * sizeof(data[0]));

				if (error == eCLE_Success)
				{
					FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
				}
				else
				{
					FreeTask(mmTaskID);
				}
			}
		}
		else
		{
			error = eCLE_InvalidRequest;
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionUpdate error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionUpdateSlots(DrxSessionHandle h, uint32 numPublic, uint32 numPrivate, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			DrxMatchMakingTaskID mmTaskID;
			error = StartTask(eT_SessionUpdateSlots, false, &mmTaskID, pTaskID, lsh, pCB, pCBArg);

			if (error == eCLE_Success)
			{
				STask* task = &m_task[mmTaskID];

				CreateTaskParam(mmTaskID, SESSION_UPDATE_SLOTS_PARAM_NUM_PUBLIC, NULL, numPublic, 0);
				CreateTaskParam(mmTaskID, SESSION_UPDATE_SLOTS_PARAM_NUM_PRIVATE, NULL, numPrivate, 0);
				FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}
		else
		{
			error = eCLE_InvalidRequest;
		}
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionUpdateSlots return %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionQuery(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionQueryCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionQuery, false, &mmTaskID, pTaskID, lsh, (void*)pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionQuery error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionGetUsers(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionGetUsersCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		SSession* pSession = &m_sessions[lsh];
		DrxMatchMakingTaskID tid;

		error = StartTask(eT_SessionGetUsers, false, &tid, pTaskID, lsh, pCB, pCBArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, tid);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionGetUsers error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionStart(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionStart, false, &mmTaskID, taskID, lsh, cb, cbArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags |= DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionStart error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionEnd(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionEnd, false, &mmTaskID, taskID, lsh, cb, cbArg);

		if (error == eCLE_Success)
		{
			pSession->localFlags &= ~DRXSESSION_LOCAL_FLAG_STARTED;
			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionEnd error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionDelete(DrxSessionHandle h, DrxLobbyTaskID* taskID, DrxMatchmakingCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SSession* pSession = &m_sessions[lsh];
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionDelete, false, &mmTaskID, taskID, lsh, cb, cbArg);

		if (error == eCLE_Success)
		{
			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
	}
	else
	{
		error = eCLE_SuccessInvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionDelete error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionSearch(uint32 user, SDrxSessionSearchParam* param, DrxLobbyTaskID* taskID, DrxMatchmakingSessionSearchCallback cb, void* cbArg)
{
	LOBBY_AUTO_LOCK;

	DrxMatchMakingTaskID mmTaskID;

	EDrxLobbyError error = StartTask(eT_SessionSearch, false, &mmTaskID, taskID, DrxLobbyInvalidSessionHandle, cb, cbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[mmTaskID];

		error = CreateTaskParam(mmTaskID, SESSION_SEARCH_PARAM_GAME_PARAM, param, 1, sizeof(SDrxSessionSearchParam));

		if (error == eCLE_Success)
		{
			error = CreateTaskParam(mmTaskID, SESSION_SEARCH_PARAM_GAME_USER_DATA, param->m_data, param->m_numData, param->m_numData * sizeof(param->m_data[0]));

			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}

		if (error != eCLE_Success)
		{
			FreeTask(mmTaskID);
		}
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionSearch error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionJoin(uint32* pUsers, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionJoinCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle h;
	EDrxLobbyError error = CreateSessionHandle(&h, false, flags, numUsers);

	if (error == eCLE_Success)
	{
		DrxMatchMakingTaskID mmTaskID;

		error = StartTask(eT_SessionJoin, false, &mmTaskID, pTaskID, h, pCB, pCBArg);
		if (error == eCLE_Success)
		{
			SDrxSteamSessionID* pSteamID = (SDrxSteamSessionID*)id.get();
			SSession* pSession = &m_sessions[h];

			pSession->m_id.m_steamID = pSteamID->m_steamID;

			FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
		}
		else
		{
			FreeSessionHandle(h);
		}
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionJoin error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionSetLocalUserData(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg)
{
	LOBBY_AUTO_LOCK;

	EDrxLobbyError error = eCLE_Success;
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		if (dataSize <= DRXLOBBY_USER_DATA_SIZE_IN_BYTES)
		{
			SSession* pSession = &m_sessions[lsh];
			SSession::SLConnection* pLConnection = &pSession->localConnection;
			DrxMatchMakingTaskID mmTaskID;

			memcpy(pLConnection->userData, pData, dataSize);

			error = StartTask(eT_SessionSetLocalUserData, false, &mmTaskID, pTaskID, lsh, (void*)pCB, pCBArg);

			if (error == eCLE_Success)
			{
				FROM_GAME_TO_LOBBY(&CDrxSteamMatchMaking::StartTaskRunning, this, mmTaskID);
			}
		}
		else
		{
			error = eCLE_OutOfUserData;
		}
	}
	else
	{
		error = eCLE_InvalidSession;
	}

	if (error != eCLE_Success)
	{
		NetLog("[MatchMaking] Start SessionSetLocalUserData error %d", error);
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

DrxSessionID CDrxSteamMatchMaking::SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h)
{
	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (m_sessions[lsh].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		SDrxSteamSessionID* pID = new SDrxSteamSessionID(m_sessions[lsh].m_id.m_steamID, false);

		if (pID)
		{
			return pID;
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

uint32 CDrxSteamMatchMaking::GetSessionIDSizeInPacket() const
{
	return SDrxSteamSessionID::GetSizeInPacket();
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const
{
	EDrxLobbyError result = eCLE_InternalError;

	if (pPacket != NULL)
	{
		SDrxSteamSessionID blank;
		SDrxSteamSessionID* pID = &blank;

		if (sessionId != DrxSessionInvalidID)
		{
			pID = (SDrxSteamSessionID*)sessionId.get();
			if (pID == NULL)
			{
				pID = &blank;
			}
		}

		pID->WriteToPacket(pPacket);
		result = eCLE_Success;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////

DrxSessionID CDrxSteamMatchMaking::ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const
{
	DrxSessionID resultID = DrxSessionInvalidID;

	if (pPacket != NULL)
	{
		SDrxSteamSessionID* pID = new SDrxSteamSessionID();
		if (pID != NULL)
		{
			pID->ReadFromPacket(pPacket);
			resultID = pID;
		}
	}

	return resultID;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg)
{
	EDrxLobbyError rc = eCLE_Success;

	#if NETWORK_HOST_MIGRATION
	LOBBY_AUTO_LOCK;
	#endif // NETWORK_HOST_MIGRATION

	NetLog("[TODO] CDrxSteamMatchMaking::SessionEnsureBestHost()");
	return rc;
}

////////////////////////////////////////////////////////////////////////////////

	#if NETWORK_HOST_MIGRATION
void CDrxSteamMatchMaking::HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause)
{
	NetLog("[TODO] CDrxSteamMatchMaking::HostMigrationInitialise()");
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::HostMigrationServer(SHostMigrationInfo* pInfo)
{
	NetLog("[TODO] CDrxSteamMatchMaking::HostMigrationServer()");
	return eCLE_InvalidSession;
}

////////////////////////////////////////////////////////////////////////////////

bool CDrxSteamMatchMaking::GetNewHostAddress(char* address, SHostMigrationInfo* pInfo)
{
	NetLog("[TODO] CDrxSteamMatchMaking::GetNewHostAddress()");
	return true;
}

	#endif // NETWORK_HOST_MIGRATION

////////////////////////////////////////////////////////////////////////////////

SDrxSteamSessionID CDrxSteamMatchMaking::GetSteamSessionIDFromSession(DrxLobbySessionHandle lsh)
{
	if (lsh != DrxLobbyInvalidSessionHandle)
	{
		SSession* pSession = &m_sessions[lsh];
		return pSession->m_id;
	}

	return SDrxSteamSessionID();
}

////////////////////////////////////////////////////////////////////////////////

TNetAddress CDrxSteamMatchMaking::GetHostAddressFromSessionHandle(DrxSessionHandle h)
{
	LOBBY_AUTO_LOCK;

	DrxLobbySessionHandle lsh = GetSessionHandleFromGameSessionHandle(h);
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (lsh != DrxLobbyInvalidSessionHandle && pSteamMatchmaking)
	{
		SSession* pSession = &m_sessions[lsh];
		SDrxSteamUserID host;

		// Is there a game server?
		if (!(pSteamMatchmaking->GetLobbyGameServer(pSession->m_id.m_steamID, NULL, NULL, &host.m_steamID)))
		{
			// If not, use the lobby owner as host...
			host = pSteamMatchmaking->GetLobbyOwner(pSession->m_id.m_steamID);
		}

		if (!(host == SDrxSteamUserID()))
		{
			if (host == pSession->localConnection.userID)
			{
				NetLog("[STEAM]: GetHostAddressFromSessionHandle(): I am the host");
				return TNetAddress(TLocalNetAddress(m_lobby->GetInternalSocketPort(eCLS_Online)));
			}

			NetLog("[STEAM]: GetHostAddressFromSessionHandle(): host is [%s]", host.GetGUIDAsString().c_str());
			return TNetAddress(LobbyIdAddr(host.m_steamID.ConvertToUint64()));
		}

		NetLog("[STEAM]: Unable to retrieve host from Steam lobby (session id [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());
	}

	return TNetAddress(SNullAddr());
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::LobbyAddrIDTick()
{
	// Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

bool CDrxSteamMatchMaking::LobbyAddrIDHasPendingData()
{
	bool pendingData = false;

	if (ISteamNetworking* pSteamNetworking = SteamNetworking())
	{
		// We don't use size since we only send/receive up to MAX_UDP_PACKET_SIZE
		uint32 size = 0;
		pendingData = pSteamNetworking->IsP2PPacketAvailable(&size);
	}

	return pendingData;
}

////////////////////////////////////////////////////////////////////////////////

ESocketError CDrxSteamMatchMaking::LobbyAddrIDSend(const uint8* buffer, uint32 size, const TNetAddress& addr)
{
	auto lobbyIdAddr = stl::get<LobbyIdAddr>(addr);
	CSteamID steamID(lobbyIdAddr.id);
	ISteamNetworking* pSteamNetworking = SteamNetworking();
	if (!pSteamNetworking)
	{
		return eSE_MiscFatalError;
	}

	bool sent = pSteamNetworking->SendP2PPacket(steamID, buffer, size, k_EP2PSendUnreliable);
	if (sent)
	{
		return eSE_Ok;
	}

	NetLog("[STEAM]: LobbyAddrIDSend() unreachable address");
	return eSE_UnreachableAddress;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::LobbyAddrIDRecv(void (* cb)(void*, uint8* buffer, uint32 size, DRXSOCKET sock, TNetAddress& addr), void* cbData)
{
	static uint8 s_recvBuffer[MAX_UDP_PACKET_SIZE];
	static TNetAddress s_recvAddr;
	static uint32 s_recvSize;
	static CSteamID sender;
	ISteamNetworking* pSteamNetworking = SteamNetworking();
	if (!pSteamNetworking)
	{
		return;
	}

	while (LobbyAddrIDHasPendingData())
	{
		if (pSteamNetworking->ReadP2PPacket(s_recvBuffer, MAX_UDP_PACKET_SIZE, &s_recvSize, &sender))
		{
			CDrxLobby::SSocketService* pService = m_lobby->GetCorrectSocketService(eCLS_Online);
			if ((pService != NULL) && (pService->m_socket != NULL))
			{
				s_recvAddr = TNetAddress(LobbyIdAddr(sender.ConvertToUint64()));
				cb(cbData, s_recvBuffer, s_recvSize, pService->m_socket->GetSysSocket(), s_recvAddr);
			}
			else
			{
				NetLog("[STEAM]: receiving Steam traffic before online service initialised");
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

const char* CDrxSteamMatchMaking::SSession::GetLocalUserName(uint32 localUserIndex) const
{
	if (localFlags & DRXSESSION_LOCAL_FLAG_USED)
	{
		return localConnection.name;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::SSession::Reset()
{
	PARENT::Reset();

	hostConnectionID = DrxMatchMakingInvalidConnectionID;
	m_id = SDrxSteamSessionID();
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::SSession::InitialiseLocalConnection(SDrxMatchMakingConnectionUID uid)
{
	localConnection.pingToServer = DRXLOBBY_INVALID_PING;
	localConnection.used = true;
	ISteamUser* pSteamUser = SteamUser();
	ISteamFriends* pSteamFriends = SteamFriends();
	if (pSteamUser && pSteamFriends)
	{
		localConnection.userID = pSteamUser->GetSteamID();
		drx_strcpy(localConnection.name, pSteamFriends->GetPersonaName());
	}
	else
	{
		localConnection.userID = k_steamIDNil;
		drx_strcpy(localConnection.name, "");
	}
	localConnection.uid = uid;
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32 createFlags, int numUsers)
{
	EDrxLobbyError error = CDrxMatchMaking::CreateSessionHandle(h, host, createFlags, numUsers);

	if (error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[*h];

		memset(pSession->localConnection.userData, 0, sizeof(pSession->localConnection.userData));
		pSession->data.m_data = pSession->userData;
		pSession->data.m_numData = m_registeredUserData.num;

		for (uint32 i = 0; i < m_registeredUserData.num; i++)
		{
			pSession->data.m_data[i] = m_registeredUserData.data[i];
		}
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

uint64 CDrxSteamMatchMaking::GetSIDFromSessionHandle(DrxLobbySessionHandle h)
{
	DRX_ASSERT_MESSAGE((h < MAX_MATCHMAKING_SESSIONS) && (m_sessions[h].localFlags & DRXSESSION_LOCAL_FLAG_USED), "CDrxSteamMatchMaking::GetSIDFromSessionHandle: invalid session handle");
	return m_sessions[h].m_id.m_steamID.ConvertToUint64();
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::SetSessionUserData(DrxMatchMakingTaskID mmTaskID, SDrxSessionUserData* pData, uint32 numData)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();

	if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USED && pSteamMatchmaking)
	{
		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> key;
			DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> value;

			for (uint32 index = 0; index < numData; ++index)
			{
				key.Format(STEAM_KEY_SESSION_USER_DATA "%d", index);

				switch (pData[index].m_type)
				{
				case eCLUDT_Int64:
					value.Format("%x:%x:%" PRIx64, pData[index].m_type, pData[index].m_id, pData[index].m_int64);
					break;
				case eCLUDT_Int32:
					value.Format("%x:%x:%x", pData[index].m_type, pData[index].m_id, pData[index].m_int32);
					break;
				case eCLUDT_Int16:
					value.Format("%x:%x:%x", pData[index].m_type, pData[index].m_id, pData[index].m_int16);
					break;
				case eCLUDT_Int8:
					value.Format("%x:%x:%x", pData[index].m_type, pData[index].m_id, pData[index].m_int8);
					break;
				case eCLUDT_Float64:
					value.Format("%x:%x:%f", pData[index].m_type, pData[index].m_id, pData[index].m_f64);
					break;
				case eCLUDT_Float32:
					value.Format("%x:%x:%f", pData[index].m_type, pData[index].m_id, pData[index].m_f32);
					break;
				}

				pSteamMatchmaking->SetLobbyData(pSession->m_id.m_steamID, key.c_str(), value.c_str());
			}
		}
		else
		{
			UpdateTaskError(mmTaskID, eCLE_InvalidRequest);
		}
	}
	else
	{
		UpdateTaskError(mmTaskID, eCLE_InvalidSession);
	}
}

////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::StartTask(ETask etask, bool startRunning, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg)
{
	DrxMatchMakingTaskID tmpMMTaskID;
	DrxMatchMakingTaskID* useMMTaskID = mmTaskID ? mmTaskID : &tmpMMTaskID;
	EDrxLobbyError error = CDrxMatchMaking::StartTask(etask, startRunning, useMMTaskID, lTaskID, h, cb, cbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[*useMMTaskID];
		pTask->Reset();
	}

	return error;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSubTask(ETask etask, DrxMatchMakingTaskID mmTaskID)
{
	CDrxMatchMaking::StartSubTask(etask, mmTaskID);

	STask* pTask = &m_task[mmTaskID];
	pTask->Reset();
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
		if (CheckTaskStillValid(mmTaskID))
		{
	#if ENABLE_DRXLOBBY_DEBUG_TESTS
			EDrxLobbyError error;

			if (!m_lobby->DebugOKToStartTaskRunning(pTask->lTaskID))
			{
				return;
			}

			if (m_lobby->DebugGenerateError(pTask->lTaskID, error))
			{
				UpdateTaskError(mmTaskID, error);
				StopTaskRunning(mmTaskID);

				if ((pTask->startedTask == eT_SessionCreate) || (pTask->startedTask == eT_SessionJoin))
				{
					FreeSessionHandle(pTask->session);
				}

				return;
			}
	#endif

			pTask->running = true;

			switch (pTask->startedTask)
			{
			case eT_SessionRegisterUserData:
			case eT_SessionGetUsers:
			case eT_SessionQuery:
			case eT_SessionStart:
			case eT_SessionEnd:
				StopTaskRunning(mmTaskID);
				break;

			case eT_SessionCreate:
				StartSessionCreate(mmTaskID);
				break;

			case eT_SessionMigrate:
				StartSessionMigrate(mmTaskID);
				break;

			case eT_SessionUpdate:
				StartSessionUpdate(mmTaskID);
				break;

			case eT_SessionUpdateSlots:
				StartSessionUpdateSlots(mmTaskID);
				break;

			case eT_SessionDelete:
				StartSessionDelete(mmTaskID);
				break;

			case eT_SessionSearch:
				StartSessionSearch(mmTaskID);
				break;

			case eT_SessionJoin:
				StartSessionJoin(mmTaskID);
				break;

			//#if NETWORK_HOST_MIGRATION
			//			case eT_SessionMigrateHostStart:
			//				HostMigrationStartNT(mmTaskID);
			//				break;
			//
			//			case eT_SessionMigrateHostServer:
			//				HostMigrationServerNT(mmTaskID);
			//				break;
			//
			//			case eT_SessionMigrateHostClient:
			//				TickHostMigrationClientNT(mmTaskID);
			//				break;
			//#endif
			//
			case eT_SessionSetLocalUserData:
				StartSessionSetLocalUserData(mmTaskID);
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::EndTask(DrxMatchMakingTaskID mmTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
	#if NETWORK_HOST_MIGRATION
		if (pTask->startedTask & HOST_MIGRATION_TASK_ID_FLAG)
		{
			m_sessions[pTask->session].hostMigrationInfo.m_taskID = DrxLobbyInvalidTaskID;
		}
	#endif

		if (pTask->cb)
		{
			switch (pTask->startedTask)
			{
			case eT_SessionRegisterUserData:
				((DrxMatchmakingCallback)pTask->cb)(pTask->lTaskID, pTask->error, pTask->cbArg);
				break;

			case eT_SessionUpdate:
			case eT_SessionUpdateSlots:
			case eT_SessionStart:
			case eT_SessionEnd:
			case eT_SessionDelete:
			case eT_SessionSetLocalUserData:
	#if NETWORK_HOST_MIGRATION
				//case eT_SessionMigrateHostStart:
	#endif
				((DrxMatchmakingCallback)pTask->cb)(pTask->lTaskID, pTask->error, pTask->cbArg);
				break;

			case eT_SessionGetUsers:
				EndSessionGetUsers(mmTaskID);
				break;

			case eT_SessionMigrate:                                                           // From the games point of view a migrate is the same as a create
				((DrxMatchmakingSessionCreateCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, m_sessions[pTask->session].localConnection.uid), pTask->cbArg);
				break;

			case eT_SessionCreate:
				((DrxMatchmakingSessionCreateCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, m_sessions[pTask->session].localConnection.uid), pTask->cbArg);

				if (pTask->error == eCLE_Success)
				{
					InitialUserDataEvent(pTask->session);
				}
				break;

			case eT_SessionSearch:
				EndSessionSearch(mmTaskID);
				break;

			case eT_SessionJoin:
				EndSessionJoin(mmTaskID);
				break;

			case eT_SessionQuery:
				EndSessionQuery(mmTaskID);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[MatchMaking] EndTask %d (%d) error %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		// Clear Steam specific task state so that base class tasks can use this slot
		pTask->Reset();

		FreeTask(mmTaskID);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StopTaskRunning(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->used)
	{
		pTask->running = false;
		TO_GAME_FROM_LOBBY(&CDrxSteamMatchMaking::EndTask, this, mmTaskID);
	}
}

////////////////////////////////////////////////////////////////////////////////

bool CDrxSteamMatchMaking::CheckTaskStillValid(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->session == DrxLobbyInvalidSessionHandle)
	{
		// If the task isn't using a session continue task.
		return true;
	}

	if ((pTask->session < MAX_MATCHMAKING_SESSIONS) && (m_sessions[pTask->session].localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		// If the task is using a session and the session is still in use continue task.
		return true;
	}
	else
	{
		// If task is using a session that has since been deleted terminate task.
		UpdateTaskError(mmTaskID, eCLE_InvalidSession);
		StopTaskRunning(mmTaskID);

		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////

DrxMatchMakingConnectionID CDrxSteamMatchMaking::AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, CSteamID steamID, uint32 numUsers, const char* name, uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES], bool isDedicated)
{
	SSession* pSession = &m_sessions[h];
	const bool bNewPlayerIsHost = (pSession->localConnection.userID.m_steamID == steamID);

	if (connectionID == DrxLobbyInvalidConnectionID)
	{
		TNetAddress addr(LobbyIdAddr(steamID.ConvertToUint64()));

		connectionID = m_lobby->FindConnection(addr);
		if (connectionID == DrxLobbyInvalidConnectionID)
		{
			connectionID = m_lobby->CreateConnection(addr);
		}
	}

	#if USE_DRX_VOICE && USE_STEAM_VOICE
	// Register the connection to DrxVoice
	SDrxSteamUserID* pUser = new SDrxSteamUserID(steamID);
	DrxUserID userId(pUser);
	CDrxSteamVoice* voice = (CDrxSteamVoice*)m_pService->GetVoice();
	NetLog("[Steam] Registering user to voice from AddRemoteConnection");
	voice->RegisterRemoteUser(connectionID, userId);
	#endif // USE_DRX_VOICE

	DrxMatchMakingConnectionID id = CDrxMatchMaking::AddRemoteConnection(h, connectionID, uid, numUsers);
	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

		pConnection->userID = SDrxSteamUserID(steamID);
		drx_strcpy(pConnection->name, name);
		memcpy(pConnection->userData, userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
		pConnection->m_isDedicated = isDedicated;
	#if NETWORK_HOST_MIGRATION
		pConnection->m_migrated = (pSession->hostMigrationInfo.m_state != eHMS_Idle);
	#endif // NETWORK_HOST_MIGRATION

		if (bNewPlayerIsHost)
		{
			pSession->hostConnectionID = id;
		}
	}

	NetLog("[STEAM]: CDrxSteamMatchMaking::AddRemoteConnection(), session %d, connection %d", h, id);
	return id;
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	if (id != DrxMatchMakingInvalidConnectionID)
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pConnection = &pSession->remoteConnection[id];
		ISteamNetworking* pSteamNetworking = SteamNetworking();

		if (pSteamNetworking && pConnection->used)
		{
			pSteamNetworking->CloseP2PSessionWithUser(pConnection->userID.m_steamID);
			CDrxMatchMaking::FreeRemoteConnection(h, id);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionCreate(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	SDrxSessionData* pData = (SDrxSessionData*)m_lobby->MemGetPtr(pTask->params[SESSION_CREATE_PARAM_SESSION_DATA]);
	pData->m_data = (SDrxSessionUserData*)m_lobby->MemGetPtr(pTask->params[SESSION_CREATE_PARAM_USER_DATA]);

	if (m_SteamOnLobbyCreated.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		m_SteamOnLobbyCreated.m_callResult.Cancel();
		StopTaskRunning(m_SteamOnLobbyCreated.m_taskID);
		NetLog("[MatchMaking]: StartSessionCreate() - cancelled previous session create");
	}

	ELobbyType lobbyType = k_ELobbyTypePrivate;
	if (pSession->createFlags & DRXSESSION_CREATE_FLAG_SEARCHABLE)
	{
		lobbyType = k_ELobbyTypePublic;
	}

	ISteamMatchmaking* matchMaking = SteamMatchmaking();
	SteamAPICall_t hSteamAPICall = k_uAPICallInvalid;
	if (matchMaking)
	{
		hSteamAPICall = matchMaking->CreateLobby(lobbyType, pSession->data.m_numPublicSlots + pSession->data.m_numPrivateSlots);
	}
	if (hSteamAPICall == k_uAPICallInvalid)
	{
		UpdateTaskError(mmTaskID, eCLE_InternalError);
		StopTaskRunning(mmTaskID);
	}
	else
	{
		m_SteamOnLobbyCreated.m_callResult.Set(hSteamAPICall, this, &CDrxSteamMatchMaking::OnLobbyCreated);
		m_SteamOnLobbyCreated.m_taskID = mmTaskID;
		NetLog("[MatchMaking]: StartSessionCreate()");
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionMigrate(DrxMatchMakingTaskID mmTaskID)
{
	NetLog("[TODO] CDrxSteamMatchMaking::StartSessionMigrate()");
	NetLog("[MatchMaking]: StartSessionMigrate()");
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::TickSessionMigrate(DrxMatchMakingTaskID mmTaskID)
{
	NetLog("[TODO] CDrxSteamMatchMaking::TickSessionMigrate()");
	NetLog("[MatchMaking]: TickSessionMigrate()");
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SDrxSessionSearchParam* pSessionSearchParam = (SDrxSessionSearchParam*)m_lobby->MemGetPtr(pTask->params[SESSION_SEARCH_PARAM_GAME_PARAM]);
	pSessionSearchParam->m_data = (SDrxSessionSearchData*)m_lobby->MemGetPtr(pTask->params[SESSION_SEARCH_PARAM_GAME_USER_DATA]);

	if (m_SteamOnLobbySearchResults.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		m_SteamOnLobbySearchResults.m_callResult.Cancel();
		StopTaskRunning(m_SteamOnLobbySearchResults.m_taskID);
		NetLog("[MatchMaking]: StartSessionSearch() - cancelled previous session search");
	}

	// Set filters here using:
	// AddRequestLobbyStringFilter()
	// AddRequestLobbyNumericalFilter()
	// AddRequestNearValueFilter()
	// AddRequestLobbyListFilterSlotsAvailable())

	
	SteamAPICall_t hSteamAPICall = k_uAPICallInvalid;
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (pSteamMatchmaking)
	{
		hSteamAPICall = pSteamMatchmaking->RequestLobbyList();
	}
	if (hSteamAPICall == k_uAPICallInvalid)
	{
		UpdateTaskError(mmTaskID, eCLE_InternalError);
		StopTaskRunning(mmTaskID);
	}
	else
	{
		pTask->StartTimer();
		m_SteamOnLobbySearchResults.m_callResult.Set(hSteamAPICall, this, &CDrxSteamMatchMaking::OnLobbySearchResults);
		m_SteamOnLobbySearchResults.m_taskID = mmTaskID;
		NetLog("[MatchMaking]: StartSessionSearch()");
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::TickSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	int64 elapsed = pTask->GetTimer();
	if (elapsed >= SESSION_SEARCH_TIMEOUT)
	{
		NetLog("[STEAM]: session search has been running for %" PRId64 "ms - stopping", elapsed);
		((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
		UpdateTaskError(mmTaskID, eCLE_TimeOut);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::EndSessionSearch(DrxMatchMakingTaskID mmTaskID)
{
	m_SteamOnLobbyDataUpdated.m_taskID = DrxMatchMakingInvalidTaskID;
	m_SteamOnLobbySearchResults.m_taskID = DrxMatchMakingInvalidTaskID;

	SendSessionSearchResultsToGame(mmTaskID, TMemInvalidHdl, TMemInvalidHdl);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionUpdate(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SDrxSessionUserData* pData = (SDrxSessionUserData*)m_lobby->MemGetPtr(pTask->params[SESSION_UPDATE_PARAM_USER_DATA]);

	SetSessionUserData(mmTaskID, pData, pTask->numParams[SESSION_UPDATE_PARAM_NUM_USER_DATA]);
	NetLog("[MatchMaking]: StartSessionUpdate()");
	StopTaskRunning(mmTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();

	if (pSteamMatchmaking && (pSession->localFlags & (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST)) == (DRXSESSION_LOCAL_FLAG_USED | DRXSESSION_LOCAL_FLAG_HOST))
	{
		DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> value;

		value.Format("%x", pTask->numParams[SESSION_UPDATE_SLOTS_PARAM_NUM_PUBLIC]);
		pSteamMatchmaking->SetLobbyData(pSession->m_id.m_steamID, STEAM_KEY_SESSION_PUBLIC_SLOTS, value.c_str());
		value.Format("%x", pTask->numParams[SESSION_UPDATE_SLOTS_PARAM_NUM_PRIVATE]);
		pSteamMatchmaking->SetLobbyData(pSession->m_id.m_steamID, STEAM_KEY_SESSION_PRIVATE_SLOTS, value.c_str());

		NetLog("[MatchMaking]: StartSessionUpdateSlots()");
	}

	StopTaskRunning(mmTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::EndSessionQuery(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[pTask->session];
		SDrxSessionSearchResult result;
		SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA];
		SDrxSteamSessionID* pID = new SDrxSteamSessionID(pSession->m_id.m_steamID, false);

		result.m_id = pID;
		result.m_numFilledSlots = pSession->localConnection.numUsers;

		for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
		{
			if (pSession->remoteConnection[i].used)
			{
				result.m_numFilledSlots += pSession->remoteConnection[i].numUsers;
			}
		}

		result.m_numFriends = 0;
		result.m_flags = 0;

		result.m_data = pSession->data;
		result.m_data.m_data = userData;
		for (int i = 0; i < MAX_MATCHMAKING_SESSION_USER_DATA; i++)
		{
			userData[i] = pSession->data.m_data[i];
		}

		((DrxMatchmakingSessionQueryCallback)pTask->cb)(pTask->lTaskID, pTask->error, &result, pTask->cbArg);
	}
	else
	{
		((DrxMatchmakingSessionQueryCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];

	if (pTask->error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[pTask->session];
		SDrxUserInfoResult result;

		SDrxSteamUserID* pID = new SDrxSteamUserID();
		if (pID != NULL)
		{
			pID->m_steamID = pSession->localConnection.userID.m_steamID;

			result.m_conID = pSession->localConnection.uid;
			result.m_userID = pID;
			drx_strcpy(result.m_userName, pSession->localConnection.name);
			memcpy(result.m_userData, pSession->localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
			result.m_isDedicated = false;

			((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &result, pTask->cbArg);
		}

		for (uint32 index = 0; index < MAX_LOBBY_CONNECTIONS; ++index)
		{
			if (pSession->remoteConnection[index].used)
			{
				pID = new SDrxSteamUserID();
				if (pID != NULL)
				{
					pID->m_steamID = pSession->remoteConnection[index].userID.m_steamID;

					result.m_conID = pSession->remoteConnection[index].uid;
					result.m_userID = pID;
					drx_strcpy(result.m_userName, pSession->remoteConnection[index].name);
					memcpy(result.m_userData, pSession->remoteConnection[index].userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
					result.m_isDedicated = pSession->remoteConnection[index].m_isDedicated;

					((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, &result, pTask->cbArg);
				}
			}
		}
	}

	((DrxMatchmakingSessionGetUsersCallback)pTask->cb)(pTask->lTaskID, pTask->error, NULL, pTask->cbArg);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	if (m_SteamOnLobbyEntered.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		m_SteamOnLobbyEntered.m_callResult.Cancel();
		StopTaskRunning(m_SteamOnLobbyEntered.m_taskID);
		NetLog("[MatchMaking]: StartSessionJoin() - cancelled previous session join");
	}

	SteamAPICall_t hSteamAPICall = k_uAPICallInvalid;
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (pSteamMatchmaking)
	{
		hSteamAPICall = pSteamMatchmaking->JoinLobby(pSession->m_id.m_steamID);
	}
	if (hSteamAPICall == k_uAPICallInvalid)
	{
		UpdateTaskError(mmTaskID, eCLE_InternalError);
		StopTaskRunning(mmTaskID);
	}
	else
	{
		pTask->StartTimer();
		m_SteamOnLobbyEntered.m_callResult.Set(hSteamAPICall, this, &CDrxSteamMatchMaking::OnLobbyEntered);
		m_SteamOnLobbyEntered.m_taskID = mmTaskID;
		NetLog("[MatchMaking]: StartSessionJoin()");
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::TickSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	int64 elapsed = pTask->GetTimer();
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (pSteamMatchmaking && elapsed >= SESSION_JOIN_TIMEOUT)
	{
		NetLog("[STEAM]: session join has been running for %" PRId64 "ms - stopping", elapsed);
		UpdateTaskError(mmTaskID, eCLE_TimeOut);

		// Make sure we leave the Steam lobby
		pSteamMatchmaking->LeaveLobby(pSession->m_id.m_steamID);
		NetLog("[STEAM]: Leave Steam lobby [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());
		FreeSessionHandle(pTask->session);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::EndSessionJoin(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	m_SteamOnLobbyEntered.m_taskID = DrxMatchMakingInvalidTaskID;

	if (pTask->error == eCLE_Success)
	{
		SSession* pSession = &m_sessions[pTask->session];

		((DrxMatchmakingSessionJoinCallback)pTask->cb)(pTask->lTaskID, pTask->error, CreateGameSessionHandle(pTask->session, pSession->localConnection.uid), 0, 0, pTask->cbArg);
		InitialUserDataEvent(pTask->session);

		return;
	}

	((DrxMatchmakingSessionJoinCallback)pTask->cb)(pTask->lTaskID, pTask->error, DrxSessionInvalidHandle, 0, 0, pTask->cbArg);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionDelete(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	SSession* pSession = &m_sessions[pTask->session];

	#if NETWORK_HOST_MIGRATION
	// Since we're deleting this session, terminate any host migration
	if (pSession->hostMigrationInfo.m_state != eHMS_Idle)
	{
		m_hostMigration.Terminate(&pSession->hostMigrationInfo);
	}
	#endif
	pSession->createFlags &= ~DRXSESSION_CREATE_FLAG_MIGRATABLE;

	// Disconnect our local connection
	SessionDisconnectRemoteConnectionViaNub(pTask->session, DrxMatchMakingInvalidConnectionID, eDS_Local, DrxMatchMakingInvalidConnectionID, eDC_UserRequested, "Session deleted");
	SessionUserDataEvent(eCLSE_SessionUserLeave, pTask->session, DrxMatchMakingInvalidConnectionID);
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (pSteamMatchmaking)
	{
		pSteamMatchmaking->LeaveLobby(pSession->m_id.m_steamID);
	}
	NetLog("[STEAM]: Leave Steam lobby [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; ++i)
	{
		SSession::SRConnection* pConnection = &pSession->remoteConnection[i];
		if (pConnection->used)
		{
			FreeRemoteConnection(pTask->session, i);
		}
	}
	FreeSessionHandle(pTask->session);

	NetLog("[MatchMaking]: StartSessionDelete()");
	StopTaskRunning(mmTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID)
{
	STask* pTask = &m_task[mmTaskID];
	const uint32 MaxBufferSize = DrxLobbyPacketHeaderSize + DrxLobbyPacketUINT16Size + DRXLOBBY_USER_DATA_SIZE_IN_BYTES;
	CDrxSharedLobbyPacket packet;

	SessionUserDataEvent(eCLSE_SessionUserUpdate, pTask->session, DrxMatchMakingInvalidConnectionID);

	if (packet.CreateWriteBuffer(MaxBufferSize))
	{
		SSession* pSession = &m_sessions[pTask->session];
		SSession::SLConnection* pLConnection = &pSession->localConnection;

		packet.StartWrite(eSteamPT_UserData, true);
		packet.WriteUINT16(pLConnection->uid.m_uid);
		packet.WriteData(pLConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			SendToAll(DrxMatchMakingInvalidTaskID, &packet, pTask->session, DrxMatchMakingInvalidConnectionID);
		}
		else
		{
			Send(DrxMatchMakingInvalidTaskID, &packet, pTask->session, pSession->hostConnectionID);
		}

		packet.FreeWriteBuffer();
	}
	else
	{
		UpdateTaskError(mmTaskID, eCLE_OutOfMemory);
	}

	NetLog("[MatchMaking]: StartSessionSetLocalUserData()");
	StopTaskRunning(mmTaskID);
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::InitialUserDataEvent(DrxLobbySessionHandle h)
{
	SSession* pSession = &m_sessions[h];
	pSession->localFlags |= DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED;

	if (pSession->localConnection.used)
	{
		SessionUserDataEvent(eCLSE_SessionUserJoin, h, DrxMatchMakingInvalidConnectionID);
	}

	for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
	{
		if (pSession->remoteConnection[i].used)
		{
			SessionUserDataEvent(eCLSE_SessionUserJoin, h, i);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id)
{
	SSession* pSession = &m_sessions[h];

	if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USER_DATA_EVENTS_STARTED)
	{
		SDrxUserInfoResult userInfo;
		SDrxSteamUserID* pUserID = new SDrxSteamUserID();
		userInfo.m_userID = pUserID;

		if (id == DrxMatchMakingInvalidConnectionID)
		{
			SSession::SLConnection* pConnection = &pSession->localConnection;

			userInfo.m_conID = pConnection->uid;
			userInfo.m_isDedicated = false;
			drx_strcpy(userInfo.m_userName, pConnection->name);
			memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
			pUserID->m_steamID = pConnection->userID.m_steamID;

			NetLog("[STEAM]: CDrxSteamMatchMaking::SessionUserDataEvent() sending event %d on session %d for local user [%s][%s]", event, h, userInfo.m_userName, pConnection->userID.GetGUIDAsString().c_str());
			CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
		}
		else
		{
			SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

			userInfo.m_conID = pConnection->uid;
			userInfo.m_isDedicated = pConnection->m_isDedicated;
			drx_strcpy(userInfo.m_userName, pConnection->name);
			memcpy(userInfo.m_userData, pConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
			pUserID->m_steamID = pConnection->userID.m_steamID;

			NetLog("[STEAM]: CDrxSteamMatchMaking::SessionUserDataEvent() sending event %d on session %d for remote user [%s][%s]", event, h, userInfo.m_userName, pConnection->userID.GetGUIDAsString().c_str());
			CDrxMatchMaking::SessionUserDataEvent(event, h, &userInfo);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Steam Lobby Packet handlers
//
////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	if (m_SteamOnLobbyEntered.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[m_SteamOnLobbyEntered.m_taskID];
		SSession* pSession = &m_sessions[pTask->session];

		uint8 numUsers;
		bool isDedicated;
		SDrxMatchMakingConnectionUID hostConnectionUID;
		SDrxSteamUserID steamID;
		char name[DRXLOBBY_USER_NAME_LENGTH];
		uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];

		pPacket->StartRead();
		pSession->InitialiseLocalConnection(pPacket->ReadConnectionUID());
		uint32 sessionCreateGameFlags = pPacket->ReadUINT16();
		pSession->createFlags = (pSession->createFlags & DRXSESSION_CREATE_FLAG_SYSTEM_MASK) | (sessionCreateGameFlags << DRXSESSION_CREATE_FLAG_GAME_FLAGS_SHIFT);

		bool hostMigrationSupported = m_lobby->GetLobbyServiceFlag(m_serviceType, eCLSF_SupportHostMigration);
		if ((pSession->createFlags & DRXSESSION_CREATE_FLAG_MIGRATABLE) && hostMigrationSupported)
		{
			pSession->localFlags |= DRXSESSION_LOCAL_FLAG_CAN_SEND_HOST_HINTS;
		}

		NetLog("[Lobby] Created local connection " PRFORMAT_SH " " PRFORMAT_UID, PRARG_SH(pTask->session), PRARG_UID(pSession->localConnection.uid));
		if ((!(pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)) && (pSession->localConnection.uid.m_uid > m_connectionUIDCounter))
		{
			// Keep all remote clients in sync with the host (in case they're chosen to be host during host migration)
			m_connectionUIDCounter = pSession->localConnection.uid.m_uid;
		}

		numUsers = pPacket->ReadUINT8();
		isDedicated = pPacket->ReadBool();
		hostConnectionUID = pPacket->ReadConnectionUID();
		steamID.ReadFromPacket(pPacket);
		pPacket->ReadString(name, DRXLOBBY_USER_NAME_LENGTH);
		pPacket->ReadData(userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

		DrxMatchMakingConnectionID id = AddRemoteConnection(pTask->session, DrxLobbyInvalidConnectionID, hostConnectionUID, steamID.m_steamID, numUsers, name, userData, isDedicated);

		if (id != DrxMatchMakingInvalidConnectionID)
		{
			NetLog("[MatchMaking] Created server connection " PRFORMAT_MMCINFO, PRARG_MMCINFO(id, pSession->remoteConnection[id].connectionID, hostConnectionUID));
			StopTaskRunning(m_SteamOnLobbyEntered.m_taskID);
			pSession->hostConnectionID = id;
			SessionUserDataEvent(eCLSE_SessionUserJoin, pTask->session, id);
		}
		else
		{
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_ConnectionFailed);
		}
	}
	else
	{
		NetLog("[STEAM]: could not find session join task when processing session join result");
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::ProcessLocalUserData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket)
{
	DrxLobbySessionHandle h;
	SDrxMatchMakingConnectionUID uid;
	DrxMatchMakingConnectionID id;

	pPacket->StartRead();
	uid = pPacket->GetFromConnectionUID();
	uid.m_uid = pPacket->ReadUINT16();

	NetLog("[STEAM]: CDrxSteamMatchMaking::ProcessLocalUserData() received user data packet");
	if (FindConnectionFromUID(uid, &h, &id))
	{
		SSession* pSession = &m_sessions[h];
		SSession::SRConnection* pRConnection = &pSession->remoteConnection[id];

		pPacket->ReadData(pRConnection->userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
		SessionUserDataEvent(eCLSE_SessionUserUpdate, h, id);
		NetLog("[STEAM]: CDrxSteamMatchMaking::ProcessLocalUserData() processed user data packet");

		if (pSession->localFlags & DRXSESSION_LOCAL_FLAG_HOST)
		{
			NetLog("[STEAM]: CDrxSteamMatchMaking::ProcessLocalUserData() host sent user data packet on to all");
			SendToAll(DrxMatchMakingInvalidTaskID, pPacket, h, id);
		}
	}
	else
	{
		NetLog("[STEAM]: CDrxSteamMatchMaking::ProcessLocalUserData() unable to find connection");
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Steam callbacks
//
////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnSteamShutdown(SteamShutdown_t* pParam)
{
	static bool exiting = false;
	if (!exiting)
	{
		NetLog("[STEAM]: Steam shutdown request; exiting");
		exiting = true;
		IConsole* pConsole = gEnv->pConsole;
		if (pConsole != NULL)
		{
			pConsole->ExecuteString("quit", true, true);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnLobbyDataUpdated(LobbyDataUpdate_t* pParam)
{

	SDrxSteamSessionID lobbyID(CSteamID(pParam->m_ulSteamIDLobby), false);
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();

	// Find the session
	SSession* pSession = NULL;
	uint32 session;
	for (session = 0; session < MAX_MATCHMAKING_SESSIONS; ++session)
	{
		if (m_sessions[session].m_id.m_steamID == lobbyID.m_steamID)
		{
			pSession = &m_sessions[session];
			break;
		}
	}

	#if USE_DRX_VOICE && USE_STEAM_VOICE
	if (pSession && pSteamMatchmaking)
	{
		int numMembers = pSteamMatchmaking->GetNumLobbyMembers(pParam->m_ulSteamIDMember);
		for (int i = 0; i < numMembers; i++)
		{
			CSteamID user = pSteamMatchmaking->GetLobbyMemberByIndex(pParam->m_ulSteamIDMember, i);

			SDrxSteamUserID* pUser = new SDrxSteamUserID(user);
			DrxUserID userId(pUser);
			CDrxSteamVoice* voice = (CDrxSteamVoice*)m_pService->GetVoice();
			ISteamUser* pSteamUser = SteamUser();
			if (pSteamUser)
			{
				for (int i = 0; i < pSession->remoteConnection.Size(); i++)
				{
					// check if connectionId is valid and user is a remote user
					if (pSession->remoteConnection[i].connectionID != DrxLobbyInvalidConnectionID && user != pSteamUser->GetSteamID())
					{
						NetLog("[Steam] Registering user to voice from OnLobbyDataUpdated");
						voice->RegisterRemoteUser(pSession->remoteConnection[i].connectionID, userId);
					}
				}
			}

		}
	}
	#endif // USE_DRX_VOICE

	if (pSteamMatchmaking && m_SteamOnLobbyDataUpdated.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[m_SteamOnLobbyDataUpdated.m_taskID];
		SDrxSessionSearchParam* pSessionSearchParam = (SDrxSessionSearchParam*)m_lobby->MemGetPtr(pTask->params[SESSION_SEARCH_PARAM_GAME_PARAM]);

		if ((pParam->m_bSuccess) && (pSessionSearchParam->m_maxNumReturn > 0))
		{
			DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> key;
			DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> value;
			uint32 count = pSteamMatchmaking->GetLobbyDataCount(pParam->m_ulSteamIDLobby);
			uint32 start = 0;

			// Extract the user data from the lobby
			TMemHdl resultsHdl = m_lobby->MemAlloc(sizeof(SDrxSessionSearchResult));
			SDrxSessionSearchResult* pResult = (SDrxSessionSearchResult*)m_lobby->MemGetPtr(resultsHdl);
			if (pResult != NULL)
			{
				// Placement new the struct to correctly initialise m_id, which is a smart pointer
				new(pResult) SDrxSessionSearchResult;

				SDrxSteamSessionID* pID = new SDrxSteamSessionID();
				if (pID != NULL)
				{
					pID->m_steamID.SetFromUint64(pParam->m_ulSteamIDLobby);

					pResult->m_id = pID;
					pResult->m_numFilledSlots = 0;
					pResult->m_numFriends = 0;
					pResult->m_ping = 0;
					pResult->m_flags = 0;

					key = STEAM_KEY_SESSION_NAME;
					value = pSteamMatchmaking->GetLobbyData(pParam->m_ulSteamIDLobby, key.c_str());
					drx_strcpy(pResult->m_data.m_name, value.c_str());
					++start;

					key = STEAM_KEY_SESSION_PUBLIC_SLOTS;
					value = pSteamMatchmaking->GetLobbyData(pParam->m_ulSteamIDLobby, key.c_str());
					sscanf(value.c_str(), "%x", &pResult->m_data.m_numPublicSlots);
					++start;

					key = STEAM_KEY_SESSION_PRIVATE_SLOTS;
					value = pSteamMatchmaking->GetLobbyData(pParam->m_ulSteamIDLobby, key.c_str());
					sscanf(value.c_str(), "%x", &pResult->m_data.m_numPrivateSlots);
					++start;

					key = STEAM_KEY_SESSION_GAME_TYPE;
					value = pSteamMatchmaking->GetLobbyData(pParam->m_ulSteamIDLobby, key.c_str());
					unsigned int ranked;
					sscanf(value.c_str(), "%x", &ranked);
					pResult->m_data.m_ranked = ranked != 0;
					++start;

					pResult->m_data.m_numData = count < start ? 0 : count - start;

					TMemHdl userHdl = 0;
					SDrxSessionUserData* pUserData = NULL;

					if (pResult->m_data.m_numData)
					{
						userHdl = m_lobby->MemAlloc(sizeof(SDrxSessionUserData) * (pResult->m_data.m_numData));
						pUserData = (SDrxSessionUserData*)m_lobby->MemGetPtr(userHdl);
					}

					if (pUserData != NULL)
					{
						pResult->m_data.m_data = pUserData;

						for (uint32 index = 0; index < pResult->m_data.m_numData; ++index)
						{
							// Placement new the struct to correctly initialise m_id
							new(&pUserData[index])SDrxSessionUserData;

							key.Format(STEAM_KEY_SESSION_USER_DATA "%d", index);
							value = pSteamMatchmaking->GetLobbyData(pParam->m_ulSteamIDLobby, key.c_str());

							sscanf(value.c_str(), "%x", &(pUserData[index].m_type));
							unsigned int id = 0;
							unsigned int val;
							switch (pUserData[index].m_type)
							{
							case eCLUDT_Int64:
								sscanf(value.c_str(), "%x:%x:%" PRIx64, &(pUserData[index].m_type), &id, &(pUserData[index].m_int64));
								break;
							case eCLUDT_Int32:
								sscanf(value.c_str(), "%x:%x:%x", &(pUserData[index].m_type), &id, &(pUserData[index].m_int32));
								break;
							case eCLUDT_Int16:
								sscanf(value.c_str(), "%x:%x:%x", &(pUserData[index].m_type), &id, &val);
								pUserData[index].m_int16 = val;
								break;
							case eCLUDT_Int8:
								sscanf(value.c_str(), "%x:%x:%x", &(pUserData[index].m_type), &id, &val);
								pUserData[index].m_int8 = val;
								break;
							case eCLUDT_Float64:
								sscanf(value.c_str(), "%x:%x:%lf", &(pUserData[index].m_type), &id, &(pUserData[index].m_f64));
								break;
							case eCLUDT_Float32:
								sscanf(value.c_str(), "%x:%x:%f", &(pUserData[index].m_type), &id, &(pUserData[index].m_f32));
								break;
							}
							pUserData[index].m_id = id;
						}

						TO_GAME_FROM_LOBBY(&CDrxSteamMatchMaking::SendSessionSearchResultsToGame, this, m_SteamOnLobbyDataUpdated.m_taskID, resultsHdl, userHdl);
						--pSessionSearchParam->m_maxNumReturn;

						if (pSessionSearchParam->m_maxNumReturn == 0)
						{
							StopTaskRunning(m_SteamOnLobbyDataUpdated.m_taskID);
							m_SteamOnLobbyDataUpdated.m_taskID = DrxMatchMakingInvalidTaskID;
						}
					}
					else
					{
						m_lobby->MemFree(resultsHdl);
						UpdateTaskError(m_SteamOnLobbyDataUpdated.m_taskID, eCLE_OutOfMemory);
					}
				}
				else
				{
					m_lobby->MemFree(resultsHdl);
					UpdateTaskError(m_SteamOnLobbyDataUpdated.m_taskID, eCLE_OutOfMemory);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnP2PSessionRequest(P2PSessionRequest_t* pParam)
{
	// Inform Steam that we're happy to accept traffic from this user
	// Note: might have to add tests here later, but for now just accept anyone
	
	ISteamNetworking* pSteamNetworking = SteamNetworking();
	if (pSteamNetworking)
	{
		NetLog("[STEAM]: Accepting P2P connection request for Steam ID [%s]", SDrxSteamUserID(pParam->m_steamIDRemote).GetGUIDAsString().c_str());
		pSteamNetworking->AcceptP2PSessionWithUser(pParam->m_steamIDRemote);
	}
	else
	{
		DrxWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "Steam networking service not avaialble");
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnLobbyChatUpdate(LobbyChatUpdate_t* pParam)
{
	SDrxSteamSessionID lobbyID(CSteamID(pParam->m_ulSteamIDLobby), false);
	SDrxSteamUserID userChanged(CSteamID(pParam->m_ulSteamIDUserChanged));
	SDrxSteamUserID userMakingChange(CSteamID(pParam->m_ulSteamIDMakingChange));

	ISteamFriends* pSteamFriends = SteamFriends();
	if (!pSteamFriends)
	{
		DrxWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "Steam friends service not available");
		return;
	}

	// Find the session
	SSession* pSession = NULL;
	uint32 session;
	for (session = 0; session < MAX_MATCHMAKING_SESSIONS; ++session)
	{
		if (m_sessions[session].m_id.m_steamID == lobbyID.m_steamID)
		{
			pSession = &m_sessions[session];
			break;
		}
	}

	if (pSession != NULL)
	{
		if (BChatMemberStateChangeRemoved(pParam->m_rgfChatMemberStateChange))
		{
			// Defaults for k_EChatMemberStateChangeLeft and k_EChatMemberStateChangeDisconnected
			EDrxLobbySystemEvent clse = eCLSE_SessionUserLeave;
			EDisconnectionCause dc = eDC_UserRequested;
			const char* description = "User requested";

			if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeKicked)
			{
				clse = eCLSE_KickedFromSession;
				dc = eDC_Kicked;
				description = "User kicked";
			}

			if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeBanned)
			{
				clse = eCLSE_KickedFromSession;
				dc = eDC_Banned;
				description = "User banned";
			}

			NetLog("[STEAM]: user [%s][%s] removed : [%s]", pSteamFriends->GetFriendPersonaName(userChanged.m_steamID), userChanged.GetGUIDAsString().c_str(), description);

			// Search for the player in the session
			if (pSession->localConnection.userID.m_steamID == userChanged.m_steamID)
			{
				SessionUserDataEvent(clse, session, DrxMatchMakingInvalidConnectionID);

				// End session
				SessionDisconnectRemoteConnectionViaNub(session, DrxMatchMakingInvalidConnectionID, eDS_Local, DrxMatchMakingInvalidConnectionID, dc, description);
				ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
				if (pSteamMatchmaking)
				{
					pSteamMatchmaking->LeaveLobby(pSession->m_id.m_steamID);
				}
				NetLog("[STEAM]: Leave Steam lobby [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());

				for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; ++i)
				{
					SSession::SRConnection* pConnection = &pSession->remoteConnection[i];
					if (pConnection->used)
					{
						FreeRemoteConnection(session, i);
					}
				}
				FreeSessionHandle(session);
			}
			else
			{
				for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; ++i)
				{
					if (pSession->remoteConnection[i].userID.m_steamID == userChanged.m_steamID)
					{
						SessionUserDataEvent(clse, session, i);
						FreeRemoteConnection(session, i);
						break;
					}
				}
			}
		}
		else
		{
			if (pSession->localConnection.userID == userChanged)
			{
				SessionUserDataEvent(eCLSE_SessionUserJoin, session, DrxMatchMakingInvalidConnectionID);
				NetLog("[STEAM]: local user [%s][%s] added", pSteamFriends->GetFriendPersonaName(userChanged.m_steamID), userChanged.GetGUIDAsString().c_str());
			}
			else
			{
				// Add the player to the session (with no user data - that comes later from lobby packets)
				uint8 data[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
				memset(data, 0, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);
				DrxMatchMakingConnectionID id = AddRemoteConnection(session, DrxLobbyInvalidConnectionID, CreateConnectionUID(session), userChanged.m_steamID, 1, pSteamFriends->GetFriendPersonaName(userChanged.m_steamID), data, false);
				if (id != DrxMatchMakingInvalidConnectionID)
				{
					// Send a response to the joining player
					CDrxSharedLobbyPacket packet;
					if (packet.CreateWriteBuffer(MAX_LOBBY_PACKET_SIZE))
					{
						SSession::SRConnection* pConnection = &pSession->remoteConnection[id];

						packet.StartWrite(eSteamPT_SessionRequestJoinResult, true);
						packet.WriteConnectionUID(pConnection->uid);
						packet.WriteUINT16((pSession->createFlags & DRXSESSION_CREATE_FLAG_GAME_MASK) >> DRXSESSION_CREATE_FLAG_GAME_FLAGS_SHIFT);
						packet.WriteUINT8(pSession->localConnection.numUsers);
						if (gEnv->IsDedicated())                          // Server should flag if its dedicated or not
						{
							packet.WriteBool(true);
						}
						else
						{
							packet.WriteBool(false);
						}
						packet.WriteConnectionUID(pSession->localConnection.uid);
						pSession->localConnection.userID.WriteToPacket(&packet);
						packet.WriteString(pSession->localConnection.name, DRXLOBBY_USER_NAME_LENGTH);
						packet.WriteData(pSession->localConnection.userData, DRXLOBBY_USER_DATA_SIZE_IN_BYTES);

						Send(DrxMatchMakingInvalidTaskID, &packet, session, id);
						SessionUserDataEvent(eCLSE_SessionUserJoin, session, id);

						NetLog("[STEAM]: remote user [%s][%s] added", pSteamFriends->GetFriendPersonaName(userChanged.m_steamID), userChanged.GetGUIDAsString().c_str());
					}
				}
				else
				{
					NetLog("[STEAM]: remote user [%s][%s] UNABLE TO ADD", pSteamFriends->GetFriendPersonaName(userChanged.m_steamID), userChanged.GetGUIDAsString().c_str());
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Steam callresults
//
////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnLobbyCreated(LobbyCreated_t* pParam, bool ioFailure)
{
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	if (m_SteamOnLobbyCreated.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[m_SteamOnLobbyCreated.m_taskID];

		if (!ioFailure)
		{
			SSession* pSession = &m_sessions[pTask->session];
			SDrxSessionData* pData = (SDrxSessionData*)m_lobby->MemGetPtr(pTask->params[SESSION_CREATE_PARAM_SESSION_DATA]);

			UpdateTaskError(m_SteamOnLobbyCreated.m_taskID, ConvertSteamErrorToDrxLobbyError(pParam->m_eResult));

			if (pTask->error == eCLE_Success)
			{
				DrxFixedStringT<MAX_STEAM_KEY_VALUE_SIZE> value;

				// Set session details
				pSession->m_id.m_steamID = pParam->m_ulSteamIDLobby;
				pSession->InitialiseLocalConnection(CreateConnectionUID(pTask->session));
				drx_strcpy(pSession->data.m_name, pSession->localConnection.name);

				NetLog("[STEAM]: Steam lobby created by [%s], id [%s]", pSession->localConnection.name, CSteamIDAsString(pSession->m_id.m_steamID).c_str());

				// Set lobby keys
				pSteamMatchmaking->SetLobbyData(pParam->m_ulSteamIDLobby, STEAM_KEY_SESSION_NAME, pData->m_name);
				value.Format("%x", pData->m_numPublicSlots);
				pSteamMatchmaking->SetLobbyData(pParam->m_ulSteamIDLobby, STEAM_KEY_SESSION_PUBLIC_SLOTS, value.c_str());
				value.Format("%x", pData->m_numPrivateSlots);
				pSteamMatchmaking->SetLobbyData(pParam->m_ulSteamIDLobby, STEAM_KEY_SESSION_PRIVATE_SLOTS, value.c_str());
				value.Format("%x", pData->m_ranked);
				pSteamMatchmaking->SetLobbyData(pParam->m_ulSteamIDLobby, STEAM_KEY_SESSION_GAME_TYPE, value.c_str());

				SetSessionUserData(m_SteamOnLobbyCreated.m_taskID, pData->m_data, pData->m_numData);
				NetLog("[STEAM]: created lobby successfully");
			}
		}
		else
		{
			UpdateTaskError(m_SteamOnLobbyCreated.m_taskID, eCLE_InternalError);
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[STEAM]: failed to create lobby");
		}

		StopTaskRunning(m_SteamOnLobbyCreated.m_taskID);
		m_SteamOnLobbyCreated.m_taskID = DrxMatchMakingInvalidTaskID;
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnLobbySearchResults(LobbyMatchList_t* pParam, bool ioFailure)
{
	if (m_SteamOnLobbySearchResults.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[m_SteamOnLobbySearchResults.m_taskID];
		SDrxSessionSearchParam* pSessionSearchParam = (SDrxSessionSearchParam*)m_lobby->MemGetPtr(pTask->params[SESSION_SEARCH_PARAM_GAME_PARAM]);
		ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();

		if (pSteamMatchmaking && pParam->m_nLobbiesMatching > 0)
		{
			if (pParam->m_nLobbiesMatching < pSessionSearchParam->m_maxNumReturn)
			{
				pSessionSearchParam->m_maxNumReturn = pParam->m_nLobbiesMatching;
			}

			for (uint32 index = 0; index < pSessionSearchParam->m_maxNumReturn; ++index)
			{
				CSteamID lobbyID = pSteamMatchmaking->GetLobbyByIndex(index);
				pSteamMatchmaking->RequestLobbyData(lobbyID);
			}

			m_SteamOnLobbyDataUpdated.m_taskID = m_SteamOnLobbySearchResults.m_taskID;
			m_SteamOnLobbySearchResults.m_taskID = DrxMatchMakingInvalidTaskID;

			NetLog("[STEAM]: found %d lobbies - retrieving data", pParam->m_nLobbiesMatching);
		}
		else
		{
			NetLog("[STEAM]: no lobbies found");
			StopTaskRunning(m_SteamOnLobbySearchResults.m_taskID);
			m_SteamOnLobbySearchResults.m_taskID = DrxMatchMakingInvalidTaskID;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::OnLobbyEntered(LobbyEnter_t* pParam, bool ioFailure)
{
	if (m_SteamOnLobbyEntered.m_taskID != DrxMatchMakingInvalidTaskID)
	{
		STask* pTask = &m_task[m_SteamOnLobbyEntered.m_taskID];
		SSession* pSession = &m_sessions[pTask->session];

		switch (pParam->m_EChatRoomEnterResponse)
		{
		case k_EChatRoomEnterResponseSuccess:             // Success
			break;
		case k_EChatRoomEnterResponseDoesntExist:         // Chat doesn't exist (probably closed)
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_InvalidSession);
			break;
		case k_EChatRoomEnterResponseLimited:             // Joining this chat is not allowed because you are a limited user (no value on account)
		case k_EChatRoomEnterResponseNotAllowed:          // General Denied - You don't have the permissions needed to join the chat
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_InsufficientPrivileges);
			break;
		case k_EChatRoomEnterResponseFull:                // Chat room has reached its maximum size
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_SessionFull);
			break;
		case k_EChatRoomEnterResponseError:               // Unexpected Error
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_InternalError);
			break;
		case k_EChatRoomEnterResponseCommunityBan:        // Attempt to join a chat when the user has a community lock on their account
		case k_EChatRoomEnterResponseBanned:              // You are banned from this chat room and may not join
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_Banned);
			break;
		case k_EChatRoomEnterResponseClanDisabled:        // Attempt to join a clan chat when the clan is locked or disabled
		case k_EChatRoomEnterResponseMemberBlockedYou:    // Join failed - some member in the chat has blocked you from joining
		case k_EChatRoomEnterResponseYouBlockedMember:    // Join failed - you have blocked some member already in the chat
			UpdateTaskError(m_SteamOnLobbyEntered.m_taskID, eCLE_SteamBlocked);
			break;
		}

		if (pTask->error == eCLE_Success)
		{
			NetLog("[STEAM]: joined Steam lobby [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());
		}
		else
		{
			NetLog("[STEAM]: could not join Steam lobby [%s]", CSteamIDAsString(pSession->m_id.m_steamID).c_str());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// End of Steam callbacks/callresults
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Utility functions to send data back to the game on the game thread
//
////////////////////////////////////////////////////////////////////////////////

void CDrxSteamMatchMaking::SendSessionSearchResultsToGame(DrxMatchMakingTaskID mmTaskID, TMemHdl resultsHdl, TMemHdl userHdl)
{
	STask* pTask = &m_task[mmTaskID];

	if (resultsHdl != TMemInvalidHdl)
	{
		SDrxSessionSearchResult* pResult = (SDrxSessionSearchResult*)m_lobby->MemGetPtr(resultsHdl);
		pResult->m_data.m_data = (SDrxLobbyUserData*)m_lobby->MemGetPtr(userHdl);

		((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, eCLE_SuccessContinue, pResult, pTask->cbArg);

		m_lobby->MemFree(userHdl);
		m_lobby->MemFree(resultsHdl);
	}
	else
	{
		// No associated memory means we can tell the game the search is finished
		((DrxMatchmakingSessionSearchCallback)pTask->cb)(pTask->lTaskID, eCLE_Success, NULL, pTask->cbArg);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// End of utility functions
//
////////////////////////////////////////////////////////////////////////////////

EDrxLobbyError CDrxSteamMatchMaking::ConvertSteamErrorToDrxLobbyError(uint32 steamErrorCode)
{
	switch (steamErrorCode)
	{
	case k_EResultOK:                             // success
		return eCLE_Success;
	case k_EResultFail:                           // generic failure
	case k_EResultNoConnection:                   // no/failed network connection
	case k_EResultAccessDenied:                   // access is denied
	case k_EResultLimitExceeded:                  // Too much of a good thing
		return eCLE_InternalError;
	case k_EResultTimeout:                        // operation timed out
		return eCLE_TimeOut;
	case k_EResultInvalidPassword:                // password/ticket is invalid
	case k_EResultLoggedInElsewhere:              // same user logged in elsewhere
	case k_EResultInvalidProtocolVer:             // protocol version is incorrect
	case k_EResultInvalidParam:                   // a parameter is incorrect
	case k_EResultFileNotFound:                   // file was not found
	case k_EResultBusy:                           // called method busy - action not taken
	case k_EResultInvalidState:                   // called object was in an invalid state
	case k_EResultInvalidName:                    // name is invalid
	case k_EResultInvalidEmail:                   // email is invalid
	case k_EResultDuplicateName:                  // name is not unique
	case k_EResultBanned:                         // VAC2 banned
	case k_EResultAccountNotFound:                // account not found
	case k_EResultInvalidSteamID:                 // steamID is invalid
	case k_EResultServiceUnavailable:             // The requested service is currently unavailable
	case k_EResultNotLoggedOn:                    // The user is not logged on
	case k_EResultPending:                        // Request is pending (may be in process, or waiting on third party)
	case k_EResultEncryptionFailure:              // Encryption or Decryption failed
	case k_EResultInsufficientPrivilege:          // Insufficient privilege
	case k_EResultRevoked:                        // Access has been revoked (used for revoked guest passes)
	case k_EResultExpired:                        // License/Guest pass the user is trying to access is expired
	case k_EResultAlreadyRedeemed:                // Guest pass has already been redeemed by account, cannot be acked again
	case k_EResultDuplicateRequest:               // The request is a duplicate and the action has already occurred in the past, ignored this time
	case k_EResultAlreadyOwned:                   // All the games in this guest pass redemption request are already owned by the user
	case k_EResultIPNotFound:                     // IP address not found
	case k_EResultPersistFailed:                  // failed to write change to the data store
	case k_EResultLockingFailed:                  // failed to acquire access lock for this operation
	case k_EResultLogonSessionReplaced:
	case k_EResultConnectFailed:
	case k_EResultHandshakeFailed:
	case k_EResultIOFailure:
	case k_EResultRemoteDisconnect:
	case k_EResultShoppingCartNotFound:           // failed to find the shopping cart requested
	case k_EResultBlocked:                        // a user didn't allow it
	case k_EResultIgnored:                        // target is ignoring sender
	case k_EResultNoMatch:                        // nothing matching the request found
	case k_EResultAccountDisabled:
	case k_EResultServiceReadOnly:                // this service is not accepting content changes right now
	case k_EResultAccountNotFeatured:             // account doesn't have value, so this feature isn't available
	case k_EResultAdministratorOK:                // allowed to take this action, but only because requester is admin
	case k_EResultContentVersion:                 // A Version mismatch in content transmitted within the Steam protocol.
	case k_EResultTryAnotherCM:                   // The current CM can't service the user making a request, user should try another.
	case k_EResultPasswordRequiredToKickSession:  // You are already logged in elsewhere, this cached credential login has failed.
	case k_EResultAlreadyLoggedInElsewhere:       // You are already logged in elsewhere, you must wait
	case k_EResultSuspended:                      // Long running operation (content download) suspended/paused
	case k_EResultCancelled:                      // Operation canceled (typically by user: content download)
	case k_EResultDataCorruption:                 // Operation canceled because data is ill formed or unrecoverable
	case k_EResultDiskFull:                       // Operation canceled - not enough disk space.
	case k_EResultRemoteCallFailed:               // an remote call or IPC call failed
	case k_EResultPasswordUnset:                  // Password could not be verified as it's unset server side
	case k_EResultExternalAccountUnlinked:        // External account (PSN, Facebook...) is not linked to a Steam account
	case k_EResultPSNTicketInvalid:               // PSN ticket was invalid
	case k_EResultExternalAccountAlreadyLinked:   // External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first
	case k_EResultRemoteFileConflict:             // The sync cannot resume due to a conflict between the local and remote files
	case k_EResultIllegalPassword:                // The requested new password is not legal
	case k_EResultSameAsPreviousValue:            // new value is the same as the old one ( secret question and answer )
	case k_EResultAccountLogonDenied:             // account login denied due to 2nd factor authentication failure
	case k_EResultCannotUseOldPassword:           // The requested new password is not legal
	case k_EResultInvalidLoginAuthCode:           // account login denied due to auth code invalid
	case k_EResultAccountLogonDeniedNoMail:       // account login denied due to 2nd factor auth failure - and no mail has been sent
	case k_EResultHardwareNotCapableOfIPT:        //
	case k_EResultIPTInitError:                   //
	case k_EResultParentalControlRestricted:      // operation failed due to parental control restrictions for current user
	case k_EResultFacebookQueryError:             // Facebook query returned an error
	case k_EResultExpiredLoginAuthCode:           // account login denied due to auth code expired
	case k_EResultIPLoginRestrictionFailed:
	case k_EResultAccountLockedDown:
	case k_EResultAccountLogonDeniedVerifiedEmailRequired:
	case k_EResultNoMatchingURL:
	default:
	#if !defined(RELEASE)
		gEnv->pSystem->debug_LogCallStack();
	#endif // !defined(RELEASE)
		DrxLog("[STEAM]: Unhandled error code %d", steamErrorCode);
		return eCLE_InternalError;
	}
}

////////////////////////////////////////////////////////////////////////////////

	#if !defined(_RELEASE)
void CDrxSteamMatchMaking::Debug_DumpLobbyMembers(DrxLobbySessionHandle lsh)
{
	SSession* pSession = &m_sessions[lsh];
	ISteamMatchmaking* pSteamMatchmaking = SteamMatchmaking();
	ISteamFriends* pSteamFriends = SteamFriends();

	if ((lsh < MAX_MATCHMAKING_SESSIONS) && (pSession->localFlags & DRXSESSION_LOCAL_FLAG_USED))
	{
		NetLog("[STEAM]: *** Dumping session members ***");
		NetLog("[STEAM]: local player is [%s][%s]", pSession->localConnection.name, pSession->localConnection.userID.GetGUIDAsString().c_str());
		uint32 remotePlayerCount = 0;
		for (uint32 i = 0; i < MAX_LOBBY_CONNECTIONS; i++)
		{
			if (pSession->remoteConnection[i].used)
			{
				NetLog("[STEAM]: remote player [%d] is [%s][%s]", ++remotePlayerCount, pSession->remoteConnection[i].name, pSession->remoteConnection[i].userID.GetGUIDAsString().c_str());
			}
		}

		uint32 numLobbyMembers = (pSteamMatchmaking && pSteamFriends) ? pSteamMatchmaking->GetNumLobbyMembers(pSession->m_id.m_steamID) : 0;
		NetLog("[STEAM]: *** Dumping Steam lobby members (lobby id [%s], %d members) ***", CSteamIDAsString(pSession->m_id.m_steamID).c_str(), numLobbyMembers);
		if (numLobbyMembers > 0 && pSteamMatchmaking && pSteamFriends)
		{
			SDrxSteamUserID id;
			if (pSteamMatchmaking->GetLobbyGameServer(pSession->m_id.m_steamID, NULL, NULL, &id.m_steamID))
			{
				NetLog("[STEAM]: lobby game server is [%s]", id.GetGUIDAsString().c_str());
			}
			else
			{
				NetLog("[STEAM]: lobby game server is [unknown]");
			}

			id = pSteamMatchmaking->GetLobbyOwner(pSession->m_id.m_steamID);
			NetLog("[STEAM]: lobby host is [%s][%s]", pSteamFriends->GetFriendPersonaName(id.m_steamID), id.GetGUIDAsString().c_str());

			for (uint32 index = 0; index < numLobbyMembers; ++index)
			{
				id = pSteamMatchmaking->GetLobbyMemberByIndex(pSession->m_id.m_steamID, index);
				NetLog("[STEAM]: lobby member [%d] is [%s][%s]", index, pSteamFriends->GetFriendPersonaName(id.m_steamID), id.GetGUIDAsString().c_str());
			}
		}
		NetLog("[STEAM]: *** Finished dumping session members ***");
	}
}
	#endif // !defined(_RELEASE)

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_MATCHMAKING
