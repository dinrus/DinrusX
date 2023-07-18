// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   UIGameEvents.h
//  Version:     v1.00
//  Created:     19/03/2012 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIGameEvents_H__
#define __UIGameEvents_H__

#include <DinrusX/DinrusXGame/IUIGameEventSystem.h>
#include <DinrusX/DinrusXSys/Scaleform/IFlashUI.h>
#include <ILevelSystem.h>

class CUIGameEvents 
	: public IUIGameEventSystem
{
public:
	CUIGameEvents();

	// IUIGameEventSystem
	UIEVENTSYSTEM( "UIGameEvents" );
	virtual void InitEventSystem() override;
	virtual void UnloadEventSystem() override;

private:
	// UI events
	void OnLoadLevel( const char* mapname, bool isServer, const char* gamerules );
	void OnReloadLevel();
	void OnSaveGame( bool shouldResume );
	void OnLoadGame( bool shouldResume );
	void OnPauseGame();
	void OnResumeGame();
	void OnExitGame();
	void OnStartGame();

private:
	IUIEventSystem* m_pUIEvents;
	SUIEventReceiverDispatcher<CUIGameEvents> m_eventDispatcher;

	IGameFramework* m_pGameFramework;
	ILevelSystem* m_pLevelSystem;
};


#endif // __UIGameEvents_H__
