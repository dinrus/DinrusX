// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Binding of network functions into script

   -------------------------------------------------------------------------
   History:
   - 24:11:2004   11:30 : Created by Craig Tiller

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/ScriptBind_Network.h>
#include <DinrusX/DinrusXAct/GameContext.h>
#include <DinrusX/DinrusXAct/ScriptSerialize.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>
#include <DinrusX/DinrusXAct/GameChannel.h>
#include <DinrusX/DinrusXAct/GameClientNub.h>
#include <DinrusX/DinrusXAct/GameClientChannel.h>
#include <DinrusX/DinrusXAct/GameServerNub.h>
#include <DinrusX/DinrusXAct/GameServerChannel.h>
#include <DinrusX/DinrusXAct/GameServerNub.h>

#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXEntity/IEntityClass.h>
#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXPhys/IPhysics.h>

//------------------------------------------------------------------------
CScriptBind_Network::CScriptBind_Network(ISystem* pSystem, CDrxAction* pFW)
{
	m_pSystem = pSystem;
	m_pFW = pFW;

	Init(gEnv->pScriptSystem, m_pSystem);
	SetGlobalName("Net");

	RegisterGlobals();
	RegisterMethods();
}

//------------------------------------------------------------------------
CScriptBind_Network::~CScriptBind_Network()
{
}

//------------------------------------------------------------------------
void CScriptBind_Network::RegisterGlobals()
{
	gEnv->pScriptSystem->SetGlobalValue("UNRELIABLE_ORDERED", eNRT_UnreliableOrdered);
	gEnv->pScriptSystem->SetGlobalValue("RELIABLE_ORDERED", eNRT_ReliableOrdered);
	gEnv->pScriptSystem->SetGlobalValue("RELIABLE_UNORDERED", eNRT_ReliableUnordered);

	gEnv->pScriptSystem->SetGlobalValue("NO_ATTACH", eRAT_NoAttach);
	gEnv->pScriptSystem->SetGlobalValue("PRE_ATTACH", eRAT_PreAttach);
	gEnv->pScriptSystem->SetGlobalValue("POST_ATTACH", eRAT_PostAttach);

	gEnv->pScriptSystem->SetGlobalValue("BOOL", eSST_Bool);
	gEnv->pScriptSystem->SetGlobalValue("FLOAT", eSST_Float);
	gEnv->pScriptSystem->SetGlobalValue("INT8", eSST_Int8);
	gEnv->pScriptSystem->SetGlobalValue("INT16", eSST_Int16);
	gEnv->pScriptSystem->SetGlobalValue("INT32", eSST_Int32);
	gEnv->pScriptSystem->SetGlobalValue("STRING", eSST_String);
	gEnv->pScriptSystem->SetGlobalValue("STRINGTABLE", eSST_StringTable);
	gEnv->pScriptSystem->SetGlobalValue("ENTITYID", eSST_EntityId);
	gEnv->pScriptSystem->SetGlobalValue("VEC3", eSST_Vec3);
}

//------------------------------------------------------------------------
void CScriptBind_Network::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_Network::

	SCRIPT_REG_FUNC(Expose);
	SCRIPT_REG_TEMPLFUNC(DelegateAuthority, "entity, channel");
}

//------------------------------------------------------------------------
int CScriptBind_Network::Expose(IFunctionHandler* pFH)
{
	return m_pFW->NetworkExposeClass(pFH);
}

//------------------------------------------------------------------------
int CScriptBind_Network::DelegateAuthority(IFunctionHandler* pFH, ScriptHandle ent, int channel)
{
	CGameContext* pCtx = m_pFW->GetGameContext();
	if (!pCtx)
	{
		pFH->GetIScriptSystem()->RaiseError("No game started");
		return pFH->EndFunction();
	}
	INetContext* pNetContext = pCtx->GetNetContext();
	CGameServerNub* pServerNub = m_pFW->GetGameServerNub();
	if (!pServerNub)
	{
		pFH->GetIScriptSystem()->RaiseError("Not a server");
		return pFH->EndFunction();
	}
	INetChannel* pChannel = NULL;
	CGameChannel* pGameChannel = NULL;
	if (channel)
	{
		if (!(pGameChannel = pServerNub->GetChannel(channel)))
			GameWarning("No such server channel %d", channel);
		else
			pChannel = pGameChannel->GetNetChannel();
	}

	pNetContext->DelegateAuthority((EntityId)ent.n, pChannel);

	if ((pFH->GetParamCount() > 2) && (pFH->GetParamType(3) == svtFunction))
	{
		HSCRIPTFUNCTION func;

		if (pFH->GetParam(3, func))
		{
			pCtx->AddControlObjectCallback((EntityId)ent.n, !channel, func);
		}
	}

	return pFH->EndFunction();
}
