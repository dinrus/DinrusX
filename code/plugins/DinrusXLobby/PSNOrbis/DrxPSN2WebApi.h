// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*
   Handles Orbis Web API. Creates a worker thread to process blocking GET requests, also registers PushEvent listener.
   All responses get pushed onto the m_lobbyThreadQueueBuilding queue to be processed on the network thread.
 */

#ifndef __DRXPSN2_WEBAPI_H__
#define __DRXPSN2_WEBAPI_H__
#pragma once

#if DRX_PLATFORM_ORBIS
	#if USE_PSN

		<DinrusX/json2.h>

class CDrxPSNSupport;
class CDrxPSNOrbisWebApiThread;
class CDrxPSNWebApiUtility;
class CDrxPSNWebApiJobController;

		#define INVALID_PSN_WEBAPI_PUSHEVENT_ID           (-1)
		#define INVALID_PSN_WEBAPI_SENDREQUEST_ID         (-1)
		#define INVALID_WEBAPI_JOB_ID                     (-1)

		#define DRX_WEBAPI_ERROR_FAIL                     (-1)
		#define DRX_WEBAPI_ERROR_END_OF_BODY              (-2)
		#define DRX_WEBAPI_ERROR_SEND_ALREADY_IN_PROGRESS (-3)
		#define DRX_WEBAPI_ERROR_NO_SEND_IN_PROGRESS      (-4)
		#define DRX_WEBAPI_ERROR_BAD_CONTENTLENGTH        (-5)

		#define DRX_WEBAPI_HEADER_CONTENT_TYPE            "Content-Type"
		#define DRX_WEBAPI_HEADER_CONTENT_LENGTH          "Content-Length"
		#define DRX_WEBAPI_CONTENT_TYPE_JSON              "application/json"

		#define DRX_WEBAPI_HEADER_VALUE_SIZE              (64)
		#define DRX_WEBAPI_TEMP_PATH_SIZE                 (256)
		#define DRX_WEBAPI_TEMP_BUFFER_SIZE               (256)

		#if !defined(_RELEASE)
			#define DRX_WEBAPI_RESPONSE_DEBUG_STR_LEN (64)
		#endif

struct SDrxPSNWebApiResponseBody
{
	enum EResponseBodyType
	{
		E_UNKNOWN = 0,
		E_JSON,
		E_RAW,
	};

	sce::Json::Value  jsonTreeRoot;
	TMemHdl           rawResponseBodyHdl;
	size_t            rawResponseBodySize;
	EResponseBodyType eType;

		#if !defined(_RELEASE)
	char debugStr[DRX_WEBAPI_RESPONSE_DEBUG_STR_LEN];
		#endif
};

// WebAPI has its own threads, with queues, because all calls are blocking.
typedef int32   DrxWebApiJobId;
typedef void (* TDrxPSNWebApiThreadJobFunc)(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);

struct SDrxPSNWebApiThreadJobInfo
{
	TDrxPSNWebApiThreadJobFunc m_pJobFunc;
	TMemHdl                    m_paramsHdl;
	DrxWebApiJobId             m_id;

