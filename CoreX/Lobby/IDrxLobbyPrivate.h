// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#pragma once

<DinrusX/CoreX/Lobby/IDrxLobby.h>
#if NETWORK_HOST_MIGRATION
	#include "../../../DrxPlugins/Core/Lobby/Module/LobbyCVars.h"
#endif

struct IDrxLobbyPrivate : public IDrxLobby
{
#if NETWORK_HOST_MIGRATION
	virtual CLobbyCVars& GetCVars() = 0;
#endif
};
