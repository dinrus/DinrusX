// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_WEBAPI_FRIENDSLIST_H__
#define __DRXPSN2_WEBAPI_FRIENDSLIST_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

//////////////////////////////////////////////////////////////////////////////////////////////

		#define DRX_WEBAPI_FRIENDSLIST_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_GET
		#define DRX_WEBAPI_FRIENDSLIST_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_FRIENDSLIST_API_GROUP    "userProfile"
		#define DRX_WEBAPI_FRIENDSLIST_REQUEST_PATH "/v1/users/%s/friendList?friendStatus=friend&presenceType=primary&limit=%d&offset=%d"
		#define DRX_WEBAPI_FRIENDSLIST_PAGE_LIMIT   2000

struct SDrxPSNOrbisWebApiGetFriendsListInput
{
	SceNpOnlineId id;
	uint32        startIndex;
	uint32        count;
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_FRIENDSLIST_H__
