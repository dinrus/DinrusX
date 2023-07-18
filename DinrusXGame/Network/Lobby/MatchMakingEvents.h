// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Classes representing events in matchmaking, migration
and session performance. Used to log telemetry of these systems
-------------------------------------------------------------------------
History:
- 13:06:2011 : Created By Andrew Blackwell

*************************************************************************/

//////////////////////////////////////////////////////////////////////////
//Header Guard
#ifndef __MATCHMAKINGEVENTS_H__
#define __MATCHMAKINGEVENTS_H__

//////////////////////////////////////////////////////////////////////////
//Base Class include
#include <DinrusX/DinrusXGame/RecordingBuffer.h>
//Session Name Length Definition
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>
//Session ID string and User Name length Definition
#include <DinrusX/CoreX/Lobby/IDrxLobby.h>

#define MAX_MM_RULES_DESC_LENGTH 64
#define MAX_MM_GENERIC_MESSAGE 128

#include <DinrusX/DinrusXGame/SessionNames.h>

//////////////////////////////////////////////////////////////////////////
// Base Class for MMTel Events

struct SMatchMakingEvent : public SRecording_Packet
{
	SMatchMakingEvent();

	CTimeValue m_timeStamp;
};

//////////////////////////////////////////////////////////////////////////
// Events for match making telemetry

struct SMMStartSearchEvent : public SMatchMakingEvent
{
	//needs the parameters used for the search
	SMMStartSearchEvent( SDrxSessionSearchParam& searchParameters, int32 searchID );

	int32	m_searchID;
	int32 m_version;
	int32 m_playlist;
	int32 m_variant;
	int32 m_numFreeSlots;
	int32 m_searchRegion;
	bool	m_ranked;
};

struct SMMFoundSessionEvent : public SMatchMakingEvent
{
	//needs the session/server details
	SMMFoundSessionEvent( SDrxSessionSearchResult* pSession, uint16 sessionStatus, int rankDiff, int32 region, int32 language, bool isBadServer, float score );

	char					m_sessionName[ MAX_SESSION_NAME_LENGTH ];
	char					m_sessionID[ DRXSESSIONID_STRINGLEN ];

	uint16				m_ping;
	uint16				m_filledSlots;
	uint16				m_status;
	int32					m_rankDiff;
	int32					m_region;
	int32					m_language;
	float					m_score;
	bool					m_badServer;


};

struct SMMChosenSessionEvent : public SMatchMakingEvent
{
	SMMChosenSessionEvent( const char* name, DrxSessionID id, const char* rulesDescription, bool created, int32 searchID, bool isPrimary );

	char					m_sessionName[ MAX_SESSION_NAME_LENGTH ];
	char					m_rulesDescription[ MAX_MM_RULES_DESC_LENGTH ];
	char					m_sessionID[ DRXSESSIONID_STRINGLEN ];
	bool					m_created;
	bool					m_primary;
	int32					m_searchID;

};

struct SMMNoServerSelectedEvent : public SMatchMakingEvent
{
	SMMNoServerSelectedEvent( const char* reason, int32 searchID );

	char					m_reason[ MAX_MM_RULES_DESC_LENGTH ];
	int32					m_searchID;
};

struct SMMSearchTimedOutEvent : public SMatchMakingEvent
{
	SMMSearchTimedOutEvent( bool searchingAgain, int32 searchID );
	
	int32		m_searchID;
	bool		m_searchingAgain;
};

struct SMMServerConnectFailedEvent : public SMatchMakingEvent
{
	SMMServerConnectFailedEvent( DrxSessionID& id, bool isPrimary, EDrxLobbyError m_errorCode );

	char	m_sessionID[ DRXSESSIONID_STRINGLEN ];
	EDrxLobbyError m_errorCode;
	bool	m_wasPrimary;
};

struct SMMMigrateHostLobbyEvent : public SMatchMakingEvent
{
	SMMMigrateHostLobbyEvent();
};

