// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script bindings needed to implement matchmaking in Lua
Hopefully covering things we may need in the future
-------------------------------------------------------------------------
History:
- 01:08:2011 : Created By Andrew Blackwell

*************************************************************************/
#include <DinrusX/DinrusXGame/StdAfx.h>

//////////////////////////////////////////////////////////////////////////
// This Include
#include <DinrusX/DinrusXGame/ScriptBind_MatchMaking.h>

#include <DinrusX/DinrusXGame/MatchMakingHandler.h>

#include <DinrusX/DinrusXGame/DLCUpr.h>
#include <DinrusX/DinrusXGame/Network/Squad/SquadUpr.h>
#include <DinrusX/DinrusXGame/GameLobbyUpr.h>
#include <DinrusX/DinrusXGame/GameCVars.h>

//------------------------------------------------------------------------
// Constructor
CScriptBind_MatchMaking::CScriptBind_MatchMaking( ISystem *pSystem )
: m_pSystem( pSystem ),
	m_pLobbyUpr( NULL )
{
	Init( pSystem->GetIScriptSystem(), m_pSystem, 1 );

	RegisterMethods();
	RegisterGlobals();
}

//------------------------------------------------------------------------
// Destructor
CScriptBind_MatchMaking::~CScriptBind_MatchMaking()
{
}

//------------------------------------------------------------------------
void CScriptBind_MatchMaking::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_MatchMaking::

	SCRIPT_REG_TEMPLFUNC( IsSquadLeaderOrSolo, "");
	SCRIPT_REG_TEMPLFUNC( CancelSearch, "" );
	SCRIPT_REG_TEMPLFUNC( IsJoiningSession, "" );
	SCRIPT_REG_TEMPLFUNC( IsSessionHost, "" );
	SCRIPT_REG_TEMPLFUNC( IsInSession, "" );
	SCRIPT_REG_TEMPLFUNC( HasGameStarted, "" );
	SCRIPT_REG_TEMPLFUNC( HaveEnoughPlayersToStart, "" );
	SCRIPT_REG_TEMPLFUNC( GetNumPlayersInCurrentSession, "" );
	SCRIPT_REG_TEMPLFUNC( GetNumPlayersInSquad, "" );
	SCRIPT_REG_TEMPLFUNC( GetMaxNumPlayers, "" );

	SCRIPT_REG_TEMPLFUNC( GetCurrentRegion, "" );
	SCRIPT_REG_TEMPLFUNC( GetCurrentLanguage, "" );
	SCRIPT_REG_TEMPLFUNC( GetAverageSkillScore, "" );

	SCRIPT_REG_TEMPLFUNC( GetCurrentMatchMakingVersionNum, "" );

	SCRIPT_REG_TEMPLFUNC( GetCurrentPlaylist, "" );
	SCRIPT_REG_TEMPLFUNC( GetCurrentVariant, "" );
	SCRIPT_REG_TEMPLFUNC( GetCurrentPing, "" );
//	SCRIPT_REG_TEMPLFUNC( GetCurrentGameMode, "" );
//	SCRIPT_REG_TEMPLFUNC( GetCurrentMap, "" );

	SCRIPT_REG_TEMPLFUNC( GetAvailableDLCs, "" );

	SCRIPT_REG_TEMPLFUNC( StartSearch, "freeSlots,maxResults,searchParams" );

	SCRIPT_REG_TEMPLFUNC( MergeWithServer, "sessionId" );
	SCRIPT_REG_TEMPLFUNC( JoinServer, "sessionId" );
	SCRIPT_REG_TEMPLFUNC( CreateServer, "sessionParams" );

	SCRIPT_REG_TEMPLFUNC( RequestUpdateCall, "timeToCall" );

	SCRIPT_REG_TEMPLFUNC( MMLog, "message,isError" );
}

//------------------------------------------------------------------------
void CScriptBind_MatchMaking::RegisterGlobals()
{
	//operators for MM searches
	SCRIPT_REG_GLOBAL( eCSSO_Equal );
	SCRIPT_REG_GLOBAL( eCSSO_NotEqual );
	SCRIPT_REG_GLOBAL( eCSSO_LessThan );
	SCRIPT_REG_GLOBAL( eCSSO_LessThanEqual );
	SCRIPT_REG_GLOBAL( eCSSO_GreaterThan );
	SCRIPT_REG_GLOBAL( eCSSO_GreaterThanEqual );
	SCRIPT_REG_GLOBAL( eCSSO_BitwiseAndNotEqualZero );
}