	SDrxPSNWebApiThreadJobInfo(DrxWebApiJobId id, TDrxPSNWebApiThreadJobFunc pFunc, TMemHdl paramsHdl)
	{
		m_id = id;
		m_pJobFunc = pFunc;
		m_paramsHdl = paramsHdl;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

		#define WEBAPI_WORKBUFFER_MAX_SIZE (4 * 1024)

class CDrxPSNOrbisWebApiThread
	: public IThread
{
public:
	CDrxPSNOrbisWebApiThread(CDrxPSNWebApiJobController* pController)
		: m_pController(pController)
		, m_bAlive(true)
	{
		memset(&m_request, 0, sizeof(m_request));
		m_request.requestId = INVALID_PSN_WEBAPI_SENDREQUEST_ID;
		m_serverError.Reset();
	}

	// Start accepting work on thread
	virtual void   ThreadEntry();       // IThread
	void           SignalStopWork();

	DrxWebApiJobId Push(TDrxPSNWebApiThreadJobFunc pFunc, TMemHdl paramsHdl);
	void           Flush();

	// Add new WebApi request jobs below here
	// FriendsList
	static void GetFriendsList(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	// Presence
	static void SetPresence(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	// RecentPlayers
	static void AddRecentPlayers(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	// ActivityFeed
	static void PostActivity(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	// Session advertising
	static void CreateSession(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void DeleteSession(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void UpdateSession(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void JoinSession(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void LeaveSession(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void GetSessionLinkData(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	// PSN Store
	static void GetCommerceList(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void GetEntitlementList(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);
	static void ConsumeEntitlement(DrxWebApiJobId id, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this);

private:

	struct SRequestAndResponse
	{
		int64  requestId;
		char   workBuffer[WEBAPI_WORKBUFFER_MAX_SIZE];
		uint32 nRecvBufferUsed;
		uint32 nRecvBufferParsed;
	};

	struct SServerError
	{
		SceNpWebApiResponseInformationOption serverError;
		#ifdef _DEBUG
		static const int                     kErrorBufSize = 256;
		char serverErrorBuffer[kErrorBufSize]; // This buffer can be inspected in the debugger, it will contain JSON error data
		#endif

		void Reset()
		{
			memset(this, 0, sizeof(SServerError));
		#ifdef _DEBUG
			serverError.pErrorObject = serverErrorBuffer;
			serverError.errorObjectSize = sizeof(serverErrorBuffer);
		#endif
		}
	};

	static DrxWebApiJobId                  sm_nextFreeJobId;

	sce::Json::Parser                      Parser;
	SRequestAndResponse                    m_request;
	SServerError                           m_serverError;
	CDrxPSNWebApiJobController*            m_pController;
	DrxMutex                               m_mtx;
	DrxConditionVariable                   m_cond;
	std::deque<SDrxPSNWebApiThreadJobInfo> m_jobQueue;
	bool m_bAlive;

	int            SendRequest(SceNpWebApiHttpMethod httpMethod, const char* pContentType, const char* pAPIGroup, const char* pPath);
	int            SendRequestText(SceNpWebApiHttpMethod httpMethod, const char* pContentType, const char* pAPIGroup, const char* pPath, const char* pBody);
	int            SendRequestData(SceNpWebApiHttpMethod httpMethod, const char* pContentType, const char* pAPIGroup, const char* pPath, const void* pData, size_t sizeofData);
	int            StoreRawResponseBody(SDrxPSNWebApiResponseBody* pResponseBody);
	int            ReadResponse(SDrxPSNWebApiResponseBody* pResponseBody);

	static int     DataProvideCallback(char& data, void* pWebAPIJobThread);

	DrxWebApiJobId GetUniqueJobId();
};

//////////////////////////////////////////////////////////////////////////////////////////////

		#define WEBAPI_JSON_DEBUG_INDENTS (64)

class CDrxPSNWebApiUtility : public sce::Json::MemAllocator
{
protected:

	struct SPrintJSONIndent
	{
		uint32 m_debugIndent;
		char   m_debugIndentString[WEBAPI_JSON_DEBUG_INDENTS + 1];

		SPrintJSONIndent()
			: m_debugIndent(0)
		{
			memset(m_debugIndentString, ' ', WEBAPI_JSON_DEBUG_INDENTS);
			m_debugIndentString[m_debugIndent] = '\0';
			m_debugIndentString[WEBAPI_JSON_DEBUG_INDENTS] = '\0';
		}
		void operator++(void)
		{
			if (m_debugIndent <= WEBAPI_JSON_DEBUG_INDENTS - 2)
			{
				m_debugIndentString[m_debugIndent] = ' ';
				m_debugIndent += 2;
				m_debugIndentString[m_debugIndent] = '\0';
			}
		}
		void operator--(void)
		{
			if (m_debugIndent >= 2)
			{
				m_debugIndentString[m_debugIndent] = ' ';
				m_debugIndent -= 2;
				m_debugIndentString[m_debugIndent] = '\0';
			}
		}
		const char* c_str() const
		{
			return m_debugIndentString;
		}
	};

	sce::Json::Initializer Initializer;
	CDrxPSNSupport*        m_pSupport;
	CDrxLobby*             m_pLobby;

	int                    m_webApiCtxId;
	int                    m_pushEventFilterId;
	int                    m_servicePushEventFilterId;

	bool                   m_bIsInitialised;

	int                    m_debugJsonAllocs;

	// utility for printing a json tree
	void DebugPrintJSONObject(const sce::Json::Object& obj, SPrintJSONIndent& indent);
	void DebugPrintJSONArray(const sce::Json::Array& ary, SPrintJSONIndent& indent);
	void DebugPrintJSONValue(const sce::Json::Value& val, SPrintJSONIndent& indent);

public:

	CDrxPSNWebApiUtility()
		: m_pSupport(nullptr)
		, m_pLobby(nullptr)
		, m_webApiCtxId(0)
		, m_pushEventFilterId(0)
		, m_servicePushEventFilterId(0)
		, m_bIsInitialised(false)
		, m_debugJsonAllocs(0)
	{
	}
	~CDrxPSNWebApiUtility() { Terminate(); }

	int             Initialise(int webApiCtxId, CDrxPSNSupport* pSupport);
	void            Terminate();
	bool            IsInitialised()                           { return m_bIsInitialised; }

	int             GetWebApiContextId() const                { return m_webApiCtxId; }
	int             GetWebApiPushEventFilterId() const        { return m_pushEventFilterId; }
	int             GetWebApiServicePushEventFilterId() const { return m_servicePushEventFilterId; }

	CDrxPSNSupport* GetPSNSupport() const                     { return m_pSupport; }
	CDrxLobby*      GetLobby() const                          { return m_pLobby; }

	// utility for printing a json tree
	int     PrintResponseJSONTree(const SDrxPSNWebApiResponseBody* pResponseBody);

	TMemHdl NewResponseBody(char* debugString);
	int     FreeResponseBody(TMemHdl responseBodyHdl);

	TMemHdl AllocMem(size_t numBytes);
	void    FreeMem(TMemHdl hdl);

	// From sce::Json::MemAllocator - Only used by Json library
	virtual void* allocate(size_t numBytes, void* pUserData);
	virtual void  deallocate(void* ptr, void* pUserData);

	// utility for encoding and decoding base64
	char*  Base64Encode(const uint8* buffer, int len, TMemHdl& hdl);
	uint8* Base64Decode(const char* buffer, int len, TMemHdl& hdl);
};

//////////////////////////////////////////////////////////////////////////////////////////////

class CDrxPSNWebApiJobController
{
protected:

	CDrxPSNWebApiUtility*     m_pWebApiUtility;
	CDrxPSNOrbisWebApiThread* m_pWebApiThread;        // 1 for now

	int                       m_webApiUserCtxId;
	int                       m_pushEventCallbackId;
	int                       m_servicePushEventCallbackId;

	bool                      m_bIsInitialised;

	static void               PushEventCallback(int, int, const SceNpPeerAddressA*, const SceNpPeerAddressA*, const SceNpWebApiPushEventDataType*, const char*, size_t, void*);
	static void               ServicePushEventCallback(int, int, const char*, SceNpServiceLabel, const SceNpPeerAddressA*, const SceNpPeerAddressA*, const SceNpWebApiPushEventDataType*, const char*, size_t, void*);

public:

	CDrxPSNWebApiJobController()
		: m_pWebApiUtility(nullptr)
		, m_pWebApiThread(nullptr)
		, m_webApiUserCtxId(0)
		, m_pushEventCallbackId(0)
		, m_servicePushEventCallbackId(0)
		, m_bIsInitialised(false)
	{
	}
	~CDrxPSNWebApiJobController() { Terminate(); }

	int                   Initialise(CDrxPSNWebApiUtility* pUtility, SceNpId npId);
	int                   Terminate();
	bool                  IsInitialised()      { return m_bIsInitialised; }

	int                   GetContextId() const { return m_webApiUserCtxId; }
	CDrxPSNWebApiUtility* GetUtility() const   { return m_pWebApiUtility; }

	DrxWebApiJobId        AddJob(TDrxPSNWebApiThreadJobFunc job, TMemHdl paramsHdl);
};

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS

#endif // __DRXPSN2_WEBAPI_H__