struct SMMBecomeHostEvent : public SMatchMakingEvent
{
	SMMBecomeHostEvent();
};

struct SMMDemotedToClientEvent : public SMatchMakingEvent
{
	SMMDemotedToClientEvent();
};

struct SMMMigrateCompletedEvent : public SMatchMakingEvent
{
	SMMMigrateCompletedEvent( DrxFixedStringT<DISPLAY_NAME_LENGTH>& newServer, DrxSessionID& sessionId );

	char m_newServer[ DISPLAY_NAME_LENGTH ];
	char	m_newSessionID[ DRXSESSIONID_STRINGLEN ];
};

struct SMMServerRequestingMerge : public SMatchMakingEvent
{
	 SMMServerRequestingMerge( DrxSessionID& currentSession, DrxSessionID& newSession );

	 char	m_currentSessionID[ DRXSESSIONID_STRINGLEN ];
	 char	m_newSessionID[ DRXSESSIONID_STRINGLEN ];
};

struct SMMMergeRequestedEvent : public SMatchMakingEvent
{
	SMMMergeRequestedEvent( DrxSessionID& id );

	char	m_sessionID[ DRXSESSIONID_STRINGLEN ];
};

struct SMMLaunchGameEvent : public SMatchMakingEvent
{
	SMMLaunchGameEvent( DrxSessionID& sessionId );

	char	m_sessionID[ DRXSESSIONID_STRINGLEN ];
};

struct SMMLeaveMatchMakingEvent : public SMatchMakingEvent
{
	SMMLeaveMatchMakingEvent();
};

//For when players join/leave during match making/quick match
struct SMMPlayerJoinedMMEvent : public SMatchMakingEvent
{
	SMMPlayerJoinedMMEvent( SDrxUserInfoResult* pUser, DrxSessionID& id, int32 nCurrentPlayers, bool isLocal );

	char			m_sessionID[ DRXSESSIONID_STRINGLEN ];
	char			m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
	char			m_userName[DRXLOBBY_USER_NAME_LENGTH];
	int32			m_nCurrentPlayers;
	bool			m_local;
};

struct SMMPlayerLeftMMEvent : public SMatchMakingEvent
{
	SMMPlayerLeftMMEvent( SDrxUserInfoResult* pUser, DrxSessionID& id, int32 nCurrentPlayers, bool isLocal );

	char			m_sessionID[ DRXSESSIONID_STRINGLEN ];
	char			m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
	char			m_userName[DRXLOBBY_USER_NAME_LENGTH];
	int32			m_nCurrentPlayers;
	bool			m_local;
};

//////////////////////////////////////////////////////////////////////////
// Events for session performance telemetry

struct SMMPlayerJoinedEvent : public SMatchMakingEvent
{
	SMMPlayerJoinedEvent( DrxFixedStringT<DRXLOBBY_USER_NAME_LENGTH>& name, DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID );

	char													m_name[DRXLOBBY_USER_NAME_LENGTH];
	char													m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
	SDrxMatchMakingConnectionUID	m_conUID;
};

struct SMMPlayerLeftEvent : public SMatchMakingEvent
{
	SMMPlayerLeftEvent( DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID );

	char													m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
	SDrxMatchMakingConnectionUID	m_conUID;
};

struct SMMPlayerPingEvent : public SMatchMakingEvent
{
	SMMPlayerPingEvent( DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID, DrxPing& ping );

	char													m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
	SDrxMatchMakingConnectionUID	m_conUID;
	DrxPing												m_ping;
};

struct SMMPlayerReportLagEvent : public SMatchMakingEvent
{
	SMMPlayerReportLagEvent( DrxUserID& uid );

	char			m_guid[DRXLOBBY_USER_GUID_STRING_LENGTH];
};

struct SMMGenericLogEvent : public SMatchMakingEvent
{
	SMMGenericLogEvent( const char* pMessage, bool isError );

	char			m_message[MAX_MM_GENERIC_MESSAGE];
	bool			m_bError;
};


#endif // __MATCHMAKINGEVENTS_H__