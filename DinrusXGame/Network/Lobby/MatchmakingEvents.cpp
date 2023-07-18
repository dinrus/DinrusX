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

#include <DinrusX/DinrusXGame/StdAfx.h>
//////////////////////////////////////////////////////////////////////////
// This Include
#include <DinrusX/DinrusXGame/MatchMakingEvents.h>

#include <DinrusX/DinrusXGame/MatchMakingTelemetry.h>
#include <DinrusX/DinrusXGame/GameLobbyData.h>

SMatchMakingEvent::SMatchMakingEvent()
{
	m_timeStamp = gEnv->pTimer->GetFrameStartTime(ITimer::ETIMER_UI);
}

SMMStartSearchEvent::SMMStartSearchEvent( SDrxSessionSearchParam& searchParameters, int32 searchID )
{
	m_numFreeSlots = searchParameters.m_numFreeSlots;;
	m_ranked = searchParameters.m_ranked;
	m_searchID = searchID;

	m_version = -1;
	m_playlist = -1;
	m_variant = -1;
	m_searchRegion = -1;

	for( uint32 iNumData = 0; iNumData < searchParameters.m_numData; iNumData++ )
	{
		DrxLobbyUserDataID id = searchParameters.m_data[iNumData].m_data.m_id;

		if (id == LID_MATCHDATA_VERSION)
		{
			m_version = searchParameters.m_data[iNumData].m_data.m_int32;
		}
		else if (id == LID_MATCHDATA_PLAYLIST)
		{
			m_playlist = searchParameters.m_data[iNumData].m_data.m_int32;
		}
		else if (id == LID_MATCHDATA_VARIANT)
		{
			m_variant = searchParameters.m_data[iNumData].m_data.m_int32;
		}
//Consoles use COUNTRY
#if GAMELOBBY_USE_COUNTRY_FILTERING
		else if (id == LID_MATCHDATA_COUNTRY)
		{
			m_searchRegion = searchParameters.m_data[iNumData].m_data.m_int32;
		}
#endif //GAMELOBBY_USE_COUNTRY_FILTERING
	}

	size = sizeof( SMMStartSearchEvent );
	type = eMMTelE_StartSearch;
}

//cppcheck-suppress uninitMemberVar
SMMFoundSessionEvent::SMMFoundSessionEvent( SDrxSessionSearchResult* pSession, uint16 sessionStatus, int rankDiff, int32 region, int32 language, bool isBadServer, float score )
{
	cry_strcpy( m_sessionName, pSession->m_data.m_name );

	pSession->m_id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	m_ping = pSession->m_ping;
	m_filledSlots = pSession->m_numFilledSlots;
	m_status = sessionStatus;
	m_rankDiff = rankDiff;
	m_badServer = isBadServer;
	m_score = score;
	m_region = region;
	m_language = language;

	size = sizeof( SMMFoundSessionEvent );
	type = eMMTelE_FoundSession;
}

SMMChosenSessionEvent::SMMChosenSessionEvent( const char* name, DrxSessionID id, const char* rulesDescription, bool created, int32 searchID, bool primary  )
{
	cry_strcpy( m_sessionName, name );
	cry_strcpy( m_rulesDescription, rulesDescription );
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	m_created = created;
	m_primary = primary;
	m_searchID = searchID;

	size = sizeof( SMMChosenSessionEvent );
	type = eMMTelE_ChosenSession;
}

SMMNoServerSelectedEvent::SMMNoServerSelectedEvent( const char* reason, int32 searchID )
{
	cry_strcpy( m_reason, reason );
	m_searchID = searchID;

	size = sizeof( SMMNoServerSelectedEvent );
	type = eMMTelE_NoServerSelected;
}

SMMSearchTimedOutEvent::SMMSearchTimedOutEvent( bool searchingAgain, int32 searchID )
{
	m_searchingAgain = searchingAgain;
	m_searchID = searchID;

	size = sizeof( SMMSearchTimedOutEvent );
	type = eMMTelE_SearchTimedOut;
}

SMMServerConnectFailedEvent::SMMServerConnectFailedEvent( DrxSessionID& id, bool isPrimary, EDrxLobbyError errorCode )
{	
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	m_wasPrimary = isPrimary;
	m_errorCode = errorCode;

	size = sizeof( SMMServerConnectFailedEvent );
	type = eMMTelE_ServerConnectFailed;
}

SMMMigrateHostLobbyEvent::SMMMigrateHostLobbyEvent()
{
	size = sizeof( SMMMigrateHostLobbyEvent );
	type = eMMTelE_MigrateHostLobby;
}

SMMBecomeHostEvent::SMMBecomeHostEvent()
{
	size = sizeof( SMMBecomeHostEvent );
	type = eMMTelE_BecomeServer;
}

SMMDemotedToClientEvent::SMMDemotedToClientEvent()
{
	size = sizeof( SMMDemotedToClientEvent );
	type = eMMTelE_DemotedToClient;
}

