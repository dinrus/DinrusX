// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_WEBAPI_STORE_H__
#define __DRXPSN2_WEBAPI_STORE_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

//////////////////////////////////////////////////////////////////////////////////////////////

		#define DRX_WEBAPI_COMMERCE_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_GET
		#define DRX_WEBAPI_COMMERCE_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_COMMERCE_API_GROUP    "commerce"
		#define DRX_WEBAPI_COMMERCE_REQUEST_PATH "/v1/users/me/container/%s?start=0&size=50&sort=price&useFree=true&useCurrencySymbol=true"
// This is an example for activity feed JSON document formatting.

// note: if DRX_WEBAPI_COMMERCE_MAX_PRODUCTS or DRX_WEBAPI_COMMERCE_PRODUCT_ID_LENGTH changes, the buffer used to concatenate ids into a string may not be long enough
// (see CDrxPSNOrbisWebApiThread::GetCommerceList)
		#define DRX_WEBAPI_COMMERCE_MAX_PRODUCTS (10)

struct SDrxPSNOrbisWebApiGetCommerceInput
{
	uint32        numProducts;
	TStoreOfferID products[DRX_WEBAPI_COMMERCE_MAX_PRODUCTS];
};

		#define DRX_WEBAPI_ENTITLEMENT_LIST_HTTP_METHOD     SCE_NP_WEBAPI_HTTP_METHOD_GET
		#define DRX_WEBAPI_ENTITLEMENT_LIST_CONTENT_TYPE    SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_ENTITLEMENT_LIST_API_GROUP       "entitlement"
		#define DRX_WEBAPI_ENTITLEMENT_LIST_REQUEST_PATH    "/v1/users/me/entitlements?entitlement_type=service&start=0&size=50"

		#define DRX_WEBAPI_ENTITLEMENT_CONSUME_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_PUT
		#define DRX_WEBAPI_ENTITLEMENT_CONSUME_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_ENTITLEMENT_CONSUME_API_GROUP    "entitlement"
		#define DRX_WEBAPI_ENTITLEMENT_CONSUME_REQUEST_PATH "/v1/users/me/entitlements/%s"

		#define DRX_WEBAPI_ENTITLEMENT_CONSUME_PUT_BODY \
		  "{"                                           \
		  "\"use_count\":%d"                            \
		  "}"

struct SDrxPSNOrbisWebApiConsumeEntitlementInput
{
	uint32        consumption;
	TStoreAssetID entitlementLabel;
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_ACTIVITYFEED_H__
