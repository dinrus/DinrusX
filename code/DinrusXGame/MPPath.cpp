// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/MPPath.h>

#include <DinrusX/DinrusXGame/GameRules.h>
#include <DinrusX/DinrusXGame/MPPathFollowingUpr.h>

bool CMPPath::Init( IGameObject *pGameObject )
{
	SetGameObject(pGameObject);

	return true;
}

//------------------------------------------------------------------------
void CMPPath::Release()
{
	CMPPathFollowingUpr* pPathFollowingUpr = g_pGame->GetGameRules() ? g_pGame->GetGameRules()->GetMPPathFollowingUpr() : NULL;
	if(pPathFollowingUpr)
	{
		pPathFollowingUpr->UnregisterPath(GetEntityId());
	}
	delete this;
}

//------------------------------------------------------------------------
void CMPPath::GetMemoryUsage(IDrxSizer *pSizer) const
{
	pSizer->Add(*this);
}

//------------------------------------------------------------------------
bool CMPPath::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();
	DRX_ASSERT(!"CMPPath::ReloadExtension not implemented");
	return false;
}

//------------------------------------------------------------------------
bool CMPPath::GetEntityPoolSignature( TSerialize signature )
{
	DRX_ASSERT(!"CMPPath::GetEntityPoolSignature not implemented");
	return true;
}

void CMPPath::ProcessEvent( SEntityEvent& details )
{
	if(details.event == ENTITY_EVENT_LEVEL_LOADED)
	{
		CMPPathFollowingUpr* pPathFollowingUpr = g_pGame->GetGameRules()->GetMPPathFollowingUpr();
		if(pPathFollowingUpr)
		{
			pPathFollowingUpr->RegisterPath(GetEntityId());
		}
	}
}
