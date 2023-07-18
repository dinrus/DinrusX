// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXPSN2_WEBAPI_SESSION_H__
#define __DRXPSN2_WEBAPI_SESSION_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

//////////////////////////////////////////////////////////////////////////////////////////////

		#define DRX_WEBAPI_CREATESESSION_HTTP_METHOD    SCE_NP_WEBAPI_HTTP_METHOD_POST
		#define DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY "mxbMLPonYFiMuuuuu999"
		#define DRX_WEBAPI_CREATESESSION_CONTENT_TYPE   "multipart/mixed; boundary=" DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY
		#define DRX_WEBAPI_CREATESESSION_API_GROUP      "sessionInvitation"
		#define DRX_WEBAPI_CREATESESSION_REQUEST_PATH   "/v1/sessions"

// sample json advertisement
		#define DRX_WEBAPI_CREATESESSION_JSON_SAMPLE \
		  "{"                                        \
		  "\"availablePlatforms\":[\"PS4\"]"         \
		  "\"sessionType\":\"owner-migration\","     \
		  "\"sessionPrivacy\":\"public\","           \
		  "\"sessionMaxUser\":16,"                   \
		  "\"localizedSessionNames\":["              \
		  "{\"en\":\"Warface - Nev's Room\"}"        \
		  "],"                                       \
		  "\"localizedSessionStatus\":["             \
		  "{\"en\":\"PVP Deathmatch in progress\"}"  \
		  "]"                                        \
		  "}"

// sample jpeg - this is a base64 encoded 16x16 white jpeg (I think)
// Should be pointed out I tried to send garbage image data and it failed.
// Supposed to send a raw jpeg. But at least the base64 jpeg doesn't fail, even if it doesn't display anything on the dashboard.
		#define DRX_WEBAPI_CREATESESSION_IMAGE_BODY_SAMPLE                                                                                                         \
		  "/9j/4AAQSkZJRgABAQEAYABgAAD/4QBoRXhpZgAATU0AKgAAAAgABAEaAAUAAAABAAAAPgEbAAUAAAABAAAARgEoAAMAAAABAAIAAAExAAIAAAASAAAATgAAAAAAAABgAAAAAQAAAGAAAAABUGFpbn" \
		  "QuTkVUIHYzLjUuMTAA/9sAQwD//////////////////////////////////////////////////////////////////////////////////////9sAQwH/////////////////////////////////" \
		  "/////////////////////////////////////////////////////8AAEQgAEAAQAwEiAAIRAQMRAf/EAB8AAAEFAQEBAQEBAAAAAAAAAAABAgMEBQYHCAkKC//EALUQAAIBAwMCBAMFBQQEAAABfQ" \
		  "ECAwAEEQUSITFBBhNRYQcicRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeo" \
		  "qaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5+v/EAB8BAAMBAQEBAQEBAQEAAAAAAAABAgMEBQYHCAkKC//EALURAAIBAgQEAwQHBQQEAAECdwABAgMRBAUhMQ" \
		  "YSQVEHYXETIjKBCBRCkaGxwQkjM1LwFWJy0QoWJDThJfEXGBkaJicoKSo1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoKDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2" \
		  "t7i5usLDxMXGx8jJytLT1NXW19jZ2uLj5OXm5+jp6vLz9PX29/j5+v/aAAwDAQACEQMRAD8AkooooA//2Q=="

		#define DRX_WEBAPI_CREATESESSION_POST_BODY_JSON                                                   \
		  "--" DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY "\r\n"                                             \
		                                               "Content-Type:application/json; charset=utf-8\r\n" \
		                                               "Content-Описание:session-request\r\n"          \
		                                               "Content-Length:%d\r\n"                            \
		                                               "\r\n"                                             \
		                                               "%s\r\n"

// then
		#define DRX_WEBAPI_CREATESESSION_POST_BODY_JPG                                         \
		  "--" DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY "\r\n"                                  \
		                                               "Content-Type:image/jpeg\r\n"           \
		                                               "Content-Описание:session-image\r\n" \
		                                               "Content-Disposition:attachment\r\n"    \
		                                               "Content-Length:%d\r\n"                 \
		                                               "\r\n"
// then jpg data

// then
		#define DRX_WEBAPI_CREATESESSION_POST_BODY_DATA                                            \
		  "\r\n"                                                                                   \
		  "--" DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY "\r\n"                                      \
		                                               "Content-Type:application/octet-stream\r\n" \
		                                               "Content-Описание:session-data\r\n"      \
		                                               "Content-Disposition:attachment\r\n"        \
		                                               "Content-Length:%d\r\n"                     \
		                                               "\r\n"
// then data

// then ending
		#define DRX_WEBAPI_CREATESESSION_POST_BODY_END \
		  "\r\n"                                       \
		  "--" DRX_WEBAPI_CREATESESSION_MIXED_BOUNDARY "--\r\n"

		#define DRX_WEBAPI_CREATESESSION_FORMATTING_MAX_SIZE (512)        // can be changed if needed
		#define DRX_WEBAPI_CREATESESSION_JSON_MAX_SIZE       (512)        // can be changed if needed
		#define DRX_WEBAPI_CREATESESSION_JPG_MAX_SIZE        (160 * 1024) // the maximum JPG size is 160KB
		#define DRX_WEBAPI_CREATESESSION_LINKDATA_MAX_SIZE   (64)         // can be changed if needed. Upper limit is 1MB
		#define DRX_WEBAPI_CREATESESSION_MAX_SIZE            (DRX_WEBAPI_CREATESESSION_FORMATTING_MAX_SIZE + DRX_WEBAPI_CREATESESSION_JSON_MAX_SIZE + DRX_WEBAPI_CREATESESSION_JPG_MAX_SIZE + DRX_WEBAPI_CREATESESSION_LINKDATA_MAX_SIZE)

