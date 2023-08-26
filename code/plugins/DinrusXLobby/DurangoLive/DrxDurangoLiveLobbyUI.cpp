// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: CDrxLobbyUI implementation for Durango

   -------------------------------------------------------------------------
   History:
   - 13:06:2013 : Created by Yeonwoon JUNG

*************************************************************************/

#include "StdAfx.h"

#include "DrxDurangoLiveLobbyUI.h"
#include "DrxDurangoLiveLobby.h"

#if USE_DURANGOLIVE

	#include "platform.h"
	<DinrusX/CoreX/Platform/IPlatformOS.h>
	<DinrusX/CoreX/String/UnicodeFunctions.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Xbox::System;
using namespace ABI::Microsoft::Xbox::Services;

	#define RICHPRESENCE_PARAM_USERDATA     0   // mem
	#define RICHPRESENCE_PARAM_NUM_USERDATA 0   // num

CDrxDurangoLiveLobbyUI::CDrxDurangoLiveLobbyUI(CDrxLobby* pLobby, CDrxLobbyService* pService) : CDrxLobbyUI(pLobby, pService)
{
	// Make the CDrxLobbyUI base pointers point to our data so we can use the common code in CDrxLobbyUI
	for (uint32 i = 0; i < MAX_LOBBYUI_TASKS; i++)
	{
		CDrxLobbyUI::m_pTask[i] = &m_task[i];
	}

	// load rich presence enumeration from xml; this info is shared with application level.
	XmlNodeRef rpXML = GetISystem()->LoadXmlFromFile("Scripts/Network/RichPresence.xml");
	if (rpXML)
	{
		int numElements = rpXML->getChildCount();
		for (int i = 0; i < numElements; ++i)
		{
			XmlNodeRef childXml = rpXML->getChild(i);

			int id = -1;
			const char* value = NULL;

			if (childXml->getAttr("id", id) && childXml->getAttr("name", &value))
			{
				m_richPresence[id] = Unicode::Convert<wstring>(value);
			}
		}
	}
}

void CDrxDurangoLiveLobbyUI::Tick(CTimeValue tv)
{
}

EDrxLobbyError CDrxDurangoLiveLobbyUI::StartTask(ETask etask, bool startRunning, uint32 user, DrxLobbyUITaskID* pUITaskID, DrxLobbyTaskID* pLTaskID, DrxLobbySessionHandle h, void* pCb, void* pCbArg)
{
	DrxLobbyUITaskID tmpUITaskID;
	DrxLobbyUITaskID* pUseUITaskID = pUITaskID ? pUITaskID : &tmpUITaskID;
	EDrxLobbyError error = CDrxLobbyUI::StartTask(etask, startRunning, pUseUITaskID, pLTaskID, h, pCb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[*pUseUITaskID];
		pTask->user = user;
	}

	return error;
}

void CDrxDurangoLiveLobbyUI::StartTaskRunning(DrxLobbyUITaskID uiTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		pTask->running = true;

		switch (pTask->startedTask)
		{
		case eT_SetRichPresence:
			StartSetRichPresence(uiTaskID);
			break;
		}
	}
}

void CDrxDurangoLiveLobbyUI::EndTask(DrxLobbyUITaskID uiTaskID)
{
	LOBBY_AUTO_LOCK;

	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		if (pTask->pCb)
		{
			switch (pTask->startedTask)
			{
			case eT_SetRichPresence:
				((DrxLobbyUICallback)pTask->pCb)(pTask->lTaskID, pTask->error, pTask->pCbArg);
				break;
			}
		}

		if (pTask->error != eCLE_Success)
		{
			NetLog("[Lobby] LobbyUI EndTask %d (%d) Result %d", pTask->startedTask, pTask->subTask, pTask->error);
		}

		FreeTask(uiTaskID);
	}
}

void CDrxDurangoLiveLobbyUI::StopTaskRunning(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];

	if (pTask->used)
	{
		pTask->running = false;
		TO_GAME_FROM_LOBBY(&CDrxDurangoLiveLobbyUI::EndTask, this, uiTaskID);
	}
}

