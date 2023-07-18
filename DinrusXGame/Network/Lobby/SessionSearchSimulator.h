// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Class for returning a fake session list to session searches.
Aim is to use this to test match making performance.
The fake session lists are loaded from XML files.

-------------------------------------------------------------------------
History:
- 20:07:2011 : Created By Andrew Blackwell

*************************************************************************/

//////////////////////////////////////////////////////////////////////////
//Header Guard
#ifndef __SESSIONSEARCHSIMULATOR_H__
#define __SESSIONSEARCHSIMULATOR_H__

//For SDrxSessionID and DRXSESSIONID_STRINGLEN
#include <DinrusX/CoreX/Lobby/IDrxLobby.h>
//For DrxMatchmakingSessionSearchCallback
#include <DinrusX/CoreX/Lobby/IDrxMatchMaking.h>


//////////////////////////////////////////////////////////////////////////
//The session search simulator class
class CSessionSearchSimulator
{
public:
	CSessionSearchSimulator();
	~CSessionSearchSimulator();

	bool OpenSessionListXML( const char* filepath );
	bool OutputSessionListBlock( DrxLobbyTaskID& taskID, DrxMatchmakingSessionSearchCallback cb, void* cbArg );

	//Inlines
	const char* GetCurrentFilepath() { return m_currentFilepath.c_str(); }

private:
	DrxFixedStringT< IDrxPak::g_nMaxPath >	m_currentFilepath;

	XmlNodeRef	m_sessionListXML;
	int	m_currentNode;
};

//////////////////////////////////////////////////////////////////////////
// Fake Session IDs used by Session Search Simulator.
// They are only required to display session IDs read in as strings
struct SDrxFakeSessionID : public SDrxSessionID
{
	char m_idStr[DRXSESSIONID_STRINGLEN];
	
	bool operator == (const SDrxSessionID &other)
	{
		char otherIdStr[DRXSESSIONID_STRINGLEN];
		other.AsCStr( otherIdStr, DRXSESSIONID_STRINGLEN );
		return ( strcmpi( m_idStr, otherIdStr ) == 0 );
	}
	
	bool operator < (const SDrxSessionID &other)
	{
		char otherIdStr[DRXSESSIONID_STRINGLEN];
		other.AsCStr( otherIdStr, DRXSESSIONID_STRINGLEN );
		return ( strcmpi( m_idStr, otherIdStr ) < 0 );
	}
	
	bool IsFromInvite() const
	{
		return false;
	}

	void AsCStr( char* pOutString, int inBufferSize ) const
	{
		if (pOutString && inBufferSize > 0)
		{
			size_t len = inBufferSize > sizeof(m_idStr) ? sizeof(m_idStr) : inBufferSize;
			memcpy( pOutString, m_idStr, len);
			pOutString[len-1]=0;
		}
	}
};


#endif	//__SESSIONSEARCHSIMULATOR_H__

