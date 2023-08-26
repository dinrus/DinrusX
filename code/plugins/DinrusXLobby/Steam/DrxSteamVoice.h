// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXSTEAMVOICE_H__
#define __DRXSTEAMVOICE_H__

#ifdef USE_FMOD
	#define USE_STEAM_VOICE 1
#else
	#define USE_STEAM_VOICE 0
#endif // USE_FMOD

#pragma once

#if USE_DRX_VOICE && USE_STEAM && USE_STEAM_VOICE

	#include "DrxVoice.h"
	#include "IAudioDevice.h"

	#define MAX_STEAM_LOCAL_USERS    1
	#define MAX_STEAM_REMOTE_TALKERS 16

class ISoundPlayer;

class CDrxSteamVoice : public CDrxVoice
{
public:
	CDrxSteamVoice(CDrxLobby* pLobby, CDrxLobbyService* pService);
	virtual ~CDrxSteamVoice();
	void                 OnPacket(const TNetAddress& addr, CDrxLobbyPacket* pPacket);
	EDrxLobbyError       Initialise();
	EDrxLobbyError       Terminate();
	virtual bool         IsSpeaking(uint32 localUser)            { return false; }
	virtual bool         IsMicrophoneConnected(DrxUserID userID) { return false; }

	virtual void         Mute(uint32 localUser, DrxUserID remoteUser, bool mute);
	virtual void         MuteLocalPlayer(uint32 localUser, bool mute);
	void                 Tick(CTimeValue tv);

	DrxVoiceRemoteUserID RegisterRemoteUser(DrxLobbyConnectionID connectionID, DrxUserID remoteUserID) { return CDrxVoice::RegisterRemoteUser(connectionID, remoteUserID); }
protected:

	virtual void SendMicrophoneNotification(uint32 remoteUserID) {};
	virtual void SendMicrophoneRequest(uint32 remoteUserID)      {};

private:
	ISoundPlayer*     m_pSoundPlayer;
	SRemoteUser       m_SteamRemoteUsers[MAX_STEAM_REMOTE_TALKERS];
	SUserRelationship m_userRelationship[MAX_STEAM_LOCAL_USERS][MAX_STEAM_REMOTE_TALKERS];
};

#endif // USE_DRX_VOICE
#endif // __DRXSTEAMVOICE_H__
