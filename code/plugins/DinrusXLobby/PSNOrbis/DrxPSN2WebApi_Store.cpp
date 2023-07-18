// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

//#pragma optimize("", off)

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		#include "DrxPSN2WebApi.h"
		#include "DrxPSN2WebApi_Store.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// GetCommerceList
// Worker function that runs on the WebApi thread to send a request to list PSN store products
// filtered by product ID

void CDrxPSNOrbisWebApiThread::GetCommerceList(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl == TMemInvalidHdl)
	{
		// no params! Error!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	SDrxPSNOrbisWebApiGetCommerceInput* pCommerceData = (SDrxPSNOrbisWebApiGetCommerceInput*)pController->GetUtility()->GetLobby()->MemGetPtr(paramsHdl);
	if (!pCommerceData)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	if (pCommerceData->numProducts > DRX_WEBAPI_COMMERCE_MAX_PRODUCTS)
	{
		pCommerceData->numProducts = DRX_WEBAPI_COMMERCE_MAX_PRODUCTS;
	}

	char pathStr[DRX_WEBAPI_TEMP_PATH_SIZE + DRX_WEBAPI_TEMP_BUFFER_SIZE];
	char workStr[DRX_WEBAPI_TEMP_BUFFER_SIZE];
	workStr[0] = 0;
	for (uint32 i = 0; i < pCommerceData->numProducts; ++i)
	{
		if (i > 0)
		{
			drx_strcat(workStr, ":");
		}
		drx_strcat(workStr, pCommerceData->products[i].productId.c_str());
	}
	drx_sprintf(pathStr, DRX_WEBAPI_COMMERCE_REQUEST_PATH, workStr);

	ret = _this->SendRequest(DRX_WEBAPI_COMMERCE_HTTP_METHOD, DRX_WEBAPI_COMMERCE_CONTENT_TYPE, DRX_WEBAPI_COMMERCE_API_GROUP, pathStr);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("commerce list");
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
// GetEntitlementList
// Worker function that runs on the WebApi thread to send a request to list PSN store
// "service" entitlements (consumables)

void CDrxPSNOrbisWebApiThread::GetEntitlementList(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl != TMemInvalidHdl)
	{
		// There should be no params for this! If there are params, it is wrong!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	ret = _this->SendRequest(DRX_WEBAPI_ENTITLEMENT_LIST_HTTP_METHOD, DRX_WEBAPI_ENTITLEMENT_LIST_CONTENT_TYPE, DRX_WEBAPI_ENTITLEMENT_LIST_API_GROUP, DRX_WEBAPI_ENTITLEMENT_LIST_REQUEST_PATH);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("entitlement list");
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
// ConsumeEntitlement
// Worker function that runs on the WebApi thread to inform PSN that an entitlement has
// been consumed.

void CDrxPSNOrbisWebApiThread::ConsumeEntitlement(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl == TMemInvalidHdl)
	{
		// no params! Error!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	SDrxPSNOrbisWebApiConsumeEntitlementInput* pConsumeData = (SDrxPSNOrbisWebApiConsumeEntitlementInput*)pController->GetUtility()->GetLobby()->MemGetPtr(paramsHdl);
	if (!pConsumeData)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	char pathStr[DRX_WEBAPI_TEMP_PATH_SIZE];
	drx_sprintf(pathStr, DRX_WEBAPI_ENTITLEMENT_CONSUME_REQUEST_PATH, pConsumeData->entitlementLabel.c_str());

	char tempStr[DRX_WEBAPI_TEMP_BUFFER_SIZE];
	drx_sprintf(tempStr, DRX_WEBAPI_ENTITLEMENT_CONSUME_PUT_BODY, pConsumeData->consumption);

	ret = _this->SendRequestText(DRX_WEBAPI_ENTITLEMENT_CONSUME_HTTP_METHOD, DRX_WEBAPI_ENTITLEMENT_CONSUME_CONTENT_TYPE, DRX_WEBAPI_ENTITLEMENT_CONSUME_API_GROUP, pathStr, tempStr);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("consume entitlement");
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
