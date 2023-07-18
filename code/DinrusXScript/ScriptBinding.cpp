// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ScriptBinding.cpp
//  Version:     v1.00
//  Created:     9/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXScript/StdAfx.h>
#include <DinrusX/DinrusXScript/ScriptBinding.h>

#include <DinrusX/DinrusXScript/ScriptBind_System.h>
#include <DinrusX/DinrusXScript/ScriptBind_Particle.h>
#include <DinrusX/DinrusXScript/ScriptBind_Sound.h>
#include <DinrusX/DinrusXScript/ScriptBind_Movie.h>
#include <DinrusX/DinrusXScript/ScriptBind_Script.h>
#include <DinrusX/DinrusXScript/ScriptBind_Physics.h>

#include <DinrusX/DinrusXScript/SurfaceTypes.h>

#include <DinrusX/DinrusXScript/IScriptSystem.h>

CScriptBindings::CScriptBindings()
{
	m_pScriptSurfaceTypes = 0;
}

CScriptBindings::~CScriptBindings()
{
	Done();
}

//////////////////////////////////////////////////////////////////////////
void CScriptBindings::Init(ISystem* pSystem, IScriptSystem* pSS)
{
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_System(pSS, pSystem)));
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_Particle(pSS, pSystem)));
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_Sound(pSS, pSystem)));
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_Movie(pSS, pSystem)));
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_Script(pSS, pSystem)));
	m_binds.push_back(std::unique_ptr<CScriptableBase>(new CScriptBind_Physics(pSS, pSystem)));

	//////////////////////////////////////////////////////////////////////////
	// Enumerate script surface types.
	//////////////////////////////////////////////////////////////////////////
	m_pScriptSurfaceTypes = new CScriptSurfaceTypesLoader;
}

//////////////////////////////////////////////////////////////////////////
void CScriptBindings::Done()
{
	if (m_pScriptSurfaceTypes)
		delete m_pScriptSurfaceTypes;
	m_pScriptSurfaceTypes = NULL;

	// Done script bindings.
	m_binds.clear();
}

//////////////////////////////////////////////////////////////////////////
void CScriptBindings::LoadScriptedSurfaceTypes(const char* sFolder, bool bReload)
{
	m_pScriptSurfaceTypes->LoadSurfaceTypes(sFolder, bReload);
}

//////////////////////////////////////////////////////////////////////////
void CScriptBindings::GetMemoryStatistics(IDrxSizer* pSizer) const
{
	//pSizer->AddObject(m_binds);
}
