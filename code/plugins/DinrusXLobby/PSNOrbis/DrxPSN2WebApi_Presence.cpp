// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

//#pragma optimize("", off)

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		#include "DrxPSN2WebApi.h"
		#include "DrxPSN2WebApi_Presence.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// SetPresence
// Worker function that runs on the WebApi thread to set the rich presence for a user on PSN.
void CDrxPSNOrbisWebApiThread::SetPresence(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl == TMemInvalidHdl)
	{
		// no params! Error!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	SDrxPSNOrbisWebApiSetPresenceInput* pPresenceData = (SDrxPSNOrbisWebApiSetPresenceInput*)pController->GetUtility()->GetLobby()->MemGetPtr(paramsHdl);
	if (!pPresenceData)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	pPresenceData->presence[DRX_WEBAPI_SETPRESENCE_MAX_SIZE - 1] = 0;

	char pathStr[DRX_WEBAPI_TEMP_PATH_SIZE];
	drx_sprintf(pathStr, DRX_WEBAPI_SETPRESENCE_REQUEST_PATH, pPresenceData->id.data);

	drx_sprintf(_this->m_request.workBuffer, DRX_WEBAPI_SETPRESENCE_PUT_BODY, pPresenceData->presence);

	ret = _this->SendRequestText(DRX_WEBAPI_SETPRESENCE_HTTP_METHOD, DRX_WEBAPI_SETPRESENCE_CONTENT_TYPE, DRX_WEBAPI_SETPRESENCE_API_GROUP, pathStr, _this->m_request.workBuffer);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("setpresence");
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
