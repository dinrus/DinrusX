// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __RICHPRESENCE_H__
#define __RICHPRESENCE_H__

#include <DinrusX/DinrusXGame/GameMechanismBase.h>

class CRichPresence : public CGameMechanismBase
{
	public:

		enum ERichPresenceState
		{
			eRPS_None,
			eRPS_Idle,
			eRPS_FrontEnd,
			eRPS_Lobby,
			eRPS_InGame,
		};

		enum ERichPresenceType
		{
			eRPT_String = 0,
			eRPT_Param1,
			eRPT_Param2,
			eRPT_Max,
		};

	public:

		CRichPresence();
		virtual ~CRichPresence();

		// CGameMechanismBase
		virtual void Update(float dt);
		// ~CGameMechanismBase

		void LoadXmlFromFile(const char* path);
		bool SetRichPresence(ERichPresenceState state);
		DrxSessionID GetPendingRichPresenceSessionID() const { return m_pendingSessionID; }
	
	private:

		void OnSetRichPresenceCallback(DrxLobbyTaskID taskID, EDrxLobbyError error);

		static void SetRichPresenceCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);

	private:

		typedef std::map<DrxFixedStringT<128>, int> TRichPresenceMap;
		TRichPresenceMap m_richPresence;
		
		ERichPresenceState m_currentState;
		ERichPresenceState m_pendingState;
		ERichPresenceState m_desiredState;

		DrxLobbyTaskID m_taskID;

		DrxSessionID m_currentSessionID;
		DrxSessionID m_pendingSessionID;

		float m_updateTimer;
		bool m_refresh;
};

#endif // __RICHPRESENCE_H__
