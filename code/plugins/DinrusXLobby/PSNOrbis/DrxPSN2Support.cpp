// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		#include "DrxPSN2WebApi.h"
		<DinrusX/CoreX/Platform/IPlatformOS.h>                                  //Used to pass messages to the PlatformOS
		<DinrusX/np_commerce_dialog.h>
		<DinrusX/invitation_dialog.h>

		#if 1
			#if !defined(_RELEASE)
				#define MAIN_THREAD_MAX_SOCKET_STALLS (15 * 15) // Guard against some more sync stalls i've not had chance to fix
			#else
// We may decrease this for release mode, but at present I've put it same as profile - as lower values are causing problems.
				#define MAIN_THREAD_MAX_SOCKET_STALLS (15 * 15) //-- Socket poll timeout is set to 66ms, so could time out 15 times per second. Test for 15 seconds.
			#endif
			#include "Socket/SocketIOUprSelect.h"
		#endif

// Use sizes from WebAPI sample code
		#define PSN_NET_POOL_SIZE             (16 * 1024)
		#define PSN_HTTP_POOL_SIZE            (48 * 1024)
		#define PSN_SSL_POOL_SIZE             (256 * 1024)
		#define PSN_WEBAPI_POOL_SIZE          (32 * 1024)

		#define CONTEXT_TIMEOUT               (10 * 1000 * 1000) // 10 seconds as recommended by sony
		#define REQUEST_TIMEOUT               (20 * 1000 * 1000) // 20 seconds for default request timeout

		#define PSN_RANDOM_SERVER_FETCH_LIMIT (5) // attempts to fetch server info before giving up.

//-- If the PSN servers ever give you any grief (ie, error 0x80552A00),
//-- you could probably turn these off temporarily
		#define DO_ORBIS_ONLINE_PRIVILEGE_TEST
		#define DO_ORBIS_PLUS_PRIVILEGE_TEST

EDrxLobbyError MapSupportErrorToLobbyError(EDrxPSNPendingError pendingError)
{
	switch (pendingError)
	{
	case ePSNPE_None:
		return eCLE_Success;                // Under normal circumstances this should not happen
	case ePSNPE_NetLibraryNotInitialised:
	case ePSNPE_MatchingLibraryNotInitialised:
	case ePSNPE_SignalingLibraryNotInitialised:
	case ePSNPE_ScoreLibraryNotInitialised:
	case ePSNPE_TUSLibraryNotInitialised:
		return eCLE_NotInitialised;
	case ePSNPE_InternetDisabled:
		return eCLE_InternetDisabled;
	case ePSNPE_PSNUnavailable:
		return eCLE_PSNUnavailable;
	case ePSNPE_NoPSNAccount:
		return eCLE_NoOnlineAccount;
	case ePSNPE_SignInCancelled:
		return eCLE_UserNotSignedIn;
	case ePSNPE_NotConnected:
	case ePSNPE_Offline:
		return eCLE_NotConnected;
	case ePSNPE_CableDisconnected:
		return eCLE_CableNotConnected;
	case ePSNPE_OutOfMemory:
		return eCLE_OutOfMemory;
	case ePSNPE_ContextError:
		return eCLE_PSNContextError;
	case ePSNPE_NoServersAvailable:
		return eCLE_ServerNotDefined;
	case ePSNPE_NoWorldsAvailable:
		return eCLE_WorldNotDefined;
	case ePSNPE_InternalError:
		return eCLE_InternalError;
	case ePSNPE_TimedOut:
		return eCLE_TimeOut;
	case ePSNPE_IncorrectState:
		return eCLE_PSNWrongSupportState;
	case ePSNPE_BadArgument:
		return eCLE_InvalidParam;
	case ePSNPE_RestrictedPrivileges:
		return eCLE_InsufficientPrivileges;
	case ePSNPE_AgeRestricted:
		return eCLE_AgeRestricted;
	case ePSNPE_RestartForInvite:
		return eCLE_CyclingForInvite;
	case ePSNPE_RoomIsFull:
		return eCLE_SessionFull;
	case ePSNPE_PSNAccountSuspended:
		return eCLE_OnlineAccountBlocked;
	}

	NetLog("[Lobby] Warning Unknown PSN Error - probably a new error code not in this mapping table (CDrxPSNSupport::MapSupportErrorToLobbyError");
	return eCLE_InternalError;
}

CDrxPSNSupport::CDrxPSNSupport(CDrxLobby* pLobby, CDrxLobbyService* pService, EDrxLobbyServiceFeatures features)
	: m_pLobby(pLobby)
	, m_netCtlHandlerID(INVALID_NET_CTL_HANDLER_ID)
	, m_netMemPoolId(INVALID_NET_MEMPOOL_ID)
	, m_sslCtxId(INVALID_PSN_SSL_CONTEXT)
	, m_httpCtxId(INVALID_PSN_HTTP_CONTEXT)
	, m_webApiCtxId(INVALID_PSN_WEBAPI_CONTEXT)
		#if USE_DRX_STATS
	, m_scoringCtxId(INVALID_PSN_STATS_CONTEXT)
	, m_tusCtxId(INVALID_PSN_STATS_CONTEXT)
		#endif
	, m_bIsTransitioningActive(false)
	, m_onlinePrivTestReqId(0)
	, m_onlinePrivRestrictionsReqId(0)
	, m_onlinePrivAge(0)
	, m_plusPrivReqId(0)
	, m_isChatRestricted(0)
	, m_curState(ePSNOS_Uninitialised)
	, m_wantedState(ePSNOS_Uninitialised)
	, m_features(features)
	, m_nCallbacksRegistered(0)
{
	ZeroMemory(&m_inviteInfo, sizeof(m_CallbackHandlers));
	ZeroMemory(&m_CallbackHandlers, sizeof(m_CallbackHandlers));
}

CDrxPSNSupport::~CDrxPSNSupport()
{
	Terminate();
}

EDrxLobbyError CDrxPSNSupport::Initialise()
{
	EDrxLobbyError error = eCLE_Success;

	m_serverBestInfo.server.serverId = INVALID_PSN_SERVER_ID;
	m_myUserId = SCE_USER_SERVICE_USER_ID_INVALID;
	m_plusPrivReqId = INVALID_PRIVILEGE_REQUEST_ID;
	m_onlinePrivTestReqId = INVALID_PRIVILEGE_REQUEST_ID;
	m_onlinePrivRestrictionsReqId = INVALID_PRIVILEGE_REQUEST_ID;
	memset(&m_onlinePrivRestrictionsResult, 0, sizeof(m_onlinePrivRestrictionsResult));
	memset(&m_myNpId, 0, sizeof(SceNpId));
	m_isChatRestricted = 0;

	m_netMemPoolId = INVALID_NET_MEMPOOL_ID;
	m_sslCtxId = INVALID_PSN_SSL_CONTEXT;
	m_httpCtxId = INVALID_PSN_HTTP_CONTEXT;
	m_webApiCtxId = INVALID_PSN_WEBAPI_CONTEXT;
	m_matchmakingCtxId = INVALID_PSN_MATCHMAKING_CONTEXT;
		#if USE_DRX_STATS
	m_scoringCtxId = INVALID_PSN_STATS_CONTEXT;
	m_tusCtxId = INVALID_PSN_STATS_CONTEXT;
		#endif

	m_nCallbacksRegistered = 0;

	m_inviteInfo.m_bUseInvite = false;

	memset(&m_netInfo, 0, sizeof(m_netInfo));

	IPlatformOS* os = GetISystem()->GetPlatformOS();
	assert(os);
	os->AddListener(this, "DrxPSNSupport");

	int ret = RegisterNetCtlHandler(PSNNetCtlCallback, this, &m_netCtlHandlerID);
	if (ret != PSN_OK)
	{
		NetLog("RegisterNetCtlHandler : Error %08X", ret);
		error = eCLE_InternalError;
		return error;
	}

	ret = sceNetPoolCreate("PSNSupport NetPool", PSN_NET_POOL_SIZE, 0);
	if (ret >= PSN_OK)
	{
		m_netMemPoolId = ret;
		ret = sceSslInit(PSN_SSL_POOL_SIZE);
		if (ret >= PSN_OK)
		{
			m_sslCtxId = ret;
			ret = sceHttpInit(m_netMemPoolId, m_sslCtxId, PSN_HTTP_POOL_SIZE);
			if (ret >= PSN_OK)
			{
				m_httpCtxId = ret;
				ret = sceNpWebApiInitialize(m_httpCtxId, PSN_WEBAPI_POOL_SIZE);
				if (ret >= PSN_OK)
				{
					m_webApiCtxId = ret;
					m_curState = ePSNOS_Offline;
				}
				else
				{
					NetLog("sceNpWebApiInitialize : Error %08X", ret);
					error = eCLE_OutOfMemory;
				}
			}
			else
			{
				NetLog("sceHttpInit : Error %08X", ret);
				error = eCLE_OutOfMemory;
			}
		}
		else
		{
			NetLog("sceSslInit : Error %08X", ret);
			error = eCLE_OutOfMemory;
		}
	}
	else
	{
		NetLog("sceNetPoolCreate : Error %08X", ret);
		error = eCLE_OutOfMemory;
	}

	return error;
}

