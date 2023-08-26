// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Разработка (C), Dinrus Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   StatCGFPhysicalize.h
//  Version:     v1.00
//  Created:     8/11/2004 by Timur.
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __StatCGFPhysicalize_h__
#define __StatCGFPhysicalize_h__
#pragma once

#include "PhysWorld.h"
#include <DinrusX/DinrusX3dEng/CGF/CGFContent.h>

//////////////////////////////////////////////////////////////////////////
class CPhysicsInterface
{
public:
	CPhysicsInterface();
	~CPhysicsInterface();

	enum EPhysicalizeResult
	{
		ePR_Empty,
		ePR_Ok,
		ePR_Fail
	};

	EPhysicalizeResult Physicalize( CNodeCGF *pNodeCGF,CContentCGF *pCGF );
	bool DeletePhysicalProxySubsets( CNodeCGF *pNodeCGF,bool bCga );
	void ProcessBreakablePhysics( CContentCGF *pCompiledCGF,CContentCGF *pSrcCGF );
	int CheckNodeBreakable(CNodeCGF *pNode, IGeometry *pGeom=0);
	IGeomUpr* GetGeomUpr() { return m_pGeomUpr; }

	// When node already physicalized, physicalize it again.
	void RephysicalizeNode( CNodeCGF *pNodeCGF,CContentCGF *pCGF );

private:
	EPhysicalizeResult PhysicalizeGeomType( int nGeomType,CNodeCGF *pNodeCGF,CContentCGF *pCGF );

	IGeomUpr* m_pGeomUpr;
	CPhysWorldLoader m_physLoader;
};

#endif // __StatCGFPhysicalize_h__
