// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// ------------------------------------------------------------------------
//  File name:   ZlibDecompressor.h
//  Created:     30/8/2012 by Axel Gneiting
//  Описание: zlib inflate wrapper
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ZLIBDECOMPRESSOR_H__
#define __ZLIBDECOMPRESSOR_H__

#include <DinrusX/DinrusXSys/IZlibDecompressor.h>

class CZLibDecompressor : public IZLibDecompressor
{
public:
	virtual IZLibInflateStream* CreateInflateStream();
	virtual void                Release();

private:
	virtual ~CZLibDecompressor() {}
};

#endif // __ZLIBDECOMPRESSOR_H__
