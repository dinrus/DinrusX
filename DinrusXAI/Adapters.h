// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// --------------------------------------------------------------------------------------
//  File name:   Adapters.h
//  Created:     02/02/2009 by Matthew
//  Описание: Реализует адаптеры для объектов ИИ из внешних интерфейсов во внутренние.
//               Это чисто "переводной" слой без конкретных экземпляров.
//               У них может отсутствовать состояние и они должны оставаться абстрактными.
//
//               * Помните, что это быстро становится redundant ("вторящим уже выраженное")!
//               В основном теперь уже играет роль оболочки... *
//
// --------------------------------------------------------------------------------------
//  History:
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef __AI_ADAPTERS_H_
#define __AI_ADAPTERS_H_

#pragma once

#include <DinrusX/DinrusXAI/IAgent.h>
#include <DinrusX/DinrusXAI/AIObject.h>
#include <DinrusX/DinrusXAI/ObjectContainer.h>

#include <DinrusX/DinrusXAI/IAIGroup.h>

CWeakRef<CAIObject> GetWeakRefSafe(IAIObject* pObj);

class CPipeUserAdapter : public IPipeUser
//Класс АдаптерПользователяПайпа, наследует интерфейс ИПользовательПайпа
{
public:
	virtual ~CPipeUserAdapter() {}

	virtual bool       SelectPipe(int id, const char* name, CWeakRef<CAIObject> refArgument, int goalPipeId = 0, bool resetAlways = false, const GoalParams* node = 0) = 0;
	virtual IGoalPipe* InsertSubPipe(int id, const char* name, CWeakRef<CAIObject> refArgument = NILREF, int goalPipeId = 0, const GoalParams* node = 0) = 0;

private:
	bool       SelectPipe(int id, const char* name, IAIObject* pArgument = 0, int goalPipeId = 0, bool resetAlways = false, const GoalParams* node = 0)
	{ return SelectPipe(id, name, GetWeakRefSafe(pArgument), goalPipeId, resetAlways, node); }
	IGoalPipe* InsertSubPipe(int id, const char* name, IAIObject* pArgument = 0, int goalPipeId = 0, const GoalParams* node = 0)
	{ return InsertSubPipe(id, name, GetWeakRefSafe(pArgument), goalPipeId, node); }

};

class CAIGroupAdapter
	: public IAIGroup
{
public:
	// cppcheck-suppress passedByValue
	virtual CWeakRef<CAIObject> GetAttentionTarget(bool bHostileOnly = false, bool bLiveOnly = false, const CWeakRef<CAIObject> refSkipTarget = NILREF) const = 0;

private:
	IAIObject* GetAttentionTarget(bool bHostileOnly = false, bool bLiveOnly = false) const;
};

#endif //__AI_ADAPTERS_H_
