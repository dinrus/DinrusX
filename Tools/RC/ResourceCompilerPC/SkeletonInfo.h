// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Разработка (C), Dinrus Studios, 2001-2006.
// -------------------------------------------------------------------------
//  File name:   AnimationInfoLoader.h
//  Version:     v1.00
//  Created:     22/6/2006 by Alexey Medvedev.
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef _SKELETON_INFO
#define _SKELETON_INFO
#pragma once

#include <DinrusX/DinrusX3dEng/CGF/CGFContent.h>

class CSkeletonInfo
{
public:
	CSkeletonInfo();
	~CSkeletonInfo();

	bool LoadFromChr(const char* name);
	bool LoadFromCga(const char* name);

public:
	CSkinningInfo m_SkinningInfo;
};

#endif
