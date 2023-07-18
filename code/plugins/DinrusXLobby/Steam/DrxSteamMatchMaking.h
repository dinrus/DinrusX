// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//
// -------------------------------------------------------------------------
//  File name:   DrxSteamMatchMaking.h
//  Created:     11/10/2012 by Andrew Catlender
//  Описание: Integration of Steamworks API into DrxMatchMaking
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXSTEAMMATCHMAKING_H__
#define __DRXSTEAMMATCHMAKING_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "DrxMatchMaking.h"

#if USE_STEAM && USE_DRX_MATCHMAKING

////////////////////////////////////////////////////////////////////////////////

	#include "steam/steam_api.h"
	#include "steam/steam_gameserver.h"

////////////////////////////////////////////////////////////////////////////////

// Utility class to map a DrxMatchMakingTaskID to a steam callback
template<class T, class P> struct SDrxSteamCallback
{
	typedef void (T::* func_t)(P*);

	SDrxSteamCallback(T* pType, func_t func)
		: m_callBack(pType, func)
		, m_taskID(DrxMatchMakingInvalidTaskID)
	{}

	CCallback<T, P, false> m_callBack;
	uint32                 m_taskID;
};

// Utility class to map a DrxMatchMakingTaskID to a steam callresult
template<class T, class P> struct SDrxSteamCallResult
{
	SDrxSteamCallResult(void)
		: m_taskID(DrxMatchMakingInvalidTaskID)
	{}

	CCallResult<T, P> m_callResult;
	uint32            m_taskID;
};

// Prevent anyone using STEAM_CALLBACK
	#if defined(STEAM_CALLBACK)
		#undef STEAM_CALLBACK
		#define STEAM_CALLBACK # error Use DRXSTEAM_CALLBACK instead
	#endif

// Wrapper for steam callbacks.  Can't use STEAM_CALLBACK because we need to
// associate the DrxMatchMakingTaskID with the callback
	#define DRXSTEAM_CALLBACK(thisclass, func, param, var) \
	  SDrxSteamCallback<thisclass, param> var;             \
	  void func(param * pParam)

// Wrapper for steam callresults.  Steam doesn't define a STEAM_CALLRESULT, and
// we need to associate the DrxMatchMakingTaskID with the callresult anyway so
// I've made an analogous macro to DRXSTEAM_CALLBACK above
	#define DRXSTEAM_CALLRESULT(thisclass, func, param, var) \
	  SDrxSteamCallResult<thisclass, param> var;             \
	  void func(param * pParam, bool ioFailure)

////////////////////////////////////////////////////////////////////////////////

	#define STEAMID_AS_STRING_LENGTH (32)
static DrxFixedStringT<STEAMID_AS_STRING_LENGTH> CSteamIDAsString(const CSteamID& id)
{
	DrxFixedStringT<STEAMID_AS_STRING_LENGTH> result;
	result.Format("%02X:%X:%05X:%08X", id.GetEUniverse(), id.GetEAccountType(), id.GetUnAccountInstance(), id.GetAccountID());

	return result;
};

////////////////////////////////////////////////////////////////////////////////

struct SDrxSteamSessionID : public SDrxSharedSessionID
{
	SDrxSteamSessionID()
		: m_steamID(CSteamID())
		, m_fromInvite(false)
	{}

	SDrxSteamSessionID(CSteamID& id, bool fromInvite)
		: m_steamID(id)
		, m_fromInvite(fromInvite)
	{}

	SDrxSteamSessionID& operator=(const SDrxSteamSessionID& other)
	{
		m_steamID = other.m_steamID;
		return *this;
	}

	// SDrxSessionID
	virtual bool operator==(const SDrxSessionID& other)
	{
		return (m_steamID == ((SDrxSteamSessionID&)other).m_steamID);
	}

	virtual bool operator<(const SDrxSessionID& other)
	{
		return (m_steamID < ((SDrxSteamSessionID&)other).m_steamID);
	}

	virtual bool IsFromInvite() const
	{
		return m_fromInvite;
	}

	virtual void AsCStr(char* pOutString, int inBufferSize) const
	{
		drx_sprintf(pOutString, inBufferSize, "%s", CSteamIDAsString(m_steamID).c_str());
	}
	// ~SDrxSessionID

