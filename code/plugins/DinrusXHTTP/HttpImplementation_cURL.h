// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#include "IHttpPlugin.h"

<DinrusX/DinrusXSys/ITimer.h>

#ifdef DRX_PLATFORM_WINDOWS
<DinrusX/winsock2.h>
<DinrusX/CoreX/Platform/DrxWindows.h>

#ifdef DELETE
#undef DELETE
#endif
#endif

<DinrusX/curl/curl.h>

namespace Drx
{
	namespace Http
	{
		namespace cURL
		{
			class CPlugin final : public Drx::Http::IPlugin
			{
				DRXINTERFACE_BEGIN()
					DRXINTERFACE_ADD(Drx::Http::IPlugin)
					DRXINTERFACE_ADD(Drx::IEnginePlugin)
				DRXINTERFACE_END()

				DRXGENERATE_SINGLETONCLASS_GUID(CPlugin, "Plugin_HTTP", "{D6F141B9-CDD8-44AB-ACF5-9C70EBC646CB}"_drx_guid)

				CPlugin() = default;
				virtual ~CPlugin();

			public:
				// IDrxPlugin
				virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
				virtual void MainUpdate(float frameTime) override;
				// ~IDrxPlugin

				// IPlugin_HTTP
				virtual EUpdateResult ProcessRequests() final;
				virtual bool Send(ERequestType requestType, const char* szURL, const char* szBody, TResponseCallback resultCallback, const THeaders& headers) final;
				// ~IPlugin_HTTP

			protected:
				static size_t CurlWriteResponse(char *ptr, size_t size, size_t nmemb, void *userData);
				static int CurlProgress(void *userData, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

			protected:
				struct SRequest
				{
					SRequest(CURL* pRequestHandle, TResponseCallback callback)
						: pHandle(pRequestHandle)
						, resultCallback(callback)
						, pHeaderList(nullptr)
						, lastTimeHandledData(gEnv->pTimer->GetAsyncCurTime())
					{
					}

					SRequest(SRequest& request) = delete;
					SRequest& operator=(SRequest&) = delete;

					SRequest(SRequest&& request)
						: pHandle(request.pHandle)
						, resultCallback(request.resultCallback)
						, result(std::move(request.result))
						, pHeaderList(request.pHeaderList)
						, lastTimeHandledData(request.lastTimeHandledData)
					{
						request.pHandle = nullptr;
						request.pHeaderList = nullptr;
					}
					SRequest& operator=(SRequest&&) = default;

					~SRequest()
					{
						if (pHeaderList != nullptr)
						{
							curl_slist_free_all(pHeaderList);
						}

						curl_easy_cleanup(pHandle);
					}

					CURL* pHandle;
					curl_slist* pHeaderList;

					TResponseCallback resultCallback;
					string result;
					// Time at which we last handled data, used for timeouts
					float lastTimeHandledData;
				};

				CURLM* m_pMultiHandle = nullptr;
				std::unordered_map<CURL*, SRequest> m_requestMap;

				float m_requestTimeOutInSeconds = 20.f;
			};
		}
	}
}