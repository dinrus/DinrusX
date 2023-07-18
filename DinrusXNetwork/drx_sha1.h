// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRX_SHA1_H__
#define __DRX_SHA1_H__

namespace DrxSHA1
{
void sha1Calc(const char* str, uint32* digest);
void sha1Calc(const unsigned char* str, uint64 length, uint32* digest);
};

#endif // __DRX_SHA1_H__