	static uint32 GetSizeInPacket()
	{
		return sizeof(uint64);
	}

	void WriteToPacket(CDrxLobbyPacket* pPacket) const
	{
		pPacket->WriteUINT64(m_steamID.ConvertToUint64());
	}

	void ReadFromPacket(CDrxLobbyPacket* pPacket)
	{
		m_steamID.SetFromUint64(pPacket->ReadUINT64());
	}

	CSteamID m_steamID;
	bool     m_fromInvite;
};

////////////////////////////////////////////////////////////////////////////////

struct SDrxSteamUserID : public SDrxSharedUserID
{
	SDrxSteamUserID()
		: m_steamID(CSteamID())
	{}

	SDrxSteamUserID(CSteamID& steamID)
		: m_steamID(steamID)
	{}

	virtual bool operator==(const SDrxUserID& other) const
	{
		return (m_steamID == static_cast<const SDrxSteamUserID&>(other).m_steamID);
	}

	virtual bool operator<(const SDrxUserID& other) const
	{
		return (m_steamID < static_cast<const SDrxSteamUserID&>(other).m_steamID);
	}

	virtual DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> GetGUIDAsString() const
	{
		DrxFixedStringT<DRXLOBBY_USER_GUID_STRING_LENGTH> result(CSteamIDAsString(m_steamID));
		return result;
	};

	void WriteToPacket(CDrxLobbyPacket* pPacket) const
	{
		pPacket->WriteUINT64(m_steamID.ConvertToUint64());
	}

	void ReadFromPacket(CDrxLobbyPacket* pPacket)
	{
		m_steamID.SetFromUint64(pPacket->ReadUINT64());
	}

	SDrxSteamUserID& operator=(const SDrxSteamUserID& other)
	{
		m_steamID = other.m_steamID;
		return *this;
	}

	CSteamID m_steamID;
};

////////////////////////////////////////////////////////////////////////////////

class CDrxSteamMatchMaking : public CDrxMatchMaking
{
public:
	CDrxSteamMatchMaking(CDrxLobby* lobby, CDrxLobbyService* service, EDrxLobbyService serviceType);

	EDrxLobbyError Initialise();
	void           Tick(CTimeValue tv);

	void           OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	/* void						ProfileSettingsChanged(ULONG_PTR param);
	          void						UserSignedOut(uint32 user);
	 */

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
	virtual EDrxLobbyError SessionJoin(uint32* pUsers, int numUsers, uint32 flags, DrxSessionID id, DrxLobbyTaskID* pTaskID, DrxMatchmakingSessionJoinCallback pCB, void* pCBArg);
	virtual EDrxLobbyError SessionSetLocalUserData(DrxSessionHandle h, DrxLobbyTaskID* pTaskID, uint32 user, uint8* pData, uint32 dataSize, DrxMatchmakingCallback pCB, void* pCBArg);
	virtual DrxSessionID   SessionGetDrxSessionIDFromDrxSessionHandle(DrxSessionHandle h);

	virtual uint32         GetSessionIDSizeInPacket() const;
	virtual EDrxLobbyError WriteSessionIDToPacket(DrxSessionID sessionId, CDrxLobbyPacket* pPacket) const;
	virtual DrxSessionID   ReadSessionIDFromPacket(CDrxLobbyPacket* pPacket) const;

	virtual EDrxLobbyError SessionEnsureBestHost(DrxSessionHandle gh, DrxLobbyTaskID* pTaskID, DrxMatchmakingCallback pCB, void* pCBArg);
	#if NETWORK_HOST_MIGRATION
	virtual void           HostMigrationInitialise(DrxLobbySessionHandle h, EDisconnectionCause cause);
	virtual EDrxLobbyError HostMigrationServer(SHostMigrationInfo* pInfo);
	virtual bool           GetNewHostAddress(char* address, SHostMigrationInfo* pInfo);
	#endif

	SDrxSteamSessionID GetSteamSessionIDFromSession(DrxLobbySessionHandle lsh);

