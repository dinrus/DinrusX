// XXX - ATG

//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#ifndef __DRXDURANGOCHAT_H__
#define __DRXDURANGOCHAT_H__

#pragma once

#if DRX_PLATFORM_DURANGO && USE_DURANGOLIVE

	<DinrusX/CoreX/Lobby/IDrxLobby.h>

	#include "../DrxDurangoLiveMatchMaking.h"
	#include "../DrxDurangoLiveLobbyPacket.h"

class ChatIntegrationLayer
{
public:
	ChatIntegrationLayer();
	~ChatIntegrationLayer();

	EDrxLobbyError Initialize(bool jitterBufferEnabled, CDrxDurangoLiveMatchMaking* pMatchMaking, DWORD cpuCore = 5);
	void           Shutdown();
	void           Reset();

	void           AddRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID connectionID, uint32 consoleId);
	void           RemoveRemoteConnection(DrxLobbySessionHandle h, DrxMatchMakingConnectionID connectionID, uint32 consoleID);

	void           AddLocalUserToChatChannel(
	  __in uint8 channelIndex,
	  __in Windows::Xbox::System::IUser^ user
	  );

	void RemoveUserFromChatChannel(
	  __in uint8 channelIndex,
	  __in Windows::Xbox::System::IUser^ user
	  );

	void RemoveRemoteConsole(
	  Platform::Object^ uniqueRemoteConsoleIdentifier
	  );

	void ReceivePacket(CDrxDurangoLiveLobbyPacket* pPacket);

	void OnIncomingChatMessage(
	  Windows::Storage::Streams::IBuffer^ chatPacket,
	  Platform::Object^ uniqueRemoteConsoleIdentifier
	  );

	Windows::Foundation::Collections::IVectorView<Microsoft::Xbox::GameChat::ChatUser^>^ GetChatUsers();
	bool                                                                                 HasMicFocus();
	void                                                                                 HandleChatUserMuteStateChanged(
	  __in INT chatUserIndex
	  );

	void HandleChatChannelChanged(
	  __in uint8 oldChatChannelIndex,
	  __in uint8 newChatChannelIndex,
	  __in INT index
	  );
	void OnSecureDeviceAssocationConnectionEstablished(__in Platform::Object^ uniqueConsoleIdentifier);

private:
	void AddAllLocallySignedInUsersToChatClient(
	  __in uint8 channelIndex,
	  __in Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ locallySignedInUsers
	  );
	void RegisterChatUprEventHandlers();

	void OnDebugMessageReceived(
	  __in Microsoft::Xbox::GameChat::DebugMessageEventArgs^ args
	  );

	void OnOutgoingChatPacketReady(
	  __in Microsoft::Xbox::GameChat::ChatPacketEventArgs^ args
	  );

	static inline Platform::Object^      Uint32ToPlatformObject(__in uint32 val)            { return Windows::Foundation::PropertyValue::CreateUInt32(val); }
	static inline uint32                 PlatformObjectToUint32(__in Platform::Object^ obj) { return safe_cast<Windows::Foundation::IPropertyValue^>(obj)->GetUInt32(); }

	Microsoft::Xbox::GameChat::ChatUser^ GetChatUserByXboxUserId(
	  __in Platform::String^ xboxUserId
	  );

	inline bool ChatIntegrationLayer::CompareUniqueConsoleIdentifiers(
	  __in Platform::Object^ uniqueRemoteConsoleIdentifier1,
	  __in Platform::Object^ uniqueRemoteConsoleIdentifier2
	  )
	{
		if (uniqueRemoteConsoleIdentifier1 == nullptr || uniqueRemoteConsoleIdentifier2 == nullptr)
		{
			return false;
		}

		return (PlatformObjectToUint32(uniqueRemoteConsoleIdentifier1) == PlatformObjectToUint32(uniqueRemoteConsoleIdentifier2));
	}

	void OnUserAudioDeviceAdded(Windows::Xbox::System::AudioDeviceAddedEventArgs^ eventArgs);

	Concurrency::critical_section               m_lock;
	Microsoft::Xbox::GameChat::ChatUpr^     m_chatUpr;
	Windows::Foundation::EventRegistrationToken m_tokenOnDebugMessage;
	Windows::Foundation::EventRegistrationToken m_tokenOnOutgoingChatPacketReady;
	Windows::Foundation::EventRegistrationToken m_tokenOnCompareUniqueConsoleIdentifiers;
	Windows::Foundation::EventRegistrationToken m_tokenResourceAvailabilityChanged;
	Windows::Foundation::EventRegistrationToken m_tokenAudioDeviceAdded;
	float m_ackTimer;
	CDrxDurangoLiveMatchMaking*                 m_pMatchMaking;

	struct ConnectionData
	{
		DrxLobbySessionHandle      sessionHandle;
		DrxMatchMakingConnectionID connectionID;
		uint32                     remoteConsoleId;
	};
	std::vector<ConnectionData> m_remoteConsoleConnections;

	void CreatePacket(_Inout_ CDrxDurangoLiveLobbyPacket* pPacket, _In_ Windows::Storage::Streams::IBuffer^ pBuffer, _Inbool reliable);
	void UnwrapPacket(_In_ CDrxDurangoLiveLobbyPacket* pPacket, _Out_ byte** ppData, _Out_ uint* size);

};

static ChatIntegrationLayer* GetChatIntegrationLayer()
{
	static ChatIntegrationLayer* pChatIntegrationLayerInstance;
	if (pChatIntegrationLayerInstance == nullptr)
	{
		pChatIntegrationLayerInstance = new ChatIntegrationLayer();
	}

	return pChatIntegrationLayerInstance;
}

static void ShutdownChatIntegrationLayer()
{
	delete GetChatIntegrationLayer();
}

#endif
#endif  // __DRXDURANGOCHAT_H__