struct SDrxPSNOrbisWebApiCreateSessionInput
{
	SDrxLobbySessionAdvertisement advertisement;
	uint8                         data[DRX_WEBAPI_CREATESESSION_LINKDATA_MAX_SIZE];
	char                          body[DRX_WEBAPI_CREATESESSION_MAX_SIZE];
};

		#define DRX_WEBAPI_DELETESESSION_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_DELETE
		#define DRX_WEBAPI_DELETESESSION_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_DELETESESSION_API_GROUP    "sessionInvitation"
		#define DRX_WEBAPI_DELETESESSION_REQUEST_PATH "/v1/sessions/%s"

struct SDrxPSNOrbisWebApiDeleteSessionInput
{
	SceNpSessionId sessionId;
};

		#define DRX_WEBAPI_UPDATESESSION_API_GROUP             "sessionInvitation"
		#define DRX_WEBAPI_UPDATESESSION_JSON_HTTP_METHOD      SCE_NP_WEBAPI_HTTP_METHOD_PUT
		#define DRX_WEBAPI_UPDATESESSION_JSON_CONTENT_TYPE     SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_UPDATESESSION_JSON_REQUEST_PATH     "/v1/sessions/%s"
		#define DRX_WEBAPI_UPDATESESSION_JPG_HTTP_METHOD       SCE_NP_WEBAPI_HTTP_METHOD_PUT
		#define DRX_WEBAPI_UPDATESESSION_JPG_CONTENT_TYPE      "image/jpeg"
		#define DRX_WEBAPI_UPDATESESSION_JPG_REQUEST_PATH      "/v1/sessions/%s/sessionImage"
		#define DRX_WEBAPI_UPDATESESSION_LINKDATA_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_PUT
		#define DRX_WEBAPI_UPDATESESSION_LINKDATA_CONTENT_TYPE "application/octet-stream"
		#define DRX_WEBAPI_UPDATESESSION_LINKDATA_REQUEST_PATH "/v1/sessions/%s/sessionData"

		#define DRX_WEBAPI_UPDATESESSION_JSON_MAX_SIZE         (512)        // can be changed if needed
		#define DRX_WEBAPI_UPDATESESSION_JPG_MAX_SIZE          (160 * 1024) // the maximum JPG size is 160KB
		#define DRX_WEBAPI_UPDATESESSION_LINKDATA_MAX_SIZE     (64)         // can be changed if needed. this buffer will be base64 encoded. Upper limit is 1MB

struct SDrxPSNOrbisWebApiUpdateSessionInput
{
	SDrxLobbySessionAdvertisement advertisement;
	SceNpSessionId                sessionId;
	uint8                         data[DRX_WEBAPI_UPDATESESSION_LINKDATA_MAX_SIZE];
	char                          jsonBody[DRX_WEBAPI_UPDATESESSION_JSON_MAX_SIZE];
};

		#define DRX_WEBAPI_JOINSESSION_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_POST
		#define DRX_WEBAPI_JOINSESSION_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_JOINSESSION_API_GROUP    "sessionInvitation"
		#define DRX_WEBAPI_JOINSESSION_REQUEST_PATH "/v1/sessions/%s/members"

struct SDrxPSNOrbisWebApiJoinSessionInput
{
	SceNpSessionId sessionId;
};

		#define DRX_WEBAPI_LEAVESESSION_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_DELETE
		#define DRX_WEBAPI_LEAVESESSION_CONTENT_TYPE SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8
		#define DRX_WEBAPI_LEAVESESSION_API_GROUP    "sessionInvitation"
		#define DRX_WEBAPI_LEAVESESSION_REQUEST_PATH "/v1/sessions/%s/members/%s"

struct SDrxPSNOrbisWebApiLeaveSessionInput
{
	SceNpSessionId sessionId;
	SceNpOnlineId  playerId;
};

		#define DRX_WEBAPI_GETSESSIONDATA_HTTP_METHOD  SCE_NP_WEBAPI_HTTP_METHOD_GET
		#define DRX_WEBAPI_GETSESSIONDATA_CONTENT_TYPE "application/octet-stream"
		#define DRX_WEBAPI_GETSESSIONDATA_API_GROUP    "sessionInvitation"
		#define DRX_WEBAPI_GETSESSIONDATA_REQUEST_PATH "/v1/sessions/%s/sessionData"

struct SDrxPSNOrbisWebApiGetSessionLinkDataInput
{
	SceNpSessionId sessionId;
};

//////////////////////////////////////////////////////////////////////////////////////////////

enum EDrxPSNOrbisWebApiSessionErrors
{
	DRX_WEBAPI_NO_SUCH_SESSION = 2113549,
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_ACTIVITYFEED_H__