	//////////////////////////////////////////////////////////////////////////////
	// LobbyIDAddr housekeeping
	TNetAddress          GetHostAddressFromSessionHandle(DrxSessionHandle h);
	virtual void         LobbyAddrIDTick();
	virtual bool         LobbyAddrIDHasPendingData();
	virtual ESocketError LobbyAddrIDSend(const uint8* buffer, uint32 size, const TNetAddress& addr);
	virtual void         LobbyAddrIDRecv(void (* cb)(void*, uint8* buffer, uint32 size, DRXSOCKET sock, TNetAddress& addr), void* cbData);
	//////////////////////////////////////////////////////////////////////////////

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
		eT_SessionQuery,
		eT_SessionSetLocalUserData,
	};

	struct  SRegisteredUserData
	{
		SDrxSessionUserData data[MAX_MATCHMAKING_SESSION_USER_DATA];
		uint32              num;
	};

	struct SSession : public CDrxMatchMaking::SSession
	{
		typedef CDrxMatchMaking::SSession PARENT;

		virtual const char* GetLocalUserName(uint32 localUserIndex) const;
		virtual void        Reset();

		void                InitialiseLocalConnection(SDrxMatchMakingConnectionUID uid);

		SDrxSessionUserData userData[MAX_MATCHMAKING_SESSION_USER_DATA];
		SDrxSessionData     data;
		HANDLE              handle;
		DWORD               flags;
		DWORD               gameType;
		DWORD               gameMode;

		struct SLConnection : public CDrxMatchMaking::SSession::SLConnection
		{
			char            name[DRXLOBBY_USER_NAME_LENGTH];
			uint8           userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
			SDrxSteamUserID userID;
		}                     localConnection;

		struct SRConnection : public CDrxMatchMaking::SSession::SRConnection
		{
			char            name[DRXLOBBY_USER_NAME_LENGTH];
			uint8           userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES];
			SDrxSteamUserID userID;

			enum ERemoteConnectionState
			{
				eRCS_None,
				eRCS_WaitingToJoin,
				eRCS_Joining,
				eRCS_JoiningButWantToLeave,
				eRCS_Joined,
				eRCS_WaitingToLeave,
				eRCS_Leaving
			}    state;

			bool m_isDedicated;
	#if NETWORK_HOST_MIGRATION
			bool m_migrated;
	#endif
		};

		DrxLobbyIDArray<SRConnection, DrxMatchMakingConnectionID, MAX_LOBBY_CONNECTIONS> remoteConnection;
		SDrxSteamSessionID m_id;
	};

	struct  STask : public CDrxMatchMaking::STask
	{
		void Reset() {};
	};

	//////////////////////////////////////////////////////////////////////////////
	// Task housekeeping
	EDrxLobbyError StartTask(ETask etask, bool startRunning, DrxMatchMakingTaskID* mmTaskID, DrxLobbyTaskID* lTaskID, DrxLobbySessionHandle h, void* cb, void* cbArg);
	void           StartSubTask(ETask etask, DrxMatchMakingTaskID mmTaskID);
	void           StartTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void           StopTaskRunning(DrxMatchMakingTaskID mmTaskID);
	void           EndTask(DrxMatchMakingTaskID mmTaskID);
	bool           CheckTaskStillValid(DrxMatchMakingTaskID mmTaskID);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Remote connection housekeeping
	DrxMatchMakingConnectionID AddRemoteConnection(DrxLobbySessionHandle h, DrxLobbyConnectionID connectionID, SDrxMatchMakingConnectionUID uid, CSteamID steamID, uint32 numUsers, const char* name, uint8 userData[DRXLOBBY_USER_DATA_SIZE_IN_BYTES], bool isDedicated);
	virtual void               FreeRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Session housekeeping
	EDrxLobbyError CreateSessionHandle(DrxLobbySessionHandle* h, bool host, uint32 createFlags, int numUsers);
	virtual uint64 GetSIDFromSessionHandle(DrxLobbySessionHandle h);
	void           SetSessionUserData(DrxMatchMakingTaskID mmTaskID, SDrxSessionUserData* pData, uint32 numData);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Tasks
	void StartSessionCreate(DrxMatchMakingTaskID mmTaskID);

	void StartSessionMigrate(DrxMatchMakingTaskID mmTaskID);
	void TickSessionMigrate(DrxMatchMakingTaskID mmTaskID);

	void StartSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void TickSessionSearch(DrxMatchMakingTaskID mmTaskID);
	void EndSessionSearch(DrxMatchMakingTaskID mmTaskID);

	void StartSessionUpdate(DrxMatchMakingTaskID mmTaskID);
	void StartSessionUpdateSlots(DrxMatchMakingTaskID mmTaskID);

	void EndSessionQuery(DrxMatchMakingTaskID mmTaskID);

	void EndSessionGetUsers(DrxMatchMakingTaskID mmTaskID);

	void StartSessionStart(DrxMatchMakingTaskID mmTaskID);
	void TickSessionStart(DrxMatchMakingTaskID mmTaskID);

	void StartSessionEnd(DrxMatchMakingTaskID mmTaskID);
	void TickSessionEnd(DrxMatchMakingTaskID mmTaskID);

	void StartSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void TickSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void EndSessionJoin(DrxMatchMakingTaskID mmTaskID);
	void ProcessSessionRequestJoinResult(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);
	void ProcessSessionAddRemoteConnections(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);

	void StartSessionDelete(DrxMatchMakingTaskID mmTaskID);
	void TickSessionDelete(DrxMatchMakingTaskID mmTaskID);

	void StartSessionSetLocalUserData(DrxMatchMakingTaskID mmTaskID);
	void ProcessLocalUserData(const TNetAddress& addr, CDrxSharedLobbyPacket* pPacket);

	void InitialUserDataEvent(DrxLobbySessionHandle h);
	void SessionUserDataEvent(EDrxLobbySystemEvent event, DrxLobbySessionHandle h, DrxMatchMakingConnectionID id);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Steam callbacks are broadcast to all listeners
	DRXSTEAM_CALLBACK(CDrxSteamMatchMaking, OnSteamShutdown, SteamShutdown_t, m_SteamOnShutdown);
	DRXSTEAM_CALLBACK(CDrxSteamMatchMaking, OnLobbyDataUpdated, LobbyDataUpdate_t, m_SteamOnLobbyDataUpdated);
	DRXSTEAM_CALLBACK(CDrxSteamMatchMaking, OnP2PSessionRequest, P2PSessionRequest_t, m_SteamOnP2PSessionRequest);
	DRXSTEAM_CALLBACK(CDrxSteamMatchMaking, OnLobbyChatUpdate, LobbyChatUpdate_t, m_SteamOnLobbyChatUpdate);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Steam callresults target one listener
	DRXSTEAM_CALLRESULT(CDrxSteamMatchMaking, OnLobbyCreated, LobbyCreated_t, m_SteamOnLobbyCreated);
	DRXSTEAM_CALLRESULT(CDrxSteamMatchMaking, OnLobbySearchResults, LobbyMatchList_t, m_SteamOnLobbySearchResults);
	DRXSTEAM_CALLRESULT(CDrxSteamMatchMaking, OnLobbyEntered, LobbyEnter_t, m_SteamOnLobbyEntered);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Utility functions to send data back to the game on the game thread
	void SendSessionSearchResultsToGame(DrxMatchMakingTaskID mmTaskID, TMemHdl resultsHdl, TMemHdl userHdl);
	//////////////////////////////////////////////////////////////////////////////

	EDrxLobbyError ConvertSteamErrorToDrxLobbyError(uint32 steamErrorCode);

	#if !defined(_RELEASE)
	//////////////////////////////////////////////////////////////////////////////
	// Debug functions
	void Debug_DumpLobbyMembers(DrxLobbySessionHandle lsh);
	//////////////////////////////////////////////////////////////////////////////
	#endif // !defined(_RELEASE)

	SRegisteredUserData m_registeredUserData;
	DrxLobbyIDArray<SSession, DrxLobbySessionHandle, MAX_MATCHMAKING_SESSIONS> m_sessions;
	STask               m_task[MAX_MATCHMAKING_TASKS];
};

////////////////////////////////////////////////////////////////////////////////

#endif // USE_STEAM && USE_DRX_MATCHMAKING
#endif // __DRXSTEAMMATCHMAKING_H__
