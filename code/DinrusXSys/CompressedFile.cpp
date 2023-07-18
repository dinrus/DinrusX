// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/System.h>
#include <plugin/z/lib/zlib.h>

bool CSystem::CompressDataBlock(const void* input, size_t inputSize, void* output, size_t& outputSize, int level)
{
	uLongf destLen = outputSize;
	Bytef* dest = static_cast<Bytef*>(output);
	uLong sourceLen = inputSize;
	const Bytef* source = static_cast<const Bytef*>(input);
	bool ok = Z_OK == compress2(dest, &destLen, source, sourceLen, level);
	outputSize = destLen;
	return ok;
}

bool CSystem::DecompressDataBlock(const void* input, size_t inputSize, void* output, size_t& outputSize)
{
	uLongf destLen = outputSize;
	Bytef* dest = static_cast<Bytef*>(output);
	uLong sourceLen = inputSize;
	const Bytef* source = static_cast<const Bytef*>(input);
	bool ok = Z_OK == uncompress(dest, &destLen, source, sourceLen);
	outputSize = destLen;
	return ok;
}
