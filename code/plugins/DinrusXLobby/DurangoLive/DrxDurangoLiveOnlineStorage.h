// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: CDrxDurangoLiveOnlineStorage implementation for Durango

   assuming:
   - TMS for user storage (global TMS, which we don't care)
   - CMS for global storage

   -------------------------------------------------------------------------
   History:
   - 26:06:2013 : Created by Yeonwoon JUNG

*************************************************************************/

#ifndef __DRXDURANGOLIVEONLINESTORAGE_H__
#define __DRXDURANGOLIVEONLINESTORAGE_H__

#pragma once

<DinrusX/DinrusXNetwork/IDrxOnlineStorage.h>

#include "DrxLobby.h"

#if USE_DURANGOLIVE && USE_DRX_ONLINE_STORAGE

	#define MAXIMUM_PENDING_QUERIES (8)

enum EDrxOnlineStorageStatus
{
	eCOSS_Idle,       //No queue item is being processed
	eCOSS_Working,    //A queue item is being processed
};

class CDrxLobby;
class CDrxLobbyService;
class CAsyncInfoPimpl;

class CDrxDurangoLiveOnlineStorage : public CMultiThreadRefCount, public IDrxOnlineStorage
{
public:
	CDrxDurangoLiveOnlineStorage(CDrxLobby* pLobby, CDrxLobbyService* pService);

	virtual void   Tick(CTimeValue tv) throw();

	EDrxLobbyError Initialise()   { return eCLE_Success; }
	EDrxLobbyError Terminate()    { return eCLE_Success; }

	virtual bool   IsDead() const { return false; }

	//Add an operation to the queue
	virtual EDrxLobbyError OnlineStorageDataQuery(DrxOnlineStorageQueryData* pQueryData);

	void                   TriggerCallbackOnGameThread(const DrxOnlineStorageQueryData& QueryData);

protected:
	DrxOnlineStorageQueryData        m_pendingQueries[MAXIMUM_PENDING_QUERIES];

	uint8                            m_pendingCount;
	EDrxOnlineStorageStatus          m_status;
	std::unique_ptr<CAsyncInfoPimpl> m_pAsyncOperation;
	CDrxLobby*                       m_pLobby;
	CDrxLobbyService*                m_pService;
};

#endif//USE_DURANGOLIVE && USE_DRX_ONLINE_STORAGE

#endif // __DRXDURANGOLIVEONLINESTORAGE_H__
