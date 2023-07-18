// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

//#pragma optimize("", off)

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		#include "DrxPSN2WebApi.h"
		#include "DrxPSN2WebApi_ActivityFeed.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// PostActivity
// Worker function that runs on the WebApi thread to send an activity feed item to PSN.

void CDrxPSNOrbisWebApiThread::PostActivity(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl == TMemInvalidHdl)
	{
		// no params! Error!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	SDrxPSNOrbisWebApiAddActivityFeedInput* pFeedData = (SDrxPSNOrbisWebApiAddActivityFeedInput*)pController->GetUtility()->GetLobby()->MemGetPtr(paramsHdl);
	if (!pFeedData)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	pFeedData->feed[DRX_WEBAPI_ACTIVITYFEED_MAX_SIZE - 1] = 0;

	ret = _this->SendRequestText(DRX_WEBAPI_ACTIVITYFEED_HTTP_METHOD, DRX_WEBAPI_ACTIVITYFEED_CONTENT_TYPE, DRX_WEBAPI_ACTIVITYFEED_API_GROUP, DRX_WEBAPI_ACTIVITYFEED_REQUEST_PATH, pFeedData->feed);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("postactivity");
	if (responseBodyHdl == TMemInvalidHdl)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_OUT_OF_MEMORY, TMemInvalidHdl);
		return;
	}

	SDrxPSNWebApiResponseBody* pResponseBody = (SDrxPSNWebApiResponseBody*)pController->GetUtility()->GetLobby()->MemGetPtr(responseBodyHdl);
	if (!pResponseBody)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_OUT_OF_MEMORY, responseBodyHdl);
		return;
	}

	ret = _this->ReadResponse(pResponseBody);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, responseBodyHdl);
		return;
	}

	// all complete!
	pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, PSN_OK, responseBodyHdl);
}

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif //USE_PSN
#endif   // DRX_PLATFORM_ORBIS
