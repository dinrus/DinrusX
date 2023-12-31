// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

//#pragma optimize("", off)

#if DRX_PLATFORM_ORBIS

	#include "DrxPSN2Lobby.h"

	#if USE_PSN

		#include "DrxPSN2Support.h"
		#include "DrxPSN2WebApi.h"
		#include "DrxPSN2WebApi_FriendsList.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// GetFriendsList
// Worker function that runs on the WebApi thread to request a friends list
void CDrxPSNOrbisWebApiThread::GetFriendsList(DrxWebApiJobId jobId, TMemHdl paramsHdl, CDrxPSNWebApiJobController* pController, CDrxPSNOrbisWebApiThread* _this)
{
	int ret = PSN_OK;

	if (paramsHdl == TMemInvalidHdl)
	{
		// no params! Error!
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	SDrxPSNOrbisWebApiGetFriendsListInput* pFriendsListData = (SDrxPSNOrbisWebApiGetFriendsListInput*)pController->GetUtility()->GetLobby()->MemGetPtr(paramsHdl);
	if (!pFriendsListData)
	{
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_INVALID_ARGUMENT, TMemInvalidHdl);
		return;
	}

	uint32 nOffset = pFriendsListData->startIndex;
	uint32 nCounter = pFriendsListData->count;
	uint32 nLimit = MIN(DRX_WEBAPI_FRIENDSLIST_PAGE_LIMIT, nCounter);

	char pathStr[DRX_WEBAPI_TEMP_PATH_SIZE];
	drx_sprintf(pathStr, DRX_WEBAPI_FRIENDSLIST_REQUEST_PATH, pFriendsListData->id.data, nLimit, nOffset);

	ret = _this->SendRequest(DRX_WEBAPI_FRIENDSLIST_HTTP_METHOD, DRX_WEBAPI_FRIENDSLIST_CONTENT_TYPE, DRX_WEBAPI_FRIENDSLIST_API_GROUP, pathStr);
	if (ret < PSN_OK)
	{
		// failed
		pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
		return;
	}

	TMemHdl responseBodyHdl = pController->GetUtility()->NewResponseBody("friendslist");
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

	// How many friends do we have?
	uint32 nTotalFriends = 0;
	uint32 nReturnedFriends = 0;
	const sce::Json::Value& numTotalFriends = pResponseBody->jsonTreeRoot["totalResults"];
	if (numTotalFriends.getType() == sce::Json::kValueTypeUInteger)
	{
		nTotalFriends = numTotalFriends.getUInteger();
	}
	const sce::Json::Value& numReturnedFriends = pResponseBody->jsonTreeRoot["size"];
	if (numReturnedFriends.getType() == sce::Json::kValueTypeUInteger)
	{
		nReturnedFriends = numReturnedFriends.getUInteger();
	}

	pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, PSN_OK, responseBodyHdl);

	if (nTotalFriends <= nLimit)
	{
		// got all the friends with the first call
		// we're finished
	}
	else if (nReturnedFriends <= nLimit)
	{
		// got all the friends in the range we requested with one call.
		// we're finished
	}
	else
	{
		// still more friends to get
		// More than nLimit friends were requested, so we have to start paging them in nLimit at a time.
		while ((nCounter > 0) && (nReturnedFriends > 0) && (nOffset < nTotalFriends))
		{
			nOffset += nReturnedFriends;
			nCounter -= nReturnedFriends;
			nLimit = MIN(DRX_WEBAPI_FRIENDSLIST_PAGE_LIMIT, nCounter);

			drx_sprintf(pathStr, DRX_WEBAPI_FRIENDSLIST_REQUEST_PATH, pFriendsListData->id.data, nLimit, nOffset);

			ret = _this->SendRequest(DRX_WEBAPI_FRIENDSLIST_HTTP_METHOD, DRX_WEBAPI_FRIENDSLIST_CONTENT_TYPE, DRX_WEBAPI_FRIENDSLIST_API_GROUP, pathStr);
			if (ret < PSN_OK)
			{
				// failed
				pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, TMemInvalidHdl);
				break;
			}

			responseBodyHdl = pController->GetUtility()->NewResponseBody("friendslist nextpage");
			if (responseBodyHdl == TMemInvalidHdl)
			{
				pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_OUT_OF_MEMORY, TMemInvalidHdl);
				break;
			}

			pResponseBody = (SDrxPSNWebApiResponseBody*)pController->GetUtility()->GetLobby()->MemGetPtr(responseBodyHdl);
			if (!pResponseBody)
			{
				pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, SCE_NP_ERROR_OUT_OF_MEMORY, responseBodyHdl);
				break;
			}

			ret = _this->ReadResponse(pResponseBody);
			if (ret < PSN_OK)
			{
				// failed
				pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, ret, responseBodyHdl);
				break;
			}

			const sce::Json::Value& numReturnedFriendsLoop = pResponseBody->jsonTreeRoot["size"];
			if (numReturnedFriendsLoop.getType() == sce::Json::kValueTypeObject)
			{
				nReturnedFriends = numReturnedFriendsLoop.getUInteger();
			}

			pController->GetUtility()->GetPSNSupport()->ReturnWebApiEvent(jobId, PSN_OK, responseBodyHdl);
		}
	}

	// all complete!
}

//////////////////////////////////////////////////////////////////////////////////////////////

	#endif //USE_PSN
#endif   // DRX_PLATFORM_ORBIS