SMMMigrateCompletedEvent::SMMMigrateCompletedEvent( DrxFixedStringT<DISPLAY_NAME_LENGTH>& newServer, DrxSessionID& sessionId )
{
	cry_strcpy( m_newServer, newServer.c_str() );
	sessionId->AsCStr( m_newSessionID, sizeof(m_newSessionID) );

	size = sizeof( SMMMigrateCompletedEvent );
	type = eMMTelE_MigrateCompleted;
}

SMMServerRequestingMerge::SMMServerRequestingMerge( DrxSessionID& currentSession, DrxSessionID& newSession )
{
	currentSession->AsCStr( m_currentSessionID, sizeof( m_currentSessionID) );
	newSession->AsCStr( m_newSessionID, sizeof(m_newSessionID) );

	size = sizeof( SMMServerRequestingMerge );
	type = eMMTelE_ServerRequestMerge;
}

SMMMergeRequestedEvent::SMMMergeRequestedEvent( DrxSessionID& id )
{
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	size = sizeof( SMMMergeRequestedEvent );
	type = eMMTelE_MergeRequested;
}

SMMLaunchGameEvent::SMMLaunchGameEvent( DrxSessionID& id )
{
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	size = sizeof( SMMLaunchGameEvent );
	type = eMMTelE_LaunchGame;
}

SMMLeaveMatchMakingEvent::SMMLeaveMatchMakingEvent()
{
	size = sizeof( SMMLeaveMatchMakingEvent );
	type = eMMTelE_LeaveMatchMaking;
}


SMMPlayerJoinedMMEvent::SMMPlayerJoinedMMEvent( SDrxUserInfoResult* pUser, DrxSessionID& id, int32 nCurrentPlayers, bool isLocal )
{
	DrxFixedStringT<40> guid = pUser->m_userID.get()->GetGUIDAsString();

	DrxLog( "MMTel: User guid %s", guid.c_str() );
	if( pUser->m_userID.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), guid.c_str() );
	}
	else
	{
		m_guid[0]=0;
	}

	memcpy( m_userName, pUser->m_userName, sizeof(m_userName) );
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	m_nCurrentPlayers = nCurrentPlayers;
	m_local = isLocal;

	size = sizeof( SMMPlayerJoinedMMEvent );
	type = eMMTelE_PlayerJoinedMM;
}

SMMPlayerLeftMMEvent::SMMPlayerLeftMMEvent( SDrxUserInfoResult* pUser, DrxSessionID& id, int32 nCurrentPlayers, bool isLocal )
{
	DrxFixedStringT<40> guid = pUser->m_userID.get()->GetGUIDAsString();

	DrxLog( "MMTel: User guid %s", guid.c_str() );
	if( pUser->m_userID.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), guid.c_str() );
	}
	else
	{
		m_guid[0]=0;
	}

	memcpy( m_userName, pUser->m_userName, sizeof(m_userName) );
	id->AsCStr( m_sessionID, sizeof(m_sessionID) );

	m_nCurrentPlayers = nCurrentPlayers;
	m_local = isLocal;

	size = sizeof( SMMPlayerLeftMMEvent );
	type = eMMTelE_PlayerLeftMM;
}

//////////////////////////////////////////////////////////////////////////
// Session Quality Events
SMMPlayerJoinedEvent::SMMPlayerJoinedEvent( DrxFixedStringT<DRXLOBBY_USER_NAME_LENGTH>& name, DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID )
{
	if( uid.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), uid.get()->GetGUIDAsString().c_str() );
	}
	else
	{
		m_guid[0]=0;
	}
	m_conUID = conUID;

	cry_sprintf( m_name, sizeof(m_name), name.c_str() );

	size = sizeof( SMMPlayerJoinedEvent );
	type = eMMTelE_PlayerJoined;
}

SMMPlayerLeftEvent::SMMPlayerLeftEvent( DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID)
{	
	if( uid.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), uid.get()->GetGUIDAsString().c_str() );
	}
	else
	{
		m_guid[0]=0;
	}
	m_conUID = conUID;

	size = sizeof( SMMPlayerLeftEvent );
	type = eMMTelE_PlayerLeft;
}

SMMPlayerPingEvent::SMMPlayerPingEvent( DrxUserID& uid, SDrxMatchMakingConnectionUID& conUID, DrxPing& ping )
{
	if( uid.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), uid.get()->GetGUIDAsString().c_str() );
	}
	else
	{
		m_guid[0]=0;
	}
	m_conUID = conUID;
	m_ping = ping;

	size = sizeof( SMMPlayerPingEvent );
	type = eMMTelE_PlayerPing;
}

SMMPlayerReportLagEvent::SMMPlayerReportLagEvent( DrxUserID& uid )
{	
	if( uid.get() )
	{
		cry_sprintf( m_guid, sizeof(m_guid), uid.get()->GetGUIDAsString().c_str() );
	}
	else
	{
		m_guid[0]=0;
	}
	size = sizeof( SMMPlayerReportLagEvent );
	type = eMMTelE_PlayerReportLag;
}


SMMGenericLogEvent::SMMGenericLogEvent( const char* pMessage, bool isError )
{
	cry_strcpy( m_message, pMessage );
	m_bError = isError;

	size = sizeof( SMMGenericLogEvent );
	type = eMMTelE_GenericLog;
}
