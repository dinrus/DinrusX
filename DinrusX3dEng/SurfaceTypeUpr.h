// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   SurfaceUpr.h
//  Version:     v1.00
//  Created:     29/9/2004 by Timur.
//  Compilers:   Visual Studio.NET
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SurfaceUpr_h__
#define __SurfaceUpr_h__
#pragma once

#include <DinrusX/DinrusX3dEng/I3DEngine.h>

#define MAX_SURFACE_ID 255

//////////////////////////////////////////////////////////////////////////
// SurfaceUpr is implementing ISurfaceUpr interface.
// Register and manages all known to game surface typs.
//////////////////////////////////////////////////////////////////////////
class CSurfaceTypeUpr : public ISurfaceTypeUpr, public DinrusX3dEngBase
{
public:
	CSurfaceTypeUpr();
	virtual ~CSurfaceTypeUpr();

	virtual void                    LoadSurfaceTypes();

	virtual ISurfaceType*           GetSurfaceTypeByName(const char* sName, const char* sWhy = NULL, bool warn = true);
	virtual ISurfaceType*           GetSurfaceType(int nSurfaceId, const char* sWhy = NULL);
	virtual ISurfaceTypeEnumerator* GetEnumerator();

	bool                            RegisterSurfaceType(ISurfaceType* pSurfaceType, bool bDefault = false);
	void                            UnregisterSurfaceType(ISurfaceType* pSurfaceType);

	ISurfaceType*                   GetSurfaceTypeFast(int nSurfaceId, const char* sWhy = NULL);

	void                            RemoveAll();

	void                            GetMemoryUsage(IDrxSizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_nameToSurface);
		for (int i = 0; i < MAX_SURFACE_ID + 1; ++i)
			pSizer->AddObject(m_idToSurface[i]);
	}
private:
	int                         m_lastSurfaceId;
	int                         m_lastDefaultId;

	class CMaterialSurfaceType* m_pDefaultSurfaceType;

	void                        RegisterAllDefaultTypes();
	CMaterialSurfaceType*       RegisterDefaultType(const char* szName);
	void                        ResetSurfaceTypes();

	struct SSurfaceRecord
	{
		bool          bLoaded;
		ISurfaceType* pSurfaceType;

		void          GetMemoryUsage(IDrxSizer* pSizer) const
		{
			pSizer->AddObject(this, sizeof(*this));
		}
	};

	SSurfaceRecord* m_idToSurface[MAX_SURFACE_ID + 1];

	typedef std::map<string, SSurfaceRecord*> NameToSurfaceMap;
	NameToSurfaceMap m_nameToSurface;
};

#endif //__SurfaceUpr_h__
