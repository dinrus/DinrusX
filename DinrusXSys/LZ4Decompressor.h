// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// ------------------------------------------------------------------------
//  File name:   LZ4Decompressor.h
//  Created:     5/9/2012 by Axel Gneiting
//  Описание: lz4 hc decompress wrapper
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __LZ4DECOMPRESSOR_H__
#define __LZ4DECOMPRESSOR_H__

#include <DinrusX/DinrusXSys/ILZ4Decompressor.h>

class CLZ4Decompressor : public ILZ4Decompressor
{
public:
	virtual bool DecompressData(const char* pIn, char* pOut, const uint outputSize) const;
	virtual void Release();

private:
	virtual ~CLZ4Decompressor() {}
};

#endif // __LZ4DECOMPRESSOR_H__
