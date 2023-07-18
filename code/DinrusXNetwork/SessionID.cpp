// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:  maintains a globally unique id for game sessions
   -------------------------------------------------------------------------
   History:
   - 27/02/2006   12:34 : Created by Craig Tiller
*************************************************************************/

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/SessionID.h>

namespace
{

string GetDigestString(const char* hostname, int counter, time_t start)
{
	string s;
	s.Format("%s:%.8x:%.8" PRIx64 "", hostname, counter, (int64) start);
	return s;
}

}

CSessionID::CSessionID()
{
}

CSessionID::CSessionID(const char* hostname, int counter, time_t start) : m_hash(GetDigestString(hostname, counter, start))
{
}
