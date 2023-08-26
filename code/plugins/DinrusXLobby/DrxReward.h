// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#if !defined(__DRXREWARD_H__)
#define __DRXREWARD_H__

<DinrusX/CoreX/Lobby/IDrxReward.h>
#include "DrxLobby.h"

enum EDrxRewardStatus
{
	eCRS_Uninitialised = 0, // Reward system is uninitialised
	eCRS_Initialising,      // Reward system is currently being initialised
	eCRS_Idle,              // Reward system is idle.
	eCRS_Working,           // Reward system is working: currently unlocking some awards
};

class CDrxReward : public CMultiThreadRefCount, public IDrxReward
{
public:
	CDrxReward(CDrxLobby* pLobby);
	virtual ~CDrxReward(void);

	// IDrxReward
	virtual EDrxRewardError Award(uint32 playerID, uint32 awardID, DrxLobbyTaskID* pTaskID, DrxRewardCallback cb, void* pCbArg);

	virtual void            Tick(CTimeValue tv);

	virtual bool            IsDead() const { return false; }

protected:
	EDrxRewardStatus m_status;
	CDrxLobby*       m_pLobby;
};

#endif // End [!defined(__DRXREWARD_H__)]
// [EOF]
