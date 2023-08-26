// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// ------------------------------------------------------------------------
//  File name:   LZ4Decompressor.cpp
//  Created:     5/9/2012 by Axel Gneiting
//  Описание: lz4 hc decompress wrapper
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <lz4.h>
#include <DinrusX/DinrusXSys/LZ4Decompressor.h>

bool CLZ4Decompressor::DecompressData(const char* pIn, char* pOut, const uint outputSize) const
{
	return LZ4_decompress_fast(pIn, pOut, outputSize) >= 0;
}

void CLZ4Decompressor::Release()
{
	delete this;
}
