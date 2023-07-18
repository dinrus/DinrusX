// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	Mouse Input implementation for Linux using SDL
   -------------------------------------------------------------------------
   History:
   - Aug 02,2013:	Created by Leander Beernaert

*************************************************************************/

#ifndef __SDLMOUSE_H__
#define __SDLMOUSE_H__

#include <DinrusX/DinrusXInput/LinuxInput.h>

struct IRenderer;
class CSDLMouse : public CLinuxInputDevice
{
public:
	CSDLMouse(CLinuxInput& input);

	virtual ~CSDLMouse();

	virtual int  GetDeviceIndex() const { return 0; }   //Assume only one mouse

	virtual bool Init();

	virtual void Update(bool focus);

	void         GrabInput();

	void         UngrabInput();

protected:

	void CapDeltas(float cap);

	void SmoothDeltas(float accel, float decel = 0.0f);

private:
	IRenderer* m_pRenderer;
	Vec2       m_deltas;
	Vec2       m_oldDeltas;
	Vec2       m_deltasInertia;
	bool       m_bGrabInput;
};

#endif // __SDLMOUSE_H__