//------------------------------------------------------------------------
void CScriptBind_MatchMaking::AttachTo( CMatchMakingHandler* pMatchMaking, CGameLobbyUpr *pLobbyUpr )
{
	m_pLobbyUpr = pLobbyUpr;
	IScriptTable *pScriptTable = pMatchMaking->GetScriptTable();

	if (pScriptTable)
	{
		SmartScriptTable thisTable(m_pSS);
		thisTable->Delegate(GetMethodsTable());

		pScriptTable->SetValue("bindings", thisTable);
	}
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::IsSquadLeaderOrSolo( IFunctionHandler *pH )
{
	if( g_pGame )
	{
		if( CSquadUpr* pSquadUpr = g_pGame->GetSquadUpr() )
		{
			if( (!pSquadUpr->InSquad()) || pSquadUpr->InCharge() )
			{
				return pH->EndFunction(1);
			}
		}
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::CancelSearch( IFunctionHandler *pH )
{
	if( CMatchMakingHandler* pMMHandler = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		pMMHandler->CancelSearch();
	}
	
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::IsJoiningSession( IFunctionHandler *pH )
{
	//if lobby state is joining we are joining
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{	
		if( pLobby->IsCreatingOrJoiningSession() )
		{
			return pH->EndFunction(1);
		}
	}

	//also check next lobby
	if( CGameLobby* pNextLobby = m_pLobbyUpr->GetNextGameLobby() )
	{
		if( pNextLobby->IsCreatingOrJoiningSession() )
		{
			return pH->EndFunction(1);
		}
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::IsSessionHost( IFunctionHandler *pH )
{
	//assume this means are we the primary session host
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{	
		if( pLobby->IsServer() )
		{
			return pH->EndFunction(1);
		}
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::IsInSession( IFunctionHandler *pH )
{
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{	
		if( pLobby->IsCurrentlyInSession() )
		{
			return pH->EndFunction(1);
		}
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::HasGameStarted( IFunctionHandler *pH )
{
	//if lobby state is lobby, game etc. we are in a session
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{	
		ELobbyState currentState = pLobby->GetState();
		if( currentState == eLS_PreGame || currentState == eLS_Game || currentState == eLS_EndSession || currentState == eLS_GameEnded )
		{
			return pH->EndFunction(1);
		}
	}

	return pH->EndFunction(0);
}


//------------------------------------------------------------------------
int CScriptBind_MatchMaking::HaveEnoughPlayersToStart( IFunctionHandler *pH )
{
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{
		int players = pLobby->GetSessionNames().Size();
		int playersNeeded = g_pGameCVars->g_minplayerlimit;

		if( players >= playersNeeded )
		{
			return pH->EndFunction( 1 );
		}
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::GetNumPlayersInCurrentSession( IFunctionHandler *pH )
{
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{
		int players = pLobby->GetSessionNames().Size();
		
		return pH->EndFunction( players );
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::GetNumPlayersInSquad( IFunctionHandler* pH )
{
	if( CSquadUpr* pSquadMan = g_pGame->GetSquadUpr() )
	{
		int players = pSquadMan->GetSquadSize();
		return pH->EndFunction( players );
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::GetMaxNumPlayers( IFunctionHandler* pH )
{
	return pH->EndFunction( MAX_PLAYER_LIMIT );
}

const int numSearchIdTypes = 12;
struct SearchKeyStringIDPair
{
	const char* key;
	ELOBBYIDS id;
};

SearchKeyStringIDPair idLookup[ numSearchIdTypes ] = 
{
	{	"gamemode", LID_MATCHDATA_GAMEMODE },
	{	"gamemap", LID_MATCHDATA_MAP },
	{	"active", LID_MATCHDATA_ACTIVE },
	{	"version", LID_MATCHDATA_VERSION },
	{	"required_dlcs", LID_MATCHDATA_REQUIRED_DLCS },
	{	"playlist", LID_MATCHDATA_PLAYLIST },
	{	"variant", LID_MATCHDATA_VARIANT },
	{	"skill", LID_MATCHDATA_SKILL },
	{	"language", LID_MATCHDATA_LANGUAGE },
//	Misc search parameters require support
//	{	"misc_1", LID_MATCHDATA_MISC1 },
//	{	"misc_2", LID_MATCHDATA_MISC2 },
//	{	"misc_3", LID_MATCHDATA_MISC3 },
};

//------------------------------------------------------------------------
int CScriptBind_MatchMaking::StartSearch( IFunctionHandler *pH, int freeSlotsRequired, int maxResults, SmartScriptTable searchParams )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		int dataIndex = 0;
		SDrxSessionSearchData data[ FIND_GAMES_SEARCH_NUM_DATA ];
		//for every type of data we can search on
		for( uint32 iKey = 0; iKey < numSearchIdTypes; iKey++ )
		{
			//see if there is a key for it in search params table
			SmartScriptTable entry;
			if( searchParams->GetValue( idLookup[ iKey ].key, entry ) )
			{
				//if there is read the value and operator
				ScriptAnyValue valueVal;
				int operatorVal;

				if( entry->GetValueAny( "val", valueVal ) )
				{
					if( entry->GetValue( "operator", operatorVal ) )
					{	
						//and set them into the search data
						data[ dataIndex ].m_data.m_id = idLookup[ iKey ].id;
					
						switch( valueVal.type )
						{
						case ANY_TNUMBER:
							data[ dataIndex ].m_data.m_type = eCLUDT_Int32;
							data[ dataIndex ].m_data.m_int32 = (int32)valueVal.number;
							break;

						case ANY_TBOOLEAN:
							data[ dataIndex ].m_data.m_type = eCLUDT_Int32;
							data[ dataIndex ].m_data.m_int32 = (int32)valueVal.b;
							break;

						case ANY_THANDLE:
							data[ dataIndex ].m_data.m_type = eCLUDT_Int32;
							data[ dataIndex ].m_data.m_int32 = (int32)(TRUNCATE_PTR)valueVal.ptr;
							break;

						default:
							MMLog( pH, "MMLua: Unsupported type in search data", true );
						}
						
						//DrxLog( "MMLua: Search Session Parameter, id %d, value %d, operator %d", data[ dataIndex ].m_data.m_id, data[ dataIndex ].m_data.m_int32, operatorVal );
						data[ dataIndex ].m_operator = static_cast<EDrxSessionSearchOperator>(operatorVal);
						dataIndex++;
					}
				}
			}
		}
	
		//pass the final search on to the handler
		pMatchMaking->Search( freeSlotsRequired, maxResults, data, dataIndex );
	}

	return pH->EndFunction( true );
}

int CScriptBind_MatchMaking::JoinServer( IFunctionHandler *pH, int sessionId )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		pMatchMaking->Join( sessionId );
	}

	return pH->EndFunction();
}

int CScriptBind_MatchMaking::CreateServer( IFunctionHandler *pH, SmartScriptTable sessionParams )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		//get rid of any stale settings
		pMatchMaking->ClearSessionParameters();

		//process the parameters

		//for every type of data sessions can broadcast
		for( uint32 iKey = 0; iKey < numSearchIdTypes; iKey++ )
		{
			//see if there is a key for it in session params table
			ScriptAnyValue valueVal;
			if( sessionParams->GetValueAny( idLookup[ iKey ].key, valueVal ) )
			{
				if( valueVal.type != ANY_TNIL )
				{
					//if there is add it to our parameters
					pMatchMaking->NewSessionParameter( idLookup[ iKey ].id, valueVal );
				}
			}
		}
	}

	//ask the networking to create a session
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{
		pLobby->FindGameCreateGame();
	}

	return pH->EndFunction();
}

int CScriptBind_MatchMaking::MergeWithServer( IFunctionHandler *pH, int sessionId )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		pMatchMaking->Merge( sessionId );
	}

	return pH->EndFunction();
}

int CScriptBind_MatchMaking::RequestUpdateCall( IFunctionHandler* pH, float timeToCall )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		pMatchMaking->RequestSubscribedUpdate( timeToCall );
	}

	return pH->EndFunction();
}

int CScriptBind_MatchMaking::MMLog( IFunctionHandler *pH, const char* message, bool isError )
{
	if( CMatchMakingHandler* pMatchMaking = m_pLobbyUpr->GetMatchMakingHandler() )
	{
		pMatchMaking->MMLog( message, isError );
	}

	return pH->EndFunction();
}

int CScriptBind_MatchMaking::GetCurrentRegion( IFunctionHandler* pH )
{
	//TODO: sort out how we want to handle region/country filtering on each platform (C2 PC did no region filtering)
	int regValue = 0;
#if GAMELOBBY_USE_COUNTRY_FILTERING
	regValue = g_pGame->GetUserRegion();
#endif

	return pH->EndFunction( ScriptHandle( regValue ) );
}

int CScriptBind_MatchMaking::GetCurrentLanguage( IFunctionHandler* pH )
{
	return pH->EndFunction( gEnv->pSystem->GetPlatformOS()->GetSystemLanguage() );
}

int CScriptBind_MatchMaking::GetCurrentMatchMakingVersionNum( IFunctionHandler* pH )
{
	return pH->EndFunction( ScriptHandle( GameLobbyData::GetVersion() ) );
}

int CScriptBind_MatchMaking::GetCurrentPlaylist( IFunctionHandler* pH )
{
	return pH->EndFunction( ScriptHandle( (int)GameLobbyData::GetPlaylistId() ) );
}

int CScriptBind_MatchMaking::GetCurrentVariant( IFunctionHandler* pH )
{
	return pH->EndFunction( GameLobbyData::GetVariantId() );
}

int CScriptBind_MatchMaking::GetAvailableDLCs( IFunctionHandler* pH )
{
	return pH->EndFunction( g_pGame->GetDLCUpr()->GetSquadCommonDLCs() );
}


int CScriptBind_MatchMaking::GetAverageSkillScore( IFunctionHandler* pH )
{
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{
		return pH->EndFunction( pLobby->CalculateAverageSkill() );
	}

	return pH->EndFunction( 0 );
}

int CScriptBind_MatchMaking::GetCurrentPing( IFunctionHandler* pH )
{
	if( CGameLobby* pLobby = m_pLobbyUpr->GetGameLobby() )
	{
		return pH->EndFunction( pLobby->GetCurrentPingToHost() );
	}

	return pH->EndFunction( 0 );
}

/*
//TODO - support individual map + game mode searches if required
int CScriptBind_MatchMaking::GetCurrentGameMode( IFunctionHandler* pH )
{

}

int CScriptBind_MatchMaking::GetCurrentMap( IFunctionHandler* pH )
{

}
*/