// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   3DEngineMemory.h
//  Version:     v1.00
//  Created:     23/04/2010 by Chris Raine.
//  Compilers:   Visual Studio.NET
// -------------------------------------------------------------------------
//  History:
////////////////////////////////////////////////////////////////////////////
#include <DinrusX/DinrusX3dEng/StdAfx.h>
#include <DinrusX/DinrusX3dEng/3DEngineMemory.h>

// Static CTemporaryPool instance
CTemporaryPool* CTemporaryPool::s_Instance = NULL;

namespace util
{
void* pool_allocate(size_t nSize)
{
	return CTemporaryPool::Get()->Allocate(nSize, 16);  // Align for possible SIMD types
}
void pool_free(void* ptr)
{
	return CTemporaryPool::Get()->Free(ptr);
}
}