EDrxLobbyError CDrxDurangoLiveLobbyUI::SetRichPresence(uint32 user, SDrxLobbyUserData* pData, uint32 numData, DrxLobbyTaskID* pTaskID, DrxLobbyUICallback cb, void* pCbArg)
{
	LOBBY_AUTO_LOCK;
	DrxLobbyUITaskID uiTaskID;
	EDrxLobbyError error = StartTask(eT_SetRichPresence, false, user, &uiTaskID, pTaskID, DrxLobbyInvalidSessionHandle, cb, pCbArg);

	if (error == eCLE_Success)
	{
		STask* pTask = &m_task[uiTaskID];

		error = CreateTaskParamMem(uiTaskID, RICHPRESENCE_PARAM_USERDATA, pData, numData * sizeof(SDrxLobbyUserData));

		if (error == eCLE_Success)
		{
			pTask->paramsNum[RICHPRESENCE_PARAM_NUM_USERDATA] = numData;

			FROM_GAME_TO_LOBBY(&CDrxDurangoLiveLobbyUI::StartTaskRunning, this, uiTaskID);
		}
		else
		{
			FreeTask(uiTaskID);
		}
	}

	return error;
}

void CDrxDurangoLiveLobbyUI::StartSetRichPresence(DrxLobbyUITaskID uiTaskID)
{
	STask* pTask = &m_task[uiTaskID];
	const SDrxLobbyUserData* pData = (SDrxLobbyUserData*)m_pLobby->MemGetPtr(pTask->paramsMem[RICHPRESENCE_PARAM_USERDATA]);

	if (pTask->error == eCLE_Success)
	{
		ComPtr<IUser> user;

		ISystem* pSystem = GetISystem();
		if (pSystem)
		{
			int userIndex = pSystem->GetPlatformOS()->GetFirstSignedInUser();
			if (userIndex != IPlatformOS::Unknown_User)
			{
				int userId = pSystem->GetPlatformOS()->UserGetId(userIndex);
				HRESULT hr = Interface::Statics<IUser>()->GetUserById(userId, &user);
			}
		}
		boolean isSignedIn = false;
		boolean isGuest = true;
		if (user.Get())
		{
			user->get_IsSignedIn(&isSignedIn);
			user->get_IsGuest(&isGuest);
		}
		PREFAST_SUPPRESS_WARNING(6102)
		if (user.Get() && isSignedIn && !isGuest)
		{
			// first data should always indicate state data which contains friendly name for rich presence
			const SDrxLobbyUserData* pStateData = pData;

			assert(pStateData->m_type == eCLUDT_Int32);
			HString presenceId;
			presenceId.Set(m_richPresence[pStateData->m_int32]);

			ComPtr<IXboxLiveContext> liveContext;
			Activation::Factory<XboxLiveContext>()->CreateInstance1(user.Get(), &liveContext);

			ComPtr<Presence::IPresenceData> presenceData;
			{
				if (presenceId.IsValid() && WindowsGetStringLen(presenceId.Get()) != 0)
				{
					CDrxDurangoLiveLobbyService* pDurangoLiveLobbyService = (CDrxDurangoLiveLobbyService*)m_pService;

					HString scid;
					pDurangoLiveLobbyService->GetServiceConfigID(scid.GetAddressOf());

					Activation::Factory<Presence::PresenceData>()->CreateInstance2(scid.Get(), presenceId.Get(), &presenceData);
				}
			}

			// TODO: we may need to change this value when ERA is suspended by user request. -- 13/6/2013 yeonwoon
			bool userIsActive = true;
			ComPtr<Presence::IPresenceService> presenceService;
			HRESULT hr = liveContext->get_PresenceService(&presenceService);
			if (SUCCEEDED(hr) && presenceService.Get())
			{
				ComPtr<IAsyncAction> action;

				hr = presenceService->SetPresenceAsync1(userIsActive, presenceData.Get(), &action);
				if (SUCCEEDED(hr))
				{
					ABI::Concurrency::task_from_async(action).then(
					  [](HRESULT hr)
					{
						DrxLogAlways("CDrxDurangoLiveLobbyUI::StartSetRichPresence: SetPresenceAsync DONE: 0x%08X", hr);
					});
				}
				else
				{
					DrxLogAlways("CDrxDurangoLiveLobbyUI::StartSetRichPresence: SetPresenceAsync failed immediately: 0x%08X", hr);
				}
			}
			else
			{
				DrxLogAlways("CDrxDurangoLiveLobbyUI::StartSetRichPresence: get_PresenceService failed: 0x%08X", hr);
			}
		}
	}

	StopTaskRunning(uiTaskID);
}

#endif//USE_DURANGOLIVE
