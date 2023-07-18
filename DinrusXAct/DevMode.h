// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	Helper class for CDrxAction implementing developer mode-only
                functionality

   -------------------------------------------------------------------------
   History:
   - 9:2:2005   12:31 : Created by Craig Tiller

*************************************************************************/
#ifndef __DEVMODE_H__
#define __DEVMODE_H__

#pragma once

#include <DinrusX/DinrusXInput/IInput.h>

struct STagFileEntry
{
	Vec3 pos;
	Ang3 ang;
};

class CDevMode : public IInputEventListener, public IRemoteConsoleListener
{
public:
	CDevMode();
	~CDevMode();

	void GotoTagPoint(int i);
	void SaveTagPoint(int i);

	// IInputEventListener
	bool OnInputEvent(const SInputEvent&);
	// ~IInputEventListener

	// IRemoteConsoleListener
	virtual void OnGameplayCommand(const char* cmd);
	// ~IRemoteConsoleListener

	void GetMemoryStatistics(IDrxSizer* s) { s->Add(*this); }

private:
	bool m_bSlowDownGameSpeed;
	bool m_bHUD;
	std::vector<STagFileEntry> LoadTagFile();
	void                       SaveTagFile(const std::vector<STagFileEntry>&);
	string                     TagFileName();
	void                       SwitchSlowDownGameSpeed();
	void                       SwitchHUD();
	void                       GotoSpecialSpawnPoint(int i);
};

#endif
