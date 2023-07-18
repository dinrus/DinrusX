// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   MiniButton.h
//  Created:     26/08/2009 by Timur.
//  Описание: Button implementation in the MiniGUI
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MiniButton_h__
#define __MiniButton_h__

#include <DinrusX/DinrusXSys/MiniGUI.h>

MINIGUI_BEGIN

//////////////////////////////////////////////////////////////////////////
// Root window all other controls derive from
class CMiniButton : public CMiniCtrl
{
public:
	CMiniButton();

	//////////////////////////////////////////////////////////////////////////
	// CMiniCtrl interface implementation.
	//////////////////////////////////////////////////////////////////////////
	virtual EMiniCtrlType GetType() const { return eCtrlType_Button; }
	virtual void          SetRect(const Rect& rc);
	virtual void          OnPaint(CDrawContext& dc);
	virtual void          OnEvent(float x, float y, EMiniCtrlEvent event);
	virtual void          Reset();
	virtual void          SaveState();
	virtual void          RestoreState();
	//////////////////////////////////////////////////////////////////////////

	virtual bool SetControlCVar(const char* sCVarName, float fOffValue, float fOnValue);
	virtual bool SetClickCallback(ClickCallback callback, void* pCallbackData);
	virtual bool SetConnectedCtrl(IMiniCtrl* pConnectedCtrl);

protected:
	ICVar*        m_pCVar;
	float         m_fCVarValue[2];
	ClickCallback m_clickCallback;
	void*         m_pCallbackData;
	IMiniCtrl*    m_pConnectedCtrl;
};

MINIGUI_END

#endif // __MiniButton_h__
