// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: Logs data about client latency

	-------------------------------------------------------------------------
	History:
	- 30:11:2011  : Created by Martin Sherburn

*************************************************************************/

#ifndef __LAGOMETER_H__
#define __LAGOMETER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if USE_LAGOMETER



#include <DinrusX/DinrusXGame/Actor.h>
#include <DinrusX/DinrusXGame/ITelemetryCollector.h>
#include <DinrusX/DinrusXInput/IInput.h>
#include <DinrusX/DinrusXGame/ItemString.h>

struct HitInfo;
class CItem;
struct IDebugHistoryUpr;
struct IDebugHistory;


class CLagOMeter : public IInputEventListener
{
public:
	enum EBandwidthGraph
	{
		eBG_None = 0,
		eBG_Upload = BIT(0),
		eBG_Download = BIT(1),
		eBG_Both = eBG_Upload|eBG_Download,
	};


	CLagOMeter();
	~CLagOMeter();

	void OnClientRequestHit(HitInfo &hitInfo);
	void OnClientReceivedKill(const CActor::KillParams &killParams);
	void OnClientRequestPurchase(int purchaseSlot);
	void OnClientPickupItem(CItem* pItem);
	void Update();
	void UpdateBandwidthGraphVisibility();
	
	// IInputEventListener
	bool OnInputEvent( const SInputEvent &event );

protected:
	struct SHitRequestHistory
	{
		SHitRequestHistory() {}
		CTimeValue requestTime;
	};

	enum { HIT_HISTORY_COUNT = 16 };		// Note this should match up with serialization policy in HitInfo & KillParams
	typedef std::map<ItemString, CTimeValue> TStringTimeMap;

	IDebugHistoryUpr* m_pDebugHistoryUpr;
	IDebugHistory* m_pUploadGraph;
	IDebugHistory* m_pDownloadGraph;
	SHitRequestHistory m_hitHistory[HIT_HISTORY_COUNT];
	int m_rollingHitId;
	TStringTimeMap m_purchaseRequests;
	CTimeValue m_prevTime;

	bool m_buttonHeld;
	bool m_perceivedLagEventRecorded;
	float m_timeSinceLastPerceivedLagEvent;
};

#endif // USE_LAGOMETER

#endif // __LAGOMETER_H__

