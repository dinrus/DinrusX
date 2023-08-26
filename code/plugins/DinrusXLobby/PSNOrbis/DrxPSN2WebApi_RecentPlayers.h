// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_WEBAPI_RECENTPLAYERS_H__
#define __DRXPSN2_WEBAPI_RECENTPLAYERS_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

//////////////////////////////////////////////////////////////////////////////////////////////

		#define DRX_WEBAPI_ADDRECENTPLAYERS_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_POST
		#define DRX_WEBAPI_ADDRECENTPLAYERS_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_ADDRECENTPLAYERS_API_GROUP    "activityFeed"
		#define DRX_WEBAPI_ADDRECENTPLAYERS_REQUEST_PATH "/v1/users/me/feed"
		#define DRX_WEBAPI_ADDRECENTPLAYERS_POST_BODY_SAMPLE \
		  "{"                                                \
		  "\"source\":{"                                     \
		  "\"meta\":\"DrxPlayer1\","                         \
		  "\"type\":\"ONLINE_ID\""                           \
		  "}, "                                              \
		  "\"storyType\":\"PLAYED_WITH\","                   \
		  "\"targets\":["                                    \
		  "{"                                                \
		  "\"meta\":\"DrxPlayer2\","                         \
		  "\"type\":\"ONLINE_ID\""                           \
		  "},"                                               \
		  "{"                                                \
		  "\"meta\":\"DrxPlayer3\","                         \
		  "\"type\":\"ONLINE_ID\""                           \
		  "},"                                               \
		  "{"                                                \
		  "\"meta\":\"NPXX12345_00\","                       \
		  "\"type\":\"TITLE_ID\""                            \
		  "},"                                               \
		  "{"                                                \
		  "\"meta\":\"Deathmatch PVP\","                     \
		  "\"type\":\"PLAYED_DESCRIPTION\""                  \
		  "}"                                                \
		  "]"                                                \
		  "}"

		#define DRX_WEBAPI_ADDRECENTPLAYERS_MAX_SIZE (4096)         // can change this if needed

struct SDrxPSNOrbisWebApiAddRecentPlayersInput
{
	char feed[DRX_WEBAPI_ADDRECENTPLAYERS_MAX_SIZE];
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_RECENTPLAYERS_H__
