// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_WEBAPI_PRESENCE_H__
#define __DRXPSN2_WEBAPI_PRESENCE_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

//////////////////////////////////////////////////////////////////////////////////////////////

		#define DRX_WEBAPI_SETPRESENCE_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_PUT
		#define DRX_WEBAPI_SETPRESENCE_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_SETPRESENCE_API_GROUP    "userProfile"
		#define DRX_WEBAPI_SETPRESENCE_REQUEST_PATH "/v1/users/%s/presence/gameStatus"
		#define DRX_WEBAPI_SETPRESENCE_PUT_BODY \
		  "{"                                   \
		  "\"gameStatus\":\"%s\""               \
		  "}"

		#define DRX_WEBAPI_SETPRESENCE_MAX_SIZE (256)           // Sony docs say presence string should be NO MORE THAN 64 UTF-8 characters.

struct SDrxPSNOrbisWebApiSetPresenceInput
{
	SceNpOnlineId id;
	char          presence[DRX_WEBAPI_SETPRESENCE_MAX_SIZE];
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_PRESENCE_H__
