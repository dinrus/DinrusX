// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#if !defined(__DRXHOSTMIGRATION_H__)
#define __DRXHOSTMIGRATION_H__

#pragma once

#if NETWORK_HOST_MIGRATION

	<DinrusX/DinrusXNetwork/INetwork.h>

struct SHostMigrationInfo_Private;

class CHostMigration : public CMultiThreadRefCount
{
public:
	CHostMigration(void) { AddRef(); }
	~CHostMigration(void) {}

	void Start(SHostMigrationInfo_Private* pInfo);
	void Update(SHostMigrationInfo_Private* pInfo);
	void Terminate(SHostMigrationInfo_Private* pInfo);
	void Reset(SHostMigrationInfo_Private* pInfo);

	bool IsDead(void) { return false; }

protected:
	void ResetListeners(SHostMigrationInfo_Private* pInfo);
	void HandleListenerReturn(SHostMigrationEventListenerInfo& li, int sessionIndex, SHostMigrationInfo_Private* pInfo, IHostMigrationEventListener::EHostMigrationReturn listenerReturn, bool ignoreTerminate);
};
#endif
#endif //__DRXHOSTMIGRATION_H__
