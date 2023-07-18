// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 

	-------------------------------------------------------------------------
	History:
	- 03:09:2009  : Created by Colin Gulliver

*************************************************************************/

#ifndef _GameRulesStandardSetup_h_
#define _GameRulesStandardSetup_h_

#if _MSC_VER > 1000
# pragma once
#endif

#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesPlayerSetupModule.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesPickupListener.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesRoundsListener.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesTeamChangedListener.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesRevivedListener.h>
#include <DinrusX/DinrusXGame/GameRulesModules/IGameRulesClientConnectionListener.h>
#include <DinrusX/DinrusXGame/GameRulesTypes.h>

struct IInventory;

class CGameRulesStandardSetup : public IGameRulesPlayerSetupModule,
								public IGameRulesPickupListener,
								public IGameRulesRoundsListener,
								public IGameRulesTeamChangedListener,
								public IGameRulesRevivedListener,
								public IGameRulesClientConnectionListener
{
protected:
	typedef DrxFixedStringT<32> TFixedString;

public:
	CGameRulesStandardSetup();
	virtual ~CGameRulesStandardSetup();

	// IGameRulesPlayerSetupModule
	virtual void Init(XmlNodeRef xml);
	virtual void PostInit();

	virtual void OnClientConnect(int channelId, bool isReset, const char* playerName, bool isSpectator);
	virtual void OnPlayerRevived(EntityId playerId);
	virtual void SetupPlayerTeamSpecifics();
	virtual void SetupRemotePlayerTeamSpecifics(EntityId playerId);
	virtual void SetupAllRemotePlayerTeamSpecifics();

	virtual void SvOnStartNewRound(bool isReset);
	virtual void OnActorJoinedFromSpectate(IActor* pActor, int channelId);
	// ~IGameRulesPlayerSetupModule

	// IGameRulesPickupListener
	virtual void OnItemPickedUp(EntityId itemId, EntityId actorId);
	virtual void OnItemDropped(EntityId itemId, EntityId actorId);
	virtual void OnPickupEntityAttached(EntityId entityId, EntityId actorId) {};
	virtual void OnPickupEntityDetached(EntityId entityId, EntityId actorId, bool isOnRemove) {};
	// ~IGameRulesPickupListener

	// IGameRulesRoundsListener
	virtual void OnRoundStart() {}
	virtual void OnRoundEnd() {}
	virtual void OnSuddenDeath() {}
	virtual void ClRoundsNetSerializeReadState(int newState, int curState);
	virtual void OnRoundAboutToStart() {}
	// ~IGameRulesRoundsListener

	// IGameRulesTeamChangedListener
	virtual void OnChangedTeam(EntityId entityId, int oldTeamId, int newTeamId);
	// ~IGameRulesTeamChangedListener

	// IGameRulesRevivedListener
	virtual void EntityRevived(EntityId entityId);
	// ~IGameRulesRevivedListener

	// IGameRulesClientConnectionListener
	virtual void OnClientConnect(int channelId, bool isReset, EntityId playerId) {};
	virtual void OnClientDisconnect(int channelId, EntityId playerId) {};
	virtual void OnClientEnteredGame(int channelId, bool isReset, EntityId playerId) {};
	virtual void OnOwnClientEnteredGame();
	// ~IGameRulesClientConnectionListener

	void AssignActorToTeam(IActor* pActor, int channelId);

protected:
	void SetAmmoCapacity(IInventory *pInventory, const char *pAmmoClass, int amount);
	bool IsInIgnoreItemTypeList(const IItem* pItem) const;
	void CallLuaFunc(TFixedString* funcName);
	void CallLuaFunc1e(TFixedString* funcName, EntityId e);

	static const int MAX_IGNORE_REMOVE_ITEM_CLASSES = 5;
	const IEntityClass *m_itemRemoveIgnoreClasses[MAX_IGNORE_REMOVE_ITEM_CLASSES];

	TFixedString  m_luaSetupPlayerTeamSpecificsFunc;
	TFixedString  m_luaSetupRemotePlayerTeamSpecificsFunc;
	TFixedString  m_luaResetPlayerTeamSpecificsFunc;
	TFixedString  m_luaEquipTeamSpecificsFunc;

	CGameRules*  m_pGameRules;

	int m_numIgnoreItems;

	bool m_usesTeamSpecifics;
};

#endif // _GameRulesStandardSetup_h_