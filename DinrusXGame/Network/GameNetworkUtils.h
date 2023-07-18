// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
Description:
	- Contains various shared network util functions
-------------------------------------------------------------------------
History:
	- 19/07/2010 : Created by Colin Gulliver

*************************************************************************/

#ifndef __GAMENETWORKUTILS_H__
#define __GAMENETWORKUTILS_H__

struct SSessionNames;

namespace GameNetworkUtils
{
	EDrxLobbyError SendToAll(CDrxLobbyPacket* pPacket, DrxSessionHandle h, SSessionNames &clients, bool bCheckConnectionState);

	const bool CompareDrxSessionId(const DrxSessionID &lhs, const DrxSessionID &rhs);

	void WebSafeEscapeString(string *ioString);
};

#endif //~__GAMENETWORKUTILS_H__


