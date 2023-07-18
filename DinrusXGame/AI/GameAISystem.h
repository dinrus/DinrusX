// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#ifndef GameAISystem_h
#define GameAISystem_h

#include <DinrusX/DinrusXGame/IGameAIModule.h>
#include <DinrusX/DinrusXGame/AdvantagePointOccupancyControl.h>
#include <DinrusX/DinrusXGame/DeathUpr.h>
#include <DinrusX/DinrusXGame/VisibleObjectsHelper.h>
#include <DinrusX/DinrusXGame/TargetTrackThreatModifier.h>
#include <DinrusX/DinrusXGame/GameAIRecorder.h>
#include <DinrusX/DinrusXGame/AIAwarenessToPlayerHelper.h>
#include <DinrusX/DinrusXGame/AICounters.h>
#include <DinrusX/DinrusXGame/AISquadUpr.h>
#include <DinrusX/DinrusXGame/EnvironmentDisturbanceUpr.h>
#include <DinrusX/DinrusXGame/AICorpse.h>

class CGameAISystem
{
public:
	CGameAISystem();
	~CGameAISystem();
	IGameAIModule* FindModule(const char* moduleName) const;
	void EnterModule(EntityId entityID, const char* moduleName);
	void LeaveModule(EntityId entityID, const char* moduleName);
	void LeaveAllModules(EntityId entityID);
	void PauseModule(EntityId entityID, const char* moduleName);
	void PauseAllModules(EntityId entityID);
	void ResumeModule(EntityId entityID, const char* moduleName);
	void ResumeAllModules(EntityId entityID);
	void Update(float frameTime);
	void Reset(bool bUnload);
	void Serialize(TSerialize ser);
	void PostSerialize();

	CAdvantagePointOccupancyControl& GetAdvantagePointOccupancyControl() { return m_advantagePointOccupancyControl; }
	GameAI::DeathUpr* GetDeathUpr() { return m_pDeathUpr; }
	CVisibleObjectsHelper& GetVisibleObjectsHelper() { return m_visibleObjectsHelper; }
	CAIAwarenessToPlayerHelper& GetAIAwarenessToPlayerHelper() { return m_AIAwarenessToPlayerHelper; }
	CAICounters& GetAICounters() { return m_AICounters; }
	AISquadUpr& GetAISquadUpr() { return m_AISquadUpr; }
	GameAI::EnvironmentDisturbanceUpr& GetEnvironmentDisturbanceUpr() { return m_environmentDisturbanceUpr; }

#ifdef INCLUDE_GAME_AI_RECORDER
	CGameAIRecorder &GetGameAIRecorder() { return m_gameAIRecorder; }
	const CGameAIRecorder &GetGameAIRecorder() const { return m_gameAIRecorder; }
#endif //INCLUDE_GAME_AI_RECORDER

	enum State
	{
		Idle,
		UpdatingModules
	};

private:
	void UpdateModules(float frameTime);
	void UpdateSubSystems(float frameTime);
	void ResetModules(bool bUnload);
	void ResetSubSystems(bool bUnload);
	void Error(const char* functionName) const;
	void InformContentCreatorOfError(string logMessage) const;
	void GetCallStack(string& callstack) const;
	typedef std::vector<IGameAIModule*> Modules;
	Modules m_modules;

	CAdvantagePointOccupancyControl m_advantagePointOccupancyControl;
	GameAI::DeathUpr* m_pDeathUpr;
	CVisibleObjectsHelper m_visibleObjectsHelper;
	CAIAwarenessToPlayerHelper m_AIAwarenessToPlayerHelper;
	CAICounters m_AICounters;
	CTargetTrackThreatModifier m_targetTrackThreatModifier;
	AISquadUpr m_AISquadUpr;
	GameAI::EnvironmentDisturbanceUpr m_environmentDisturbanceUpr;
	State m_state;

	CAICorpseUpr* m_pCorpsesUpr;

#ifdef INCLUDE_GAME_AI_RECORDER
	CGameAIRecorder m_gameAIRecorder;
#endif //INCLUDE_GAME_AI_RECORDER
};

#endif // GameAISystem_h
