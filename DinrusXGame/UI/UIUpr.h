// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   UIUpr.h
//  Version:     v1.00
//  Created:     08/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIUpr_H__
#define __UIUpr_H__

#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/CoreX/Game/IGameFramework.h>
#include <DinrusX/DinrusXGame/IUIGameEventSystem.h>
#include <DinrusX/DinrusXGame/UICVars.h>
#include <DinrusX/DinrusXGame/UITypes.h>

class CWarningsUpr;
class CProfileOptions;
class C2DRenderUtils;
class ScreenLayoutUpr;
class CHUDSilhouettes;


class CUIUpr
	: public ISystemEventListener
	, public IGameFrameworkListener
{
public:
	CUIUpr();
	~CUIUpr();

	// lifetime
	void Init();
	void Shutdown();
	void PostSerialize();

	// subsystems
	IUIGameEventSystem* GetUIEventSystem(const char* typeName) const;
	CWarningsUpr* GetWarningUpr() const { return m_pWarningUpr; }
	CProfileOptions* GetOptions() const { return m_pOptions; }
	C2DRenderUtils* Get2DRenderUtils() const { return m_p2DRendUtils; }
	ScreenLayoutUpr* GetLayoutUpr() const { return m_pScreenLayoutMan; }
	CHUDSilhouettes* GetSilhouettes() const { return m_pHudSilhouettes; }
	CUICVars* GetCVars() const { return m_pCVars; }
	CHUDMissionObjectiveSystem* GetMOSystem() const { return m_pMOSystem; }

	// updated by PlayerView
	void ProcessViewParams(const SViewParams &viewParams);

	// states
	void ActivateState(const char* state);
	void ActivateStateImmediate(const char* state);
	void ActivateDefaultState();
	void ActivateDefaultStateImmediate();

	// menu state
	bool IsLoading();
	bool IsInMenu();
	bool IsPreGameDone();
	void ForceCompletePreGame();

	// mp game type
	void InitGameType(bool multiplayer, bool fromInit);

	// Control scheme related
	bool RegisterControlSchemeListener(IUIControlSchemeListener* pListener);
	bool UnregisterControlSchemeListener(IUIControlSchemeListener* pListener);
	void ClearControlSchemeListeners();
	void SetDefaultControlScheme();
	void SetCurControlScheme( const EControlScheme controlScheme );
	EControlScheme GetCurControlScheme() const { return m_curControlScheme; }

	// ISystemEventListener
	virtual void OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam );
	// ~ISystemEventListener

	// IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime);
	virtual void OnSaveGame(ISaveGame* pSaveGame) {}
	virtual void OnLoadGame(ILoadGame* pLoadGame) {}
	virtual void OnLevelEnd(const char* nextLevel) {}
	virtual void OnActionEvent(const SActionEvent& event) {}
	virtual void OnPreRender() {};
	// ~IGameFrameworkListener

	void GetMemoryUsage( IDrxSizer *pSizer ) const;

private:
	void InitSound();
	void ShutdownSound();
private:
	EntityId m_soundListener;
	bool m_bRegistered;

	CWarningsUpr* m_pWarningUpr;
	CProfileOptions* m_pOptions;
	C2DRenderUtils* m_p2DRendUtils;
	ScreenLayoutUpr* m_pScreenLayoutMan;
	CHUDSilhouettes* m_pHudSilhouettes;
	CUICVars* m_pCVars;
	CHUDMissionObjectiveSystem *m_pMOSystem;

	EControlScheme m_curControlScheme;
	TUIControlSchemeListeners m_controlSchemeListeners;

	typedef std::map<string, TUIEventSystemPtr> TUIEventSystems;
	TUIEventSystems m_EventSystems;
};

namespace UIEvents
{
	template <class T>
	T* Get()
	{
		if(g_pGame->GetUI())
			return static_cast<T*>(g_pGame->GetUI()->GetUIEventSystem(T::GetTypeNameS()));
		return NULL;
	}
}

#if FRONTEND_ENABLE_EXTRA_DEBUG
#define FE_LOG( ... )                                    \
{                                                        \
	string temp;                                            \
	temp.Format(__VA_ARGS__);                               \
	DrxLogAlways( "[UI] %s", temp.c_str() );                \
}
#else
#define FE_LOG(...)
#endif


#endif