EDrxLobbyError CDrxPSNSupport::Terminate()
{
	int ret;

	CleanTo(ePSNOS_Uninitialised, ePSNPE_Offline);

	if (m_webApiCtxId != INVALID_PSN_WEBAPI_CONTEXT)
	{
		ret = sceNpWebApiTerminate(m_webApiCtxId);
		m_webApiCtxId = INVALID_PSN_WEBAPI_CONTEXT;
	}
	if (m_httpCtxId != INVALID_PSN_HTTP_CONTEXT)
	{
		ret = sceHttpTerm(m_httpCtxId);
		DRX_ASSERT_MESSAGE(ret == PSN_OK, "Failed to unregister Http context");
		m_httpCtxId = INVALID_PSN_HTTP_CONTEXT;
	}
	if (m_sslCtxId != INVALID_PSN_SSL_CONTEXT)
	{
		ret = sceSslTerm(m_sslCtxId);
		DRX_ASSERT_MESSAGE(ret == PSN_OK, "Failed to unregister SSL context");
		m_sslCtxId = INVALID_PSN_SSL_CONTEXT;
	}
	if (m_netMemPoolId != INVALID_NET_MEMPOOL_ID)
	{
		ret = sceNetPoolDestroy(m_netMemPoolId);
		DRX_ASSERT_MESSAGE(ret == PSN_OK, "Failed to destroy NetPool");
		m_netMemPoolId = INVALID_NET_MEMPOOL_ID;
	}

	if (m_netCtlHandlerID != INVALID_NET_CTL_HANDLER_ID)
	{
		ret = UnregisterNetCtlHandler(m_netCtlHandlerID);
		DRX_ASSERT_MESSAGE(ret == PSN_OK, "Failed to unregister NetCtl handler. cellNetCtlHandler/sceNetCtlUnregisterCallback");
		m_netCtlHandlerID = INVALID_NET_CTL_HANDLER_ID;
	}

	IPlatformOS* os = GetISystem()->GetPlatformOS();
	assert(os);
	os->RemoveListener(this);

	return eCLE_Success;
}

void CDrxPSNSupport::DispatchLinkState(ECableState state)
{
	if (m_pLobby)
	{
		UDrxLobbyEventData eventData;
		SDrxLobbyEthernetStateData ether;

		eventData.pEthernetStateData = &ether;
		ether.m_curState = state;

		m_pLobby->DispatchEvent(eCLSE_EthernetState, eventData);
	}
}

void CDrxPSNSupport::NetCtlCallbackHandler(int event)
{
	UDrxLobbyEventData eventData;
	SDrxLobbyEthernetStateData ether;
	int ret;
	ECableState state;

	// Change in state, check the link status now.
	SceNetCtlInfo info;

	ret = sceNetCtlGetInfo(SCE_NET_CTL_INFO_LINK, &info);
	if (ret == SCE_OK)
	{
		if (info.link == SCE_NET_CTL_LINK_DISCONNECTED)
		{
			// We are disconnected, check that the current network device is wired then dispatch the cable disconnected event
			ret = sceNetCtlGetInfo(SCE_NET_CTL_INFO_DEVICE, &info);
			if (ret == SCE_OK)
			{
				if (info.device == SCE_NET_CTL_DEVICE_WIRED)
				{
					// Wired adapter, disconnected state should indicate Ethernet unplugged.
					state = eCS_Unplugged;
				}
				else
				{
					// Wireless but link is down (router powered off?)
					state = eCS_Disconnected;
				}
			}
		}
		else
		{
			state = eCS_Connected;
		}
		if ((ret == SCE_OK) && m_pLobby)
		{
			TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchLinkState, this, state);
		}
	}
}

void CDrxPSNSupport::PSNNetCtlCallback(int event, void* pArg)
{
	CDrxPSNSupport* _this = static_cast<CDrxPSNSupport*>(pArg);

	_this->m_lobbyThreadQueueBuildingMutex.Lock();
	_this->m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::NetCtlCallbackHandler, _this, int(event));
	_this->m_lobbyThreadQueueBuildingMutex.Unlock();
}

//CALLED FROM SAME THREAD AS NETWORK - NO SPECIAL HANDLING REQUIRED
void CDrxPSNSupport::SystemCallbackHandler(SceSystemServiceEvent event)
{
	switch (event.eventType)
	{
	case SCE_SYSTEM_SERVICE_EVENT_SESSION_INVITATION:
		{
			// accepted an invite, or joined someone else via session advertisement
			SceInvitationEventParam& param = (SceInvitationEventParam&)event.data;

			SDrxPSNSessionID* pSessionInfo = new SDrxPSNSessionID;
			if (pSessionInfo)
			{
				memcpy(&pSessionInfo->m_sessionId, &param.sessionId, sizeof(SceNpSessionId));
				pSessionInfo->m_fromInvite = (param.flag & SCE_NP_SESSION_INVITATION_EVENT_FLAG_INVITATION) ? true : false;

				TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchInviteAccepted, this, DrxSessionID(pSessionInfo), eCLE_Success);
			}
		}
		break;
	}
}

void CDrxPSNSupport::DispatchInviteAccepted(DrxSessionID session, EDrxLobbyError error)
{
	if (m_pLobby)
	{
		m_pLobby->InviteAccepted(eCLS_Online, 0, session, error);
	}
}

void CDrxPSNSupport::OnPlatformEvent(const IPlatformOS::SPlatformEvent& event)
{
	// Filter and store events coming in and check them in the network thread
	if (event.m_eEventType == IPlatformOS::SPlatformEvent::eET_PlatformSpecific)
	{
		const IPlatformOS::SPlatformEventOrbis& orbevent = static_cast<const IPlatformOS::SPlatformEventOrbis&>(event);
		m_lobbyThreadQueueBuildingMutex.Lock();
		m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::SystemCallbackHandler, this, orbevent.m_event);
		m_lobbyThreadQueueBuildingMutex.Unlock();
	}
}

// Context callback function
void CDrxPSNSupport::ContextCallbackHandler(SceNpMatching2ContextId id, SceNpMatching2Event myEvent, SceNpMatching2EventCause eventCause, int errorCode)
{
	//-- matchmaking context id should match stored version - if they don't match, the context has probably been invalidated
	if (id == m_matchmakingCtxId)
	{
		if (errorCode >= PSN_OK)
		{
			// no error
			switch (myEvent)
			{
			case MATCHING2_CONTEXT_EVENT_STARTED:
				{
					if (m_curState == ePSNOS_Matchmaking_WaitingForContextToStart)
					{
						NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_WaitingForContextToStart -> ePSNOS_Matchmaking_ContextReady");
						m_curState = ePSNOS_Matchmaking_ContextReady;
					}
					else
					{
						NetLog("CDrxPSNSupport::ContextCallbackHandler: Not in correct state to handle this event! Reverting back to safe state ePSNOS_Online.");
						FlagPendingError(ePSNPE_IncorrectState, ePSNOS_Online);
					}
				}
				break;

			case MATCHING2_CONTEXT_EVENT_STOPPED:
				{
					// Matchmaking context has stopped. If we're in a valid matchmaking state, we need to revert to a safe non-matchmaking state
					FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
				}
				break;

			case MATCHING2_CONTEXT_EVENT_START_OVER:
				{
					// Matchmaking context has been invalidated. Revert state back to a safe non-matchmaking state
					FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
				}
				break;

			default:
				// ignore unknown states (that may be introduced in future SDK)
				break;
			}
		}
		else
		{
			//-- Some error occurred. Revert state.
			if (!HandlePSNError(errorCode))
			{
				//-- if the PSN error not handled nicely, force the state machine to a safe state anyway.
				NetLog("CDrxPSNSupport::ContextCallbackHandler: PSN error did not change state, but cannot stay in this state! Reverting back to safe state.");
				FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
			}
		}
	}
	else
	{
		//-- This should never happen, but if it does we'll ignore it.
		NetLog("CDrxPSNSupport::ContextCallbackHandler: Mismatching Matchmaking ContextID, ignoring.");
	}
}

//CALLED FROM SONY THREAD - SPECIAL HANDLING REQUIRED
void CDrxPSNSupport::PSNContextCallback(SceNpMatching2ContextId id, SceNpMatching2Event myEvent, SceNpMatching2EventCause eventCause, int errorCode, void* pArg)
{
	CDrxPSNSupport* _this = static_cast<CDrxPSNSupport*>(pArg);

	_this->m_lobbyThreadQueueBuildingMutex.Lock();
	_this->m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::ContextCallbackHandler, _this, id, myEvent, eventCause, errorCode);
	_this->m_lobbyThreadQueueBuildingMutex.Unlock();
}

void CDrxPSNSupport::SignalingCallbackHandler(uint32 id, uint32 subject, SceNpMatching2RoomId roomId, int myEvent, int errorCode)
{
	//-- It's worth noting here, that errorCode is not always an error for signaling events!
	//-- It can also be a event cause too. So we still need to look at the event even if the errorCode < 0.
	if (id == m_matchmakingCtxId)
	{
		if (subject != INVALID_PSN_ROOM_MEMBER_ID)
		{
			SDrxPSNSupportCallbackEventData evd;
			memset(&evd, 0, sizeof(evd));
			evd.m_signalEvent.m_subject = subject;
			evd.m_signalEvent.m_roomId = roomId;
			evd.m_signalEvent.m_event = myEvent;
			evd.m_signalEvent.m_error = errorCode;
			BroadcastEventToHandlers(eCE_SignalEvent, evd);

			if (evd.m_signalEvent.m_bHandled == false)
			{
				switch (evd.m_signalEvent.m_event)
				{
				case SIGNALING_EVENT_CONNECTED:
					{
						NetLog("[SIGNALING] Signaling ID %d: SIGNALING_EVENT_CONNECTED not handled: error %08X", evd.m_signalEvent.m_subject, evd.m_signalEvent.m_error);
					}
					break;
				case SIGNALING_EVENT_DEAD:
					{
						NetLog("[SIGNALING] Signaling ID %d: SIGNALING_EVENT_DEAD not handled: error %08X", evd.m_signalEvent.m_subject, evd.m_signalEvent.m_error);
					}
					break;
				default:
					{
						NetLog("[SIGNALING] Signaling ID %d: Unknown event %08X not handled: error %08X", evd.m_signalEvent.m_subject, evd.m_signalEvent.m_event, evd.m_signalEvent.m_error);
					}
				}

				if (!HandlePSNError(errorCode))
				{
					NetLog("CDrxPSNSupport::SignalingCallbackHandler() : Unexpected signaling event 0x%08X.", myEvent);
					NetLog("CDrxPSNSupport::SignalingCallbackHandler() : HandlePSNError did not alter state. Ignore error and continue.");
				}
			}
		}
		else
		{
			NetLog("CDrxPSNSupport::SignalingCallbackHandler() : INVALID_PSN_SIGNALING_ID encountered, ignoring event.");
		}
	}
	else
	{
		//-- This should never happen, but if it does, we'll ignore it.
		NetLog("CDrxPSNSupport::SignalingCallbackHandler() : Mismatching Signaling ContextID, ignoring event.");
	}
}

