// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   UIMenuEvents.h
//  Version:     v1.00
//  Created:     21/11/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIMenuEvents_H__
#define __UIMenuEvents_H__

#include <DinrusX/DinrusXGame/IUIGameEventSystem.h>
#include <DinrusX/DinrusXSys/Scaleform/IFlashUI.h>
#include <DinrusX/CoreX/Game/IGameFramework.h>

class CUIMenuEvents
	: public IUIGameEventSystem
	, public IUIModule
{
public:
	CUIMenuEvents();

	// IUIGameEventSystem
	UIEVENTSYSTEM( "UIMenuEvents" );
	virtual void InitEventSystem() override;
	virtual void UnloadEventSystem() override;

	//IUIModule
	virtual void Reset() override;

	void DisplayIngameMenu(bool bDisplay);
	bool IsIngameMenuStarted() const { return m_bIsIngameMenuStarted; }

private:
	void StartIngameMenu();
	void StopIngameMenu();

private:
	enum EUIEvent
	{
		eUIE_StartIngameMenu,
		eUIE_StopIngameMenu,
	};

	SUIEventReceiverDispatcher<CUIMenuEvents> m_eventDispatcher;
	SUIEventSenderDispatcher<EUIEvent> m_eventSender;
	IUIEventSystem* m_pUIEvents;
	IUIEventSystem* m_pUIFunctions;

	bool m_bIsIngameMenuStarted;
};


#endif // __UISettings_H__