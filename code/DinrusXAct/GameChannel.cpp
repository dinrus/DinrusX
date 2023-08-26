// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Common functionality for CGameServerChannel, CGameClientChannel

   -------------------------------------------------------------------------
   History:
   - 6:10:2004   11:38 : Created by Craig Tiller

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/GameChannel.h>
#include <DinrusX/DinrusXAct/PhysicsSync.h>
#include <DinrusX/DinrusXAct/GameObject.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>
#include <DinrusX/DinrusXAct/GameContext.h>

CGameChannel::CGameChannel() : m_pNetChannel(NULL), m_pContext(NULL), m_playerId(0)
{
	m_pPhysicsSync.reset(new CPhysicsSync(this));
}

CGameChannel::~CGameChannel()
{
}

void CGameChannel::Init(INetChannel* pNetChannel, CGameContext* pGameContext)
{
	m_pNetChannel = pNetChannel;
	m_pContext = pGameContext;
}

void CGameChannel::ConfigureContext(bool isLocal)
{
	if (isLocal)
		return;

	GameWarning("Need to set physics time here");
}

void CGameChannel::SetPlayerId(EntityId id)
{
	m_playerId = id;
	if (id)
		m_pNetChannel->DeclareWitness(id);
}
