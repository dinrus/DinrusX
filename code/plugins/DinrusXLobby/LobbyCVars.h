// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __LOBBYCVARS_H__
#define __LOBBYCVARS_H__

#pragma once

<DinrusX/CoreX/Lobby/CommonIDrxLobby.h>
<DinrusX/DinrusXSys/IConsole.h>

class CLobbyCVars
{
public:
	CLobbyCVars();
	~CLobbyCVars();

	static CLobbyCVars& Get() { return *m_pThis; }

	float minMicrophonNotificationInterval;
	float serverPingNotificationInterval;
	int   showMatchMakingTasks;
	int   fullStatusOnClient;
	int   lobbyDefaultPort;

#if NETWORK_HOST_MIGRATION
	float hostMigrationTimeout;
	int   netAutoMigrateHost;
	int   showNewHostHinting;
	int   showHostIdentification;
	int   netHostHintingNATTypeOverride;
	int   netHostHintingActiveConnectionsOverride;
	int   netHostHintingPingOverride;
	int   netHostHintingUpstreamBPSOverride;
	int   netPruneConnectionsAfterHostMigration;
	int   netAnticipatedNewHostLeaves;
	int   netAlternateDeferredDisconnect;
	#if ENABLE_HOST_MIGRATION_STATE_CHECK
	int   doHostMigrationStateCheck;
	#endif
#endif

#if ENABLE_DRXLOBBY_DEBUG_TESTS
	int cldErrorPercentage;
	int cldMinDelayTime;
	int cldMaxDelayTime;
	int cldEnable;
#endif

#if DRX_PLATFORM_DURANGO
	int p2pShowConnectionStatus;
#endif

#if DRX_PLATFORM_ORBIS
	int psnForceWorld;
	int psnBestWorldMaxRooms;
	int psnBestWorldNumRoomsBonus;
	f32 psnBestWorldNumRoomsPivot;
	int psnBestWorldPlayersPerRoomBonus;
	f32 psnBestWorldPlayersPerRoomPivot;
	int psnBestWorldRandomBonus;
	int psnShowRooms;
#endif

#if USE_STEAM
	int useSteamAsOnlineLobby;
	int resetSteamAchievementsOnBoot;
	int lobbySteamOnlinePort;
#endif // USE_STEAM

#if defined(DEDICATED_SERVER)
	int ReservedSlotsActive;
#endif

	ICVar* pDedicatedServerArbitratorIP;
	int    dedicatedServerArbitratorPort;

private:
#if NETWORK_HOST_MIGRATION
	static void EnsureBestHostForSessionCmd(IConsoleCmdArgs* pArgs);
#endif

#if USE_DRX_DEDICATED_SERVER_ARBITRATOR
	static void DedicatedServerArbitratorListFreeServersCmd(IConsoleCmdArgs* pArgs);
#endif

	static CLobbyCVars * m_pThis;
};

#endif // __LOBBYCVARS_H__