void CDrxPSNSupport::PSNSignalingCallback(SceNpMatching2ContextId id, SceNpMatching2RoomId roomId, SceNpMatching2RoomMemberId subject, SceNpMatching2Event myEvent, int errorCode, void* pArg)
{
	CDrxPSNSupport* _this = static_cast<CDrxPSNSupport*>(pArg);

	_this->m_lobbyThreadQueueBuildingMutex.Lock();
	_this->m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::SignalingCallbackHandler, _this, (uint32)id, (uint32)subject, roomId, (int)myEvent, errorCode);
	_this->m_lobbyThreadQueueBuildingMutex.Unlock();
}

void CDrxPSNSupport::RequestCallbackHandler(SceNpMatching2ContextId id, SceNpMatching2RequestId reqId, SceNpMatching2Event myEvent, int errorCode, TMemHdl dataHandle, uint32 dataSize)
{
	if (id == m_matchmakingCtxId)
	{
		if (reqId != INVALID_PSN_REQUEST_ID)
		{
			//-- request is not part of the CDrxPSNSupport class, so we'll broadcast it out to any registered handlers since it should belong to one of those.
			SDrxPSNSupportCallbackEventData evd;
			memset(&evd, 0, sizeof(evd));
			evd.m_requestEvent.m_reqId = reqId;
			evd.m_requestEvent.m_event = myEvent;
			evd.m_requestEvent.m_dataHdl = dataHandle;
			evd.m_requestEvent.m_dataSize = dataSize;
			evd.m_requestEvent.m_error = errorCode;
			BroadcastEventToHandlers(eCE_RequestEvent, evd);

			if (evd.m_requestEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SRequestEvent::REQUEST_EVENT_HANDLED)
			{
				if (evd.m_requestEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SRequestEvent::REQUEST_EVENT_OWNS_MEMORY)
				{
					// the event has taken ownership of the memory handle.
					// In this case we don't want this function to clear up!
					// Set the dataHandle to invalid, so it doesn't get freed at the bottom of this function
					dataHandle = TMemInvalidHdl;
				}
			}
			else
			{
				switch (evd.m_requestEvent.m_event)
				{
				case REQUEST_EVENT_GET_WORLD_INFO_LIST:
					{
						NetLog("Request ID %d: REQUEST_EVENT_GET_WORLD_INFO_LIST not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_error);
					}
					break;
				case REQUEST_EVENT_SEARCH_ROOM:
					{
						NetLog("Request ID %d: REQUEST_EVENT_SEARCH_ROOM not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_error);
					}
					break;
				case REQUEST_EVENT_CREATE_JOIN_ROOM:
					{
						NetLog("Request ID %d: REQUEST_EVENT_CREATE_JOIN_ROOM not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_error);
					}
					break;
				case REQUEST_EVENT_JOIN_ROOM:
					{
						NetLog("Request ID %d: REQUEST_EVENT_JOIN_ROOM not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_error);
					}
					break;
				case REQUEST_EVENT_LEAVE_ROOM:
					{
						NetLog("Request ID %d: REQUEST_EVENT_LEAVE_ROOM not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_error);
					}
					break;
				default:
					{
						NetLog("Request ID %d: Unknown request event %d not handled: error %08X", evd.m_requestEvent.m_reqId, evd.m_requestEvent.m_event, evd.m_requestEvent.m_error);
					}
				}

				if (!HandlePSNError(errorCode))
				{
					NetLog("CDrxPSNSupport::RequestCallbackHandler() : Unhandled request event 0x%08X.", myEvent);
					NetLog("CDrxPSNSupport::RequestCallbackHandler() : HandlePSNError did not alter state. Ignore error and continue.");
				}
			}
		}
		else
		{
			NetLog("CDrxPSNSupport::RequestCallbackHandler: INVALID_PSN_REQUEST_ID encountered, ignoring event.");
		}
	}
	else
	{
		NetLog("CDrxPSNSupport::RequestCallbackHandler: Mismatching Matchmaking Context Id, ignoring event.");
	}

	//-- If any memory was allocated for the callback, release it now (if the callback hasn't taken ownership of the memory)
	if (dataHandle)
	{
		ReleaseClonedResponse(myEvent, dataHandle);
	}
}

//CALLED FROM SONY THREAD - SPECIAL HANDLING REQUIRED
void CDrxPSNSupport::PSNRequestCallback(SceNpMatching2ContextId id, SceNpMatching2RequestId reqId, SceNpMatching2Event myEvent, int errorCode, const void* pData, void* pArg)
{
	CDrxPSNSupport* _this = static_cast<CDrxPSNSupport*>(pArg);
	TMemHdl outDataHandle = TMemInvalidHdl;
	uint32 outDataSize = 0;

	if (pData)
	{
		outDataHandle = _this->CloneResponse(myEvent, pData, &outDataSize);
		if (outDataHandle != TMemInvalidHdl)
		{
			// If any data attached was to the request event, it's now been successfully cloned to outDataHandle.
		}
		else
		{
			// We've failed to clone the data. This is probably very bad, but we'll allow it to continue.
			// The Request callback handlers for each eventType should validate the size of the buffer they are passed, and
			// handle 0 size with the correct error.
		}
	}

	_this->m_lobbyThreadQueueBuildingMutex.Lock();
	_this->m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::RequestCallbackHandler, _this, id, reqId, myEvent, errorCode, outDataHandle, outDataSize);
	_this->m_lobbyThreadQueueBuildingMutex.Unlock();
}

void CDrxPSNSupport::RoomEventCallbackHandler(SceNpMatching2ContextId id, SceNpMatching2RoomId roomId, SceNpMatching2Event myEvent, int errorCode, TMemHdl dataHandle, uint32 dataSize)
{
	if (id == m_matchmakingCtxId)
	{
		if (roomId != INVALID_PSN_ROOM_ID)
		{
			void* pData = GetLobby()->MemGetPtr(dataHandle);

			SDrxPSNSupportCallbackEventData evd;
			memset(&evd, 0, sizeof(evd));
			evd.m_roomEvent.m_event = myEvent;
			evd.m_roomEvent.m_roomId = roomId;
			evd.m_roomEvent.m_dataHdl = dataHandle;
			evd.m_roomEvent.m_dataSize = dataSize;
			evd.m_roomEvent.m_error = errorCode;
			BroadcastEventToHandlers(eCE_RoomEvent, evd);

			if (evd.m_roomEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SRoomEvent::ROOM_EVENT_HANDLED)
			{
				if (evd.m_roomEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SRoomEvent::ROOM_EVENT_OWNS_MEMORY)
				{
					// the event has taken ownership of the memory handle.
					// In this case we don't want this function to clear up!
					// Set the dataHandle to invalid, so it doesn't get freed at the bottom of this function
					dataHandle = TMemInvalidHdl;
				}
			}
			else
			{
				switch (evd.m_roomEvent.m_event)
				{
				case ROOM_EVENT_MEMBER_JOINED:
					{
						NetLog("Room ID %lld: ROOM_EVENT_MEMBER_JOINED not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_MEMBER_LEFT:
					{
						NetLog("Room ID %lld: ROOM_EVENT_MEMBER_LEFT not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_KICKED_OUT:
					{
						NetLog("Room ID %lld: ROOM_EVENT_KICKED_OUT not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_ROOM_DESTROYED:
					{
						NetLog("Room ID %lld: ROOM_EVENT_ROOM_DESTROYED not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_ROOM_OWNER_CHANGED:
					{
						NetLog("Room ID %lld: ROOM_EVENT_ROOM_OWNER_CHANGED not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_UPDATED_ROOM_DATA_INTERNAL:
					{
						NetLog("Room ID %lld: ROOM_EVENT_UPDATED_ROOM_DATA_INTERNAL not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_UPDATED_ROOM_MEMBER_DATA_INTERNAL:
					{
						NetLog("Room ID %lld: ROOM_EVENT_UPDATED_ROOM_MEMBER_DATA_INTERNAL not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				case ROOM_EVENT_UPDATED_SIGNALING_OPT_PARAM:
					{
						NetLog("Room ID %lld: ROOM_EVENT_UPDATED_SIGNALING_OPT_PARAM not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_error);
					}
					break;
				default:
					{
						NetLog("Room ID %lld: Unknown room event %d not handled: error %08X", (long long)evd.m_roomEvent.m_roomId, evd.m_roomEvent.m_event, evd.m_roomEvent.m_error);
					}
					break;
				}

				if (!HandlePSNError(errorCode))
				{
					NetLog("CDrxPSNSupport::RoomEventCallbackHandler() : Unhandled room event 0x%08X.", myEvent);
					NetLog("CDrxPSNSupport::RoomEventCallbackHandler() : HandlePSNError did not alter state. Ignore error and continue.");
				}
			}
		}
		else
		{
			NetLog("CDrxPSNSupport::RoomEventCallbackHandler: INVALID_PSN_ROOM_ID encountered, ignoring event.");
		}
	}
	else
	{
		NetLog("CDrxPSNSupport::RoomEventCallbackHandler: Mismatching Context Id, ignoring event.");
	}

	//-- If any memory was allocated for the callback, release it now (if the callback hasn't taken ownership of the memory)
	if (dataHandle)
	{
		ReleaseClonedResponse(myEvent, dataHandle);
	}
}

//CALLED FROM SONY THREAD - SPECIAL HANDLING REQUIRED
void CDrxPSNSupport::PSNRoomEventCallback(SceNpMatching2ContextId id, SceNpMatching2RoomId roomId, SceNpMatching2Event myEvent, const void* pData, void* pArg)
{
	CDrxPSNSupport* _this = static_cast<CDrxPSNSupport*>(pArg);
	TMemHdl outDataHandle = TMemInvalidHdl;
	uint32 outDataSize = 0;

	if (pData)
	{
		outDataHandle = _this->CloneResponse(myEvent, pData, &outDataSize);
		if (outDataHandle != TMemInvalidHdl)
		{
			// If any data attached was to the request event, it's now been successfully cloned to outDataHandle.
		}
		else
		{
			// We've failed to clone the data. This is probably very bad, but we'll allow it to continue.
			// The Room event callback handlers for each eventType should validate the size of the buffer they are passed, and
			// handle 0 size with the correct error.
		}
	}

	_this->m_lobbyThreadQueueBuildingMutex.Lock();
	_this->m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::RoomEventCallbackHandler, _this, id, roomId, myEvent, 0, outDataHandle, outDataSize);
	_this->m_lobbyThreadQueueBuildingMutex.Unlock();
}

void CDrxPSNSupport::WebApiCallbackHandler(DrxWebApiJobId id, int errorCode, TMemHdl responseBodyHdl)
{
	SDrxPSNSupportCallbackEventData evd;
	memset(&evd, 0, sizeof(evd));
	evd.m_webApiEvent.m_error = errorCode;
	evd.m_webApiEvent.m_id = id;
	if (responseBodyHdl != TMemInvalidHdl)
	{
		evd.m_webApiEvent.m_pResponseBody = (SDrxPSNWebApiResponseBody*)GetLobby()->MemGetPtr(responseBodyHdl);
		//DrxLog("CDrxPSNSupport::WebApiCallbackHandler() : %d - %p\n", responseBodyHdl, evd.m_webApiEvent.m_pResponseBody);
	}

	BroadcastEventToHandlers(eCE_WebApiEvent, evd);

	if (evd.m_webApiEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_HANDLED)
	{
		if (evd.m_webApiEvent.m_returnFlags & SDrxPSNSupportCallbackEventData::SWebApiEvent::WEBAPI_EVENT_OWNS_MEMORY)
		{
			// the event has taken ownership of the json tree.
			evd.m_webApiEvent.m_pResponseBody = NULL;
		}
		else
		{
			//-- If any memory was allocated for the callback, release it now (if the callback hasn't taken ownership of the memory)
			if (responseBodyHdl != TMemInvalidHdl)
			{
				m_WebApiUtility.FreeResponseBody(responseBodyHdl);
				evd.m_webApiEvent.m_pResponseBody = NULL;
			}
		}
	}
	else
	{
		NetLog("CDrxPSNSupport::WebApiCallbackHandler() : WebApiJob ID %d not handled: error %08X", evd.m_webApiEvent.m_id, evd.m_webApiEvent.m_error);

		//-- If any memory was allocated for the callback, release it now (if the callback hasn't taken ownership of the memory)
		if (responseBodyHdl != TMemInvalidHdl)
		{
			m_WebApiUtility.FreeResponseBody(responseBodyHdl);
			evd.m_webApiEvent.m_pResponseBody = NULL;
		}

		if (!HandlePSNError(errorCode))
		{
			NetLog("CDrxPSNSupport::WebApiCallbackHandler() : HandlePSNError did not alter state. Ignore error and continue.");
		}
	}
}

// CALLED FROM WEBAPI THREAD
void CDrxPSNSupport::ReturnWebApiEvent(DrxWebApiJobId id, int errorCode, TMemHdl responseBodyHdl)
{
	m_lobbyThreadQueueBuildingMutex.Lock();
	m_lobbyThreadQueueBuilding.Add(&CDrxPSNSupport::WebApiCallbackHandler, this, id, errorCode, responseBodyHdl);
	m_lobbyThreadQueueBuildingMutex.Unlock();
}

void CDrxPSNSupport::TransitionSigninDialogOpen()
{
	assert(m_curState == ePSNOS_Offline);

	// On Orbis, skip all signin dialogs and go straight to NpId discovery.
	NetLog("[PSN SUPPORT] State update: ePSNOS_Offline -> ePSNOS_DiscoverNpId");
	m_curState = ePSNOS_DiscoverNpId;
}

void CDrxPSNSupport::TransitionSigninDialogClose()
{
	// Orbis does nothing
}

void CDrxPSNSupport::DispatchChatRestrictedEvent(int isRestricted)
{
	UDrxLobbyEventData data;
	SDrxLobbyChatRestrictedData chatData;
	data.pChatRestrictedData = &chatData;

	chatData.m_user = 0;
	chatData.m_chatRestricted = isRestricted;

	m_pLobby->DispatchEvent(eCLSE_ChatRestricted, data);
}

void CDrxPSNSupport::TransitionDiscoverNpId()
{
	assert(m_curState == ePSNOS_DiscoverNpId);

	int ret;

	SceUserServiceInitializeParams initParams;
	memset(&initParams, 0, sizeof(initParams));
	initParams.priority = SCE_KERNEL_PRIO_FIFO_DEFAULT;

	ret = sceUserServiceInitialize(&initParams);
	// ret probably will be "error_already_intialised" because chances are something will have already initialised this before networking
	if ((ret == PSN_OK) || (ret == SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED))
	{
		ret = sceUserServiceGetInitialUser(&m_myUserId);
		if (ret == PSN_OK)
		{
			ret = sceNpGetNpId(m_myUserId, &m_myNpId);
			if (ret == PSN_OK)
			{
				NetLog("[PSN SUPPORT] State update: ePSNOS_DiscoverNpId -> ePSNOS_OnlinePrivilegeTest");
				m_curState = ePSNOS_OnlinePrivilegeTest;
			}
			else
			{
				NetLog("sceNpGetNpId() : Error %08X", ret);
				if (!HandlePSNError(ret))
				{
					//-- failed to handle PSN error nicely, and we can't stay in this state, so force a new state.
					NetLog("CDrxPSNSupport::TransitionDiscoverNpId: PSN error did not force a state change, but we cannot stay in this state. Reverting to safe state.");
					FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
				}
			}
		}
		else
		{
			NetLog("sceUserServiceGetInitialUser() : Error %08X", ret);
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
		}
	}
	else
	{
		NetLog("sceUserServiceInitialize() : Error %08X", ret);
		FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
	}
}

void CDrxPSNSupport::TransitionOnlinePrivilegeTest()
{
	assert(m_curState == ePSNOS_OnlinePrivilegeTest);

		#if defined(DO_ORBIS_ONLINE_PRIVILEGE_TEST)
	int ret;
	SceNpCreateAsyncRequestParameter param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.cpuAffinityMask = SCE_KERNEL_CPUMASK_USER_ALL;
	param.threadPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;

	ret = sceNpCreateAsyncRequest(&param);
	if (ret > PSN_OK)
	{
		m_onlinePrivTestReqId = ret;

		ret = sceNpCheckNpAvailability(m_onlinePrivTestReqId, &m_myNpId.handle, NULL);
		if (ret == PSN_OK)
		{
			NetLog("[PSN SUPPORT] State update: ePSNOS_OnlinePrivilegeTest -> ePSNOS_WaitingForOnlinePrivilegeTest");
			m_curState = ePSNOS_WaitingForOnlinePrivilegeTest;
		}
		else
		{
			NetLog("sceNpCheckNpAvailability() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_AgeRestricted, ePSNOS_Offline);
			}
			return;
		}
	}
	else
	{
		// Error handling
		NetLog("sceNpCreateAsyncRequest() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
		}
		return;
	}
		#else
	NetLog("[PSN SUPPORT] State update: ePSNOS_OnlinePrivilegeTest -> ePSNOS_RegisterOnlineServices");
	m_curState = ePSNOS_RegisterOnlineServices;
		#endif //DO_ORBIS_ONLINE_PRIVILEGE_TEST
}

void CDrxPSNSupport::TransitionWaitingForOnlinePrivilegeTest()
{
	assert(m_curState == ePSNOS_WaitingForOnlinePrivilegeTest);

		#if defined(DO_ORBIS_ONLINE_PRIVILEGE_TEST)
	int ret, result;
	ret = sceNpPollAsync(m_onlinePrivTestReqId, &result);
	if (ret > PSN_OK)
	{
		// still running
	}
	else
	{
		// complete
		sceNpDeleteRequest(m_onlinePrivTestReqId);
		m_onlinePrivTestReqId = INVALID_PRIVILEGE_REQUEST_ID;

		if (ret == PSN_OK)
		{
			if (result == PSN_OK)
			{
				NetLog("[PSN SUPPORT] State update: ePSNOS_WaitingForOnlinePrivilegeTest -> ePSNOS_OnlinePrivilegeRestrictions");
				m_curState = ePSNOS_OnlinePrivilegeRestrictions;
			}
			else
			{
				if (result == SCE_NP_ERROR_AGE_RESTRICTION)
				{
					// age restriction
					FlagPendingError(ePSNPE_AgeRestricted, ePSNOS_Offline);
				}
				else
				{
					NetLog("sceNpGetParentalControlInfo result in sceNpPollAsync() : Error %08X", result);
					if (!HandlePSNError(result))
					{
						FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
					}
				}
				return;
			}
		}
		else
		{
			NetLog("sceNpPollAsync() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
			}
			return;
		}
	}
		#else
	NetLog("[PSN SUPPORT] State update: ePSNOS_WaitingForOnlinePrivilegeTest -> ePSNOS_RegisterOnlineServices");
	m_curState = ePSNOS_RegisterOnlineServices;
		#endif //DO_ORBIS_ONLINE_PRIVILEGE_TEST
}

void CDrxPSNSupport::TransitionOnlinePrivilegeRestrictions()
{
	assert(m_curState == ePSNOS_OnlinePrivilegeRestrictions);

		#if defined(DO_ORBIS_ONLINE_PRIVILEGE_TEST)
	int ret;
	SceNpCreateAsyncRequestParameter param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.cpuAffinityMask = SCE_KERNEL_CPUMASK_USER_ALL;
	param.threadPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;

	ret = sceNpCreateAsyncRequest(&param);
	if (ret > PSN_OK)
	{
		m_onlinePrivRestrictionsReqId = ret;

		ret = sceNpGetParentalControlInfo(m_onlinePrivRestrictionsReqId, &m_myNpId.handle, &m_onlinePrivAge, &m_onlinePrivRestrictionsResult);
		if (ret == PSN_OK)
		{
			NetLog("[PSN SUPPORT] State update: ePSNOS_OnlinePrivilegeRestrictions -> ePSNOS_WaitingForOnlinePrivilegeRestrictions");
			m_curState = ePSNOS_WaitingForOnlinePrivilegeRestrictions;
		}
		else
		{
			NetLog("sceNpGetParentalControlInfo() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
			}
			return;
		}
	}
	else
	{
		// Error handling
		NetLog("sceNpCreateAsyncRequest() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
		}
		return;
	}
		#else
	NetLog("[PSN SUPPORT] State update: ePSNOS_OnlinePrivilegeRestrictions -> ePSNOS_RegisterOnlineServices");
	m_curState = ePSNOS_RegisterOnlineServices;
		#endif //DO_ORBIS_ONLINE_PRIVILEGE_TEST
}

void CDrxPSNSupport::TransitionWaitingForOnlinePrivilegeRestrictions()
{
	assert(m_curState == ePSNOS_WaitingForOnlinePrivilegeRestrictions);

		#if defined(DO_ORBIS_ONLINE_PRIVILEGE_TEST)
	int ret, result;
	ret = sceNpPollAsync(m_onlinePrivRestrictionsReqId, &result);
	if (ret > PSN_OK)
	{
		// still running
	}
	else
	{
		// complete
		sceNpDeleteRequest(m_onlinePrivRestrictionsReqId);
		m_onlinePrivRestrictionsReqId = INVALID_PRIVILEGE_REQUEST_ID;

		if (ret == PSN_OK)
		{
			if (result == PSN_OK)
			{
				if (m_onlinePrivRestrictionsResult.chatRestriction)
				{
					m_isChatRestricted = 1;
					TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchChatRestrictedEvent, this, m_isChatRestricted);
				}

				NetLog("[PSN SUPPORT] State update: ePSNOS_WaitingForOnlinePrivilegeRestrictions -> ePSNOS_RegisterOnlineServices");
				m_curState = ePSNOS_RegisterOnlineServices;
			}
			else
			{
				NetLog("sceNpGetParentalControlInfo result in sceNpPollAsync() : Error %08X", result);
				if (!HandlePSNError(result))
				{
					FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
				}
				return;
			}
		}
		else
		{
			NetLog("sceNpPollAsync() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
			}
			return;
		}
	}
		#else
	NetLog("[PSN SUPPORT] State update: ePSNOS_WaitingForOnlinePrivilegeRestrictions -> ePSNOS_RegisterOnlineServices");
	m_curState = ePSNOS_RegisterOnlineServices;
		#endif //DO_ORBIS_ONLINE_PRIVILEGE_TEST
}

void CDrxPSNSupport::TransitionRegisterOnlineServices()
{
	assert(m_curState == ePSNOS_RegisterOnlineServices);
	int ret;

		#if USE_DRX_STATS
	if (m_features & eCLSO_Stats)
	{
		SceNpServiceLabel label = 0;

		assert(m_scoringCtxId == INVALID_PSN_STATS_CONTEXT);

		ret = sceNpScoreCreateNpTitleCtx(label, &m_myNpId);
		if (ret > PSN_OK)
		{
			m_scoringCtxId = ret;
		}
		else
		{
			NetLog("sceNpScoreCreateTitleCtx() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_ContextError, ePSNOS_Offline);
			}
			return;
		}

		assert(m_tusCtxId == INVALID_PSN_STATS_CONTEXT);

		ret = sceNpTusCreateNpTitleCtx(label, &m_myNpId);
		if (ret > PSN_OK)
		{
			m_tusCtxId = ret;
		}
		else
		{
			NetLog("sceNpTusCreateTitleCtx() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_ContextError, ePSNOS_Offline);
			}
			return;
		}
	}
		#endif //USE_DRX_STATS

	ret = m_WebApiUtility.Initialise(m_webApiCtxId, this);
	if (ret < PSN_OK)
	{
		if (!HandlePSNError(ret))
		{
			FlagPendingError(ePSNPE_ContextError, ePSNOS_Offline);
		}
		return;
	}

	ret = m_WebApiController.Initialise(&m_WebApiUtility, m_myNpId);
	if (ret < PSN_OK)
	{
		if (!HandlePSNError(ret))
		{
			FlagPendingError(ePSNPE_ContextError, ePSNOS_Offline);
		}
		return;
	}

	NetLog("[PSN SUPPORT] State update: ePSNOS_RegisterOnlineServices -> ePSNOS_Online");
	m_curState = ePSNOS_Online;
}

void CDrxPSNSupport::TransitionMatchmakingPrivilegeTest()
{
	assert(m_curState == ePSNOS_Matchmaking_Initialise);

		#if defined(DO_ORBIS_PLUS_PRIVILEGE_TEST)

	SConfigurationParams neededInfo = {
		CLCC_PSN_PLUS_TEST_REQUIRED, { NULL }
	};
	neededInfo.m_32 = FALSE;
	m_pLobby->GetConfigurationInformation(&neededInfo, 1);
	if (neededInfo.m_32 == TRUE)
	{
		int ret;
		SceNpCreateAsyncRequestParameter param;

		memset(&param, 0, sizeof(param));
		param.size = sizeof(param);
		param.cpuAffinityMask = SCE_KERNEL_CPUMASK_USER_ALL;
		param.threadPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;

		ret = sceNpCreateAsyncRequest(&param);
		if (ret > PSN_OK)
		{
			m_plusPrivReqId = ret;

			SceNpCheckPlusParameter plusparam;
			memset(&plusparam, 0, sizeof(plusparam));
			plusparam.size = sizeof(plusparam);
			plusparam.userId = m_myUserId;
			plusparam.features = SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY;

			ret = sceNpCheckPlus(m_plusPrivReqId, &plusparam, &m_plusPrivResult);
			if (ret == PSN_OK)
			{
				NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_Initialise -> ePSNOS_Matchmaking_WaitingForPrivilegeTest");
				m_curState = ePSNOS_Matchmaking_WaitingForPrivilegeTest;
			}
			else
			{
				NetLog("sceNpCheckPlus() : Error %08X", ret);
				if (!HandlePSNError(ret))
				{
					FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
				}
				return;
			}
		}
		else
		{
			// Error handling
			NetLog("sceNpCreateAsyncRequest() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
			}
			return;
		}
	}
	else
	{
		NetLog("[PSN SUPPORT] PS Plus test not required.");
		NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_Initialise -> ePSNOS_Matchmaking_StartContext");
		m_curState = ePSNOS_Matchmaking_StartContext;
	}
		#else
	NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_Initialise -> ePSNOS_Matchmaking_StartContext");
	m_curState = ePSNOS_Matchmaking_StartContext;
		#endif // DO_ORBIS_PLUS_PRIVILEGE_TEST
}

void CDrxPSNSupport::TransitionMatchmakingWaitingForPrivilegeTest()
{
	assert(m_curState == ePSNOS_Matchmaking_WaitingForPrivilegeTest);

	int ret, result;
	ret = sceNpPollAsync(m_plusPrivReqId, &result);
	if (ret > PSN_OK)
	{
		// still running
	}
	else
	{
		// complete
		sceNpDeleteRequest(m_plusPrivReqId);
		m_plusPrivReqId = INVALID_PRIVILEGE_REQUEST_ID;

		if (ret == PSN_OK)
		{
			if (result == PSN_OK)
			{
				if (m_plusPrivResult.authorized)
				{
					NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_WaitingForPrivilegeTest -> ePSNOS_Matchmaking_StartContext");
					m_curState = ePSNOS_Matchmaking_StartContext;
				}
				else
				{
					ret = sceNpCommerceDialogInitialize();
					if ((ret == PSN_OK) || (ret == SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED))
					{
						SceNpCommerceDialogParam commerceParams;
						sceNpCommerceDialogParamInitialize(&commerceParams);
						commerceParams.mode = SCE_NP_COMMERCE_DIALOG_MODE_PLUS;
						commerceParams.userId = m_myUserId;
						commerceParams.features = SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY;

						ret = sceNpCommerceDialogOpen(&commerceParams);
						if (ret == PSN_OK)
						{
							m_curState = ePSNOS_Matchmaking_WaitingForPrivilegeDialog;
						}
						else
						{
							if (!HandlePSNError(ret))
							{
								FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
							}
							return;
						}
					}
					else
					{
						NetLog("sceNpCommerceDialogInitialize failed. ret = 0x%x\n", ret);
						FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
						return;
					}
				}
			}
			else
			{
				NetLog("sceNpCheckPlus result in sceNpPollAsync() : Error %08X", result);
				if (!HandlePSNError(result))
				{
					FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
				}
				return;
			}
		}
		else
		{
			NetLog("sceNpPollAsync() : Error %08X", ret);
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
			}
			return;
		}
	}
}

void CDrxPSNSupport::TransitionMatchmakingWaitingForPrivilegeDialog()
{
	assert(m_curState == ePSNOS_Matchmaking_WaitingForPrivilegeDialog);

	int ret;

	ret = sceNpCommerceDialogUpdateStatus();
	if (ret == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		// completed.
		SceNpCommerceDialogResult commerceResult;
		memset(&commerceResult, 0, sizeof(commerceResult));

		ret = sceNpCommerceDialogGetResult(&commerceResult);
		sceNpCommerceDialogTerminate();
		if (ret >= PSN_OK)
		{
			// dialog closed
			if (commerceResult.authorized)
			{
				// Plus activated, we can continue!
				NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_WaitingForPrivilegeDialog -> ePSNOS_Matchmaking_StartContext");
				m_curState = ePSNOS_Matchmaking_StartContext;
			}
			else
			{
				// not authorized for plus, abort
				FlagPendingError(ePSNPE_RestrictedPrivileges, ePSNOS_Online);
				return;
			}
		}
		else
		{
			// error
			if (!HandlePSNError(ret))
			{
				FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
			}
			return;
		}
	}
}

void CDrxPSNSupport::TransitionMatchmakingStartContext()
{
	assert(m_curState == ePSNOS_Matchmaking_StartContext);

	int ret;

	// Create Matching2 Context
	// Create a context (we will use the same one at all times, while still signed in)
	assert(m_matchmakingCtxId == INVALID_PSN_MATCHMAKING_CONTEXT);

	SceNpMatching2CreateContextParam param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.serviceLabel = 0; // I don't think we need a special service label, so 0 is ok?
	param.npId = &m_myNpId;

	ret = sceNpMatching2CreateContext(&param, &m_matchmakingCtxId);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2CreateContext() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, restart this state next time
			FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		}
		return;
	}

	// Register the context callback handler
	ret = sceNpMatching2RegisterContextCallback(PSNContextCallback, this);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2RegisterContextCallback() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, revert to safe state
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
		}
		return;
	}

	// Start the context
	ret = sceNpMatching2ContextStart(m_matchmakingCtxId, CONTEXT_TIMEOUT);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2ContextStart/Async() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, revert to safe state
			FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		}
		return;
	}

	NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_StartContext -> ePSNOS_Matchmaking_WaitingForContextToStart");
	m_curState = ePSNOS_Matchmaking_WaitingForContextToStart;
}

void CDrxPSNSupport::TransitionMatchmakingRegisterHandlers()
{
	assert(m_curState == ePSNOS_Matchmaking_ContextReady);

	// Set default option parameters for this context - means we don't need to pass optParam to any query functions
	// This sets the PSNRequestCallback
	SceNpMatching2RequestOptParam optParam;
	memset(&optParam, 0, sizeof(optParam));
	optParam.cbFunc = PSNRequestCallback;
	optParam.cbFuncArg = this;
	optParam.timeout = REQUEST_TIMEOUT;
	optParam.appReqId = 0;

	int ret;

	ret = sceNpMatching2SetDefaultRequestOptParam(m_matchmakingCtxId, &optParam);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2SetDefaultRequestOptParam() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, and we can't stay in this state, so force a new state.
			NetLog("CDrxPSNSupport::TransitionMatchmakingRegisterHandlers: PSN error did not force a state change, but we cannot stay in this state. Reverting to safe state.");
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
		}
		return;
	}

	// Set PSNRoomCallback
	ret = sceNpMatching2RegisterRoomEventCallback(m_matchmakingCtxId, PSNRoomEventCallback, this);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2RegisterRoomEventCallback() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, revert to safe state
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
		}
		return;
	}

	ret = sceNpMatching2RegisterSignalingCallback(m_matchmakingCtxId, PSNSignalingCallback, this);
	if (ret < PSN_OK)
	{
		NetLog("sceNpMatching2RegisterSignalingCallback() : Error %08X", ret);
		if (!HandlePSNError(ret))
		{
			//-- failed to handle PSN error nicely, revert to safe state
			FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
		}
		return;
	}

	memset(&m_netInfo, 0, sizeof(m_netInfo));
	m_netInfo.size = sizeof(m_netInfo);

	ret = sceNpMatching2SignalingGetLocalNetInfo(&m_netInfo);
	if (ret != PSN_OK)
	{
		NetLog("sceNpSignalingGetLocalNetInfo() : Error %08X", ret);
		// this isn't fatal so can just continue as normal
	}

	NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_ContextReady -> ePSNOS_MatchMaking_ReadyToEnumerateServers");
	m_curState = ePSNOS_Matchmaking_ReadyToEnumerateServers;
}

void CDrxPSNSupport::TransitionMatchmakingEnumerateServers()
{
	assert(m_curState == ePSNOS_Matchmaking_ReadyToEnumerateServers);

	int ret = sceNpMatching2GetServerId(m_matchmakingCtxId, &m_serverBestInfo.server.serverId);
	if (ret == PSN_OK)
	{
		// got server.
		// On Orbis, this is everything we need for server, so we can skip all the ServerInfoDiscovery/GettingNextServer/ServerContext/ServerJoined/etc loop stuff.
		// Go directly to ServerJoined.
		NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_ReadyToEnumerateServers -> ePSNOS_Matchmaking_ServerJoined");
		m_curState = ePSNOS_Matchmaking_ServerJoined;
	}
	else
	{
		if (!HandlePSNError(ret))
		{
			//-- There are no servers available. Flag the error and stay in this state so we can try to get server again next time.
			FlagPendingError(ePSNPE_NoServersAvailable, ePSNOS_Online);
		}
	}
}

void CDrxPSNSupport::TransitionMatchmakingServerInfoDiscovery()
{
	// Orbis doesn't get server info, so it shouldn't ever be in this function.
	DrxFatalError("CDrxPSNSupport::TransitionServerInfoDiscovery() not used on Orbis! Shouldn't be here!");
}

void CDrxPSNSupport::TransitionMatchmakingNextServer()
{
	// Orbis doesn't get server info, so it shouldn't ever be in this function.
	DrxFatalError("CDrxPSNSupport::TransitionServerInfoDiscovery() not used on Orbis! Shouldn't be here!");
}

void CDrxPSNSupport::TransitionMatchmakingCreateServerContext()
{
	// Orbis doesn't use the notion of ServerContext , so it shouldn't ever be in this function.
	DrxFatalError("CDrxPSNSupport::TransitionMatchmakingCreateServerContext() not used on Orbis! Shouldn't be here!");
}

void CDrxPSNSupport::TransitionMatchmakingAvailable()
{
	assert(m_curState == ePSNOS_Matchmaking_ServerJoined);

	NetLog("[PSN SUPPORT] State update: ePSNOS_Matchmaking_ServerJoined -> ePSNOS_Matchmaking_Available");

	PrepareOnlineStatus(ePSNOS_Matchmaking_Available, ePSNPE_None);
	m_curState = ePSNOS_Matchmaking_Available;
}

void CDrxPSNSupport::PrepareOnlineStatus(EDrxPSNOnlineState state, EDrxPSNPendingError eReason)
{
	int ret;
	bool bConnected = true;

	SceNpState connectedToPSN = SCE_NP_STATE_SIGNED_IN;
	SceUserServiceUserId userId = SCE_USER_SERVICE_USER_ID_INVALID;
	ret = sceUserServiceGetInitialUser(&userId);
	DRX_ASSERT(ret > -1);

	ret = sceNpGetState(userId, &connectedToPSN);
	if ((ret == PSN_OK) && (connectedToPSN != SCE_NP_STATE_SIGNED_IN))
	{
		bConnected = false;
	}

	if (state >= ePSNOS_Online)
	{
		// if state >= ePSNOS_Online, we are online.
		TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchOnlineStatus, this, eOS_SignedIn, MapSupportErrorToLobbyError(eReason), bConnected);
	}
	else
	{
		if (m_bIsTransitioningActive)
		{
			// Else if transitioning forwards, we're 'signing in'
			TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchOnlineStatus, this, eOS_SigningIn, MapSupportErrorToLobbyError(eReason), bConnected);
		}
		else
		{
			// If not transitioning forwards, we are 'signed out'
			// But only if we have have gone beyond signed into PSN stage (to avoid' signed out' messages if you weren't signed into PSN)
			if (GetLocalNpId())
			{
				TO_GAME_FROM_LOBBY(&CDrxPSNSupport::DispatchOnlineStatus, this, eOS_SignedOut, MapSupportErrorToLobbyError(eReason), bConnected);
			}
		}
	}
}

void CDrxPSNSupport::CleanTo(EDrxPSNOnlineState state, EDrxPSNPendingError eReason)
{
	int ret;

	switch (state)
	{
	default:
		{
			NET_ASSERT(!"CDrxPSNSupport::CleanTo() : Shouldn't be cleaning to this state!");
		}
		return;

	case ePSNOS_Uninitialised:
	case ePSNOS_Offline:
	case ePSNOS_Online:
		//-- these are ok
		break;
	}

	m_serverBestInfo.server.serverId = INVALID_PSN_SERVER_ID;

	if (m_matchmakingCtxId != INVALID_PSN_MATCHMAKING_CONTEXT)
	{
		ret = sceNpMatching2DestroyContext(m_matchmakingCtxId);
		if (ret < PSN_OK)
		{
			NetLog("sceNpMatching2DestroyContext() : Error %08X", ret);
		}
		m_matchmakingCtxId = INVALID_PSN_MATCHMAKING_CONTEXT;
	}

	if (m_plusPrivReqId != INVALID_PRIVILEGE_REQUEST_ID)
	{
		ret = sceNpDeleteRequest(m_plusPrivReqId);
		if (ret < PSN_OK)
		{
			NetLog("sceNpDeleteRequest() : Error %08X", ret);
		}
		m_plusPrivReqId = INVALID_PRIVILEGE_REQUEST_ID;
		memset(&m_plusPrivResult, 0, sizeof(m_plusPrivResult));
	}

	if (state == ePSNOS_Online)
	{
		//-- stop here
		if (m_bIsTransitioningActive == true)
		{
			m_bIsTransitioningActive = false;
			PrepareOnlineStatus(state, eReason);
		}
		m_wantedState = state;
		m_curState = state;
		return;
	}

	m_WebApiController.Terminate();
	m_WebApiUtility.Terminate();

		#if USE_DRX_STATS
	if (m_features & eCLSO_Stats)
	{
		if (m_tusCtxId != INVALID_PSN_STATS_CONTEXT)
		{
			ret = sceNpTusDeleteNpTitleCtx(m_tusCtxId);
			if (ret < PSN_OK)
			{
				NetLog("sceNpTusDestroyTitleCtx() failed. ret = 0x%x\n", ret);
			}
			m_tusCtxId = INVALID_PSN_STATS_CONTEXT;
		}

		if (m_scoringCtxId != INVALID_PSN_STATS_CONTEXT)
		{
			ret = sceNpScoreDeleteNpTitleCtx(m_scoringCtxId);
			if (ret < PSN_OK)
			{
				NetLog("sceNpScoreDestroyTitleCtx() failed. ret = 0x%x\n", ret);
			}
			m_scoringCtxId = INVALID_PSN_STATS_CONTEXT;
		}
	}
		#endif

	if (m_onlinePrivRestrictionsReqId != INVALID_PRIVILEGE_REQUEST_ID)
	{
		ret = sceNpDeleteRequest(m_onlinePrivRestrictionsReqId);
		if (ret < PSN_OK)
		{
			NetLog("sceNpDeleteRequest() : Error %08X", ret);
		}
		m_onlinePrivRestrictionsReqId = INVALID_PRIVILEGE_REQUEST_ID;
		memset(&m_onlinePrivRestrictionsResult, 0, sizeof(m_onlinePrivRestrictionsResult));
	}
	if (m_onlinePrivTestReqId != INVALID_PRIVILEGE_REQUEST_ID)
	{
		ret = sceNpDeleteRequest(m_onlinePrivTestReqId);
		if (ret < PSN_OK)
		{
			NetLog("sceNpDeleteRequest() : Error %08X", ret);
		}
		m_onlinePrivTestReqId = INVALID_PRIVILEGE_REQUEST_ID;
	}

	m_myUserId = SCE_USER_SERVICE_USER_ID_INVALID;
	memset(&m_myNpId, 0, sizeof(SceNpId));
	m_isChatRestricted = 0;

	if (state == ePSNOS_Offline)
	{
		//-- stop here
		if (m_bIsTransitioningActive == true)
		{
			m_bIsTransitioningActive = false;
			PrepareOnlineStatus(state, eReason);
		}
		m_wantedState = state;
		m_curState = state;
		return;
	}

	//-- If there was any uninitialise work to do, do it here

	//-- Uninitialised
	if (m_bIsTransitioningActive == true)
	{
		m_bIsTransitioningActive = false;
		PrepareOnlineStatus(state, eReason);
	}
	m_wantedState = state;
	m_curState = state;
}

void CDrxPSNSupport::ResumeTransitioning(EDrxPSNOnlineState state)
{
	switch (state)
	{
	case ePSNOS_Uninitialised:
	case ePSNOS_Offline:
	case ePSNOS_Online:
	case ePSNOS_Matchmaking_Available:
		{
			if (m_bIsTransitioningActive == false)
			{
				m_bIsTransitioningActive = true;
				PrepareOnlineStatus(m_curState, ePSNPE_None);
			}
			m_wantedState = state;
		}
		break;
	default:
		{
			NET_ASSERT(!"CDrxPSNSupport::ResumeTransitioning() : Shouldn't be resuming transioning to this state!");
		}
		return;
	}
}

bool CDrxPSNSupport::HasTransitioningReachedState(EDrxPSNOnlineState state)
{
	return (m_curState >= state);
}

void CDrxPSNSupport::DispatchOnlineStatus(EOnlineState eState, EDrxLobbyError eReason, bool bIsConnected)
{
	if (m_pLobby)
	{
		UDrxLobbyEventData data;
		SDrxLobbyOnlineStateData stateData;
		data.pOnlineStateData = &stateData;

		stateData.m_user = 0;
		stateData.m_curState = eState;
		stateData.m_reason = eReason;
		stateData.m_serviceConnected = bIsConnected;

		m_pLobby->DispatchEvent(eCLSE_OnlineState, data);
	}
}

void CDrxPSNSupport::Tick()
{
	if (m_lobbyThreadQueueBuildingMutex.TryLock())
	{
		//Copy the events into a queue just used on this thread and process the events
		m_lobbyThreadQueueProcessing.FlushEmpty();
		m_lobbyThreadQueueBuilding.Swap(m_lobbyThreadQueueProcessing);
		m_lobbyThreadQueueBuildingMutex.Unlock();

		m_lobbyThreadQueueProcessing.Flush(false);
	}

	sceNpCheckCallback();

	if (m_curState >= ePSNOS_Online)
	{
		SceNpNotifyPlusFeatureParameter param;
		memset(&param, 0, sizeof(param));
		param.size = sizeof(param);
		param.userId = m_myUserId;
		param.features = SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY;

		sceNpNotifyPlusFeature(&param);
	}

	if (IsRunning())
	{
		switch (m_curState)
		{
		case ePSNOS_Uninitialised:
		case ePSNOS_WaitingForDialogToOpen:               // Holding slot (waiting for external callback to trigger before moving on)
		case ePSNOS_WaitingForDialogToClose:
		case ePSNOS_Matchmaking_WaitingForContextToStart:
		case ePSNOS_Matchmaking_WaitingForServerInfo:
		case ePSNOS_Matchmaking_WaitingForServerContext:
		case ePSNOS_Matchmaking_Available:
			// NOOP
			break;

		case ePSNOS_Online:
		#if USE_DRX_MATCHMAKING
			if (m_features & eCLSO_Matchmaking)
			{
				if (m_wantedState > ePSNOS_Online)
				{
					NetLog("[PSN SUPPORT] State update: ePSNOS_Online -> ePSNOS_Matchmaking_Initialise");
					m_curState = ePSNOS_Matchmaking_Initialise;
				}
			}
		#endif
			break;

		default:
			NetLog("Unexpected state: %d", m_curState);
			break;

		case ePSNOS_Offline:
			TransitionSigninDialogOpen();
			break;
		case ePSNOS_DialogFinished:
			TransitionSigninDialogClose();
			break;
		case ePSNOS_DiscoverNpId:
			TransitionDiscoverNpId();
			break;
		case ePSNOS_OnlinePrivilegeTest:
			TransitionOnlinePrivilegeTest();
			break;
		case ePSNOS_WaitingForOnlinePrivilegeTest:
			TransitionWaitingForOnlinePrivilegeTest();
			break;
		case ePSNOS_OnlinePrivilegeRestrictions:
			TransitionOnlinePrivilegeRestrictions();
			break;
		case ePSNOS_WaitingForOnlinePrivilegeRestrictions:
			TransitionWaitingForOnlinePrivilegeRestrictions();
			break;
		case ePSNOS_RegisterOnlineServices:
			TransitionRegisterOnlineServices();
			break;

		case ePSNOS_Matchmaking_Initialise:
			TransitionMatchmakingPrivilegeTest();
			break;
		case ePSNOS_Matchmaking_WaitingForPrivilegeTest:
			TransitionMatchmakingWaitingForPrivilegeTest();
			break;
		case ePSNOS_Matchmaking_WaitingForPrivilegeDialog:
			TransitionMatchmakingWaitingForPrivilegeDialog();
			break;
		case ePSNOS_Matchmaking_StartContext:
			TransitionMatchmakingStartContext();
			break;
		case ePSNOS_Matchmaking_ContextReady:
			TransitionMatchmakingRegisterHandlers();
			break;
		case ePSNOS_Matchmaking_ReadyToEnumerateServers:
			TransitionMatchmakingEnumerateServers();
			break;
		case ePSNOS_Matchmaking_GettingNextServer:
			TransitionMatchmakingServerInfoDiscovery();
			break;
		case ePSNOS_Matchmaking_ServerInfoRetrieved:
			TransitionMatchmakingNextServer();
			break;
		case ePSNOS_Matchmaking_ServerListDone:
			TransitionMatchmakingCreateServerContext();
			break;
		case ePSNOS_Matchmaking_ServerJoined:
			TransitionMatchmakingAvailable();
			break;
		}
	}
}

bool CDrxPSNSupport::FlagPendingError(EDrxPSNPendingError errorCode, EDrxPSNOnlineState newState)
{
	SDrxPSNSupportCallbackEventData ecd;
	memset(&ecd, 0, sizeof(SDrxPSNSupportCallbackEventData));
	ecd.m_errorEvent.m_error = errorCode;
	ecd.m_errorEvent.m_oldState = m_curState;
	ecd.m_errorEvent.m_newState = newState;

	switch (newState)
	{
	default:
		{
			//-- no state change (and therefore no CleanTo!)
			NET_ASSERT(!"Shouldn't be switching to this state after an error!");
		}
		break;

	case ePSNOS_Matchmaking_Available:
		{
			assert(m_curState == ePSNOS_Matchmaking_Available);
			if (m_curState == ePSNOS_Matchmaking_Available)
			{
				//-- FlagPenderingError(ePSNOS_Matchmaking_Available) is a special case.
				//-- Send error out to all registered handlers, but no rollback of state!
				BroadcastEventToHandlers(eCE_ErrorEvent, ecd);

				NetLog("[PSN SUPPORT] State revert: -> ePSNOS_Matchmaking_Available (error no rollback)");

				return true;
			}
		}
		break;

	case ePSNOS_Offline:
		{
			assert(m_curState >= ePSNOS_Offline);
			if (m_curState >= ePSNOS_Offline)
			{
				BroadcastEventToHandlers(eCE_ErrorEvent, ecd);

				NetLog("[PSN SUPPORT] State revert: -> Rollback to ePSNOS_Offline");

				CleanTo(newState, errorCode);

				return true;
			}
		}
		break;

	case ePSNOS_Online:
		{
			assert(m_curState >= ePSNOS_Online);
			if (m_curState >= ePSNOS_Online)
			{
				BroadcastEventToHandlers(eCE_ErrorEvent, ecd);

				NetLog("[PSN SUPPORT] State revert: -> Rollback to ePSNOS_Online");

				CleanTo(newState, errorCode);

				return true;
			}
		}
		break;
	}

	return false;
}

bool CDrxPSNSupport::HandlePSNErrorInternal(int retCode, const char* const file, const int line)
{
	//-- Any PSN error (retCode < 0) not handled above will be treated as a generic worst-case error here.
	//-- But if a value not a PSN error was passed in, return false.
	if (retCode < 0)
	{
		NetLog("[PSN] Error code 0x%08X - HandlePSNError() trapped '%s' %d", retCode, file ? file : "NULL", line);

		switch (retCode)
		{
		case SCE_NET_CTL_ERROR_NOT_INITIALIZED:
			return FlagPendingError(ePSNPE_NetLibraryNotInitialised, ePSNOS_Offline);
		case SCE_NET_CTL_ERROR_INVALID_SIZE:
		case SCE_NET_CTL_ERROR_INVALID_TYPE:
		case SCE_NET_CTL_ERROR_NOT_AVAIL:
		case SCE_NET_CTL_ERROR_INVALID_ADDR:
		case SCE_NET_CTL_ERROR_INVALID_CODE:
		case SCE_NET_CTL_ERROR_INVALID_ID:
		case SCE_NET_CTL_ERROR_ID_NOT_FOUND:
			return FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
		case SCE_NET_CTL_ERROR_NOT_CONNECTED:
			return FlagPendingError(ePSNPE_NotConnected, ePSNOS_Offline);

		case SCE_NP_ERROR_INVALID_ARGUMENT:
			return FlagPendingError(ePSNPE_BadArgument, ePSNOS_Offline);
		case SCE_NP_ERROR_ALREADY_INITIALIZED:
			return FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);

		case SCE_NP_MATCHING2_ERROR_NOT_INITIALIZED:
			return FlagPendingError(ePSNPE_MatchingLibraryNotInitialised, ePSNOS_Offline);
		case SCE_NP_MATCHING2_ERROR_INVALID_ALIGNMENT:
			return FlagPendingError(ePSNPE_OutOfMemory, ePSNOS_Offline);
		case SCE_NP_MATCHING2_ERROR_INVALID_ARGUMENT:
			return FlagPendingError(ePSNPE_BadArgument, ePSNOS_Offline);
		case SCE_NP_MATCHING2_ERROR_CONTEXT_NOT_FOUND:
		case SCE_NP_MATCHING2_ERROR_INVALID_CONTEXT_ID:
			return FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		case SCE_NP_MATCHING2_ERROR_CONTEXT_ALREADY_STARTED:
			return FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		case SCE_NP_MATCHING2_ERROR_INVALID_SERVER_ID:
			return FlagPendingError(ePSNPE_InternalError, ePSNOS_Online);
		case SCE_NP_MATCHING2_ERROR_SERVER_NOT_AVAILABLE:
			return FlagPendingError(ePSNPE_NoServersAvailable, ePSNOS_Online);
		case SCE_NP_MATCHING2_SERVER_ERROR_ROOM_FULL:
			return false;
		case SCE_NP_MATCHING2_ERROR_ROOM_NOT_FOUND:
		case SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_ROOM:
		case SCE_NP_MATCHING2_SERVER_ERROR_NO_SUCH_ROOM_INSTANCE:
			return false;
		case SCE_NP_MATCHING2_SERVER_ERROR_SERVICE_UNAVAILABLE:
			return FlagPendingError(ePSNPE_PSNUnavailable, ePSNOS_Offline);
		case SCE_NP_MATCHING2_ERROR_ALREADY_JOINED:
			return false;

		case SCE_NP_SIGNALING_ERROR_NOT_INITIALIZED:
			return FlagPendingError(ePSNPE_SignalingLibraryNotInitialised, ePSNOS_Offline);
		case SCE_NP_SIGNALING_ERROR_ALREADY_INITIALIZED:
			return FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
		case SCE_NP_SIGNALING_ERROR_OUT_OF_MEMORY:
			return FlagPendingError(ePSNPE_OutOfMemory, ePSNOS_Offline);
		case SCE_NP_SIGNALING_ERROR_CTXID_NOT_AVAILABLE:
		case SCE_NP_SIGNALING_ERROR_CTX_MAX:
			return FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		case SCE_NP_SIGNALING_ERROR_INVALID_ARGUMENT:
			return FlagPendingError(ePSNPE_BadArgument, ePSNOS_Offline);
		case SCE_NP_SIGNALING_ERROR_CONN_NOT_FOUND: //-- not a fatal error if the connection isn't found.
		case SCE_NP_SIGNALING_ERROR_TIMEOUT:        //-- Not fatal if signaling times out.
			return false;

		case SCE_NP_COMMUNITY_ERROR_NOT_INITIALIZED:
			return FlagPendingError(ePSNPE_ScoreLibraryNotInitialised, ePSNOS_Offline);
		case SCE_NP_COMMUNITY_ERROR_OUT_OF_MEMORY:
			return FlagPendingError(ePSNPE_OutOfMemory, ePSNOS_Offline);
		case SCE_NP_COMMUNITY_ERROR_TOO_MANY_OBJECTS:
			return FlagPendingError(ePSNPE_ContextError, ePSNOS_Online);
		case SCE_NP_COMMUNITY_ERROR_INSUFFICIENT_ARGUMENT:
			return FlagPendingError(ePSNPE_BadArgument, ePSNOS_Offline);
		//		case SCE_NP_COMMUNITY_ERROR_TIMEOUT:
		case SCE_NP_COMMUNITY_ERROR_HTTP_SERVER:
			//		case SCE_NP_COMMUNITY_ERROR_BUSY_BY_ANOTEHR_TRANSACTION:
			return false;                                            //-- not fatal if we fail to communicate with leaderboard service.
		case SCE_NP_COMMUNITY_SERVER_ERROR_GAME_RANKING_NOT_FOUND: //-- not fatal if we can't find something on the leaderboards
			return false;
		case SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND: //- not fatal if we can't find TUS data. Probably means it hasn't been written yet for this user.
			return false;

		case SCE_NP_ERROR_AGE_RESTRICTION:
			return FlagPendingError(ePSNPE_AgeRestricted, ePSNOS_Offline);

		default:
			break;
		}

		return FlagPendingError(ePSNPE_InternalError, ePSNOS_Offline);
	}
	else
	{
		return false;
	}
}

bool CDrxPSNSupport::RegisterEventHandler(int eventTypes, DrxPSNSupportCallback pCB, void* pCBArg)
{
	if (m_nCallbacksRegistered < MAX_SUPPORT_EVENT_HANDLERS)
	{
		m_CallbackHandlers[m_nCallbacksRegistered].m_CallbackFunc = pCB;
		m_CallbackHandlers[m_nCallbacksRegistered].m_CallbackArg = pCBArg;
		m_CallbackHandlers[m_nCallbacksRegistered].m_CallbackEventTypes = eventTypes;
		m_nCallbacksRegistered++;

		return true;
	}

	return false;
}

void CDrxPSNSupport::BroadcastEventToHandlers(EDrxPSNSupportCallbackEvent eEvent, SDrxPSNSupportCallbackEventData& evd)
{
	for (uint32 i = 0; i < m_nCallbacksRegistered; i++)
	{
		if (m_CallbackHandlers[i].m_CallbackEventTypes & eEvent)
		{
			m_CallbackHandlers[i].m_CallbackFunc(eEvent, evd, m_CallbackHandlers[i].m_CallbackArg);
		}
	}
}

		#if USE_DRX_MATCHMAKING
/*
   void CDrxPSNSupport::PrepareForInviteOrJoin(SDrxPSNSessionID* pSessionId)
   {
   m_inviteInfo.m_serverId = pSessionId->m_sessionInfo.m_serverId;
   m_inviteInfo.m_worldId = pSessionId->m_sessionInfo.m_worldId;
   m_inviteInfo.m_bUseInvite = true;

   if (m_curState == ePSNOS_Matchmaking_Available)
   {
    if (m_serverBestInfo.server.serverId != pSessionId->m_sessionInfo.m_serverId)
    {
      FlagPendingError(ePSNPE_RestartForInvite, ePSNOS_Online);
    }
   }
   else
   {
    if (m_curState >= ePSNOS_Online)
    {
      FlagPendingError(ePSNPE_RestartForInvite, ePSNOS_Online);
    }
   }
   }
 */
		#endif // USE_DRX_MATCHMAKING

uint32 CDrxPSNSupport::RandomNumber(uint32 range) const
{
	return (uint32)g_time.GetMilliSeconds() % range;
}

		#if USE_PSN_VOICE
void CDrxPSNSupport::ChatEventCallbackHandler(CellSysutilAvc2EventId eventId, CellSysutilAvc2EventParam eventParam)
{
	CDrxPSNVoice* pVoice = (CDrxPSNVoice*)m_pLobby->GetVoice();
	if (pVoice)
	{
		pVoice->ChatEventCallbackHandler(eventId, eventParam);
	}
}
		#endif

	#endif // USE_PSN
#endif   // DRX_PLATFORM_ORBIS
