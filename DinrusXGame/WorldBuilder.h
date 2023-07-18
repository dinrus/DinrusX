// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: 
  
 -------------------------------------------------------------------------
  History:
  - Created by Marco Corbetta

*************************************************************************/

#ifndef __WorldBuilder_h__
#define __WorldBuilder_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <DinrusX/DinrusXAct/ILevelSystem.h>
#include <DinrusX/DinrusXGame/PrefabUpr.h>

////////////////////////////////////////////////////////////////////////////
inline uint hash(int key)
{
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}

inline uint VecHash(const Vec3 &vPos)
{
	uint x=*((unsigned *) & vPos.x)*73856093;
	uint y=*((unsigned *) & vPos.y)*19349663;
	uint z=*((unsigned *) & vPos.z)*83492791;
	return (x^y^z);
}

inline uint VecHash2(const Vec3 &vPos)
{
	uint x=(unsigned)(vPos.x*73856093);
	uint y=(unsigned)(vPos.y*19349663);
	uint z=(unsigned)(vPos.z*83492791);
	return hash(x^hash(y^z));
}

////////////////////////////////////////////////////////////////////////////
class CWorldBuilder : public ILevelSystemListener
{
public:
	CWorldBuilder();
	~CWorldBuilder();
	
	// ILevelSystemListener
	virtual void OnLevelNotFound(const char* levelName) {};
	virtual void OnLoadingLevelEntitiesStart(ILevelInfo* pLevel) {}
	virtual void OnLoadingStart(ILevelInfo* pLevel);
	virtual void OnLoadingComplete(ILevelInfo* pLevel);
	virtual void OnLoadingError(ILevelInfo* pLevel, const char* error) {};
	virtual void OnLoadingProgress(ILevelInfo* pLevel, int progressAmount) {};
	virtual void OnUnloadComplete(ILevelInfo* pLevel);
	// ~ILevelSystemListener

	DrxGame::CPrefabUpr &GetPrefabUpr() { return (m_PrefabUpr); }

protected:
	// Draw debug info
	void DrawDebugInfo();

	DrxGame::CPrefabUpr m_PrefabUpr;

};

#endif // WorldBuilder
