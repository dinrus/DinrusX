// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXLiveCreate/StdAfx.h>
#include <DinrusX/DinrusXNetwork/IServiceNetwork.h>
#include <DinrusX/DinrusXNetwork/IRemoteCommand.h>
#include <DinrusX/DinrusXLiveCreate/LiveCreateUpr.h>
#include <DinrusX/DinrusXLiveCreate/LiveCreateHost.h>
#include <DinrusX/DinrusXLiveCreate/LiveCreateNativeFile.h>

#ifndef NO_LIVECREATE

namespace LiveCreate
{

CLiveCreateFileReader::CLiveCreateFileReader()
	: m_handle(NULL)
{
}

CLiveCreateFileReader::~CLiveCreateFileReader()
{
	if (NULL != m_handle)
	{
		gEnv->pDrxPak->FClose(m_handle);
		m_handle = NULL;
	}
}

void CLiveCreateFileReader::Delete()
{
	delete this;
}

void CLiveCreateFileReader::Read(void* pData, const uint32 size)
{
	DWORD readBytes = 0;
	gEnv->pDrxPak->FRead((char*)pData, size, m_handle);
}

void CLiveCreateFileReader::Skip(const uint32 size)
{
	gEnv->pDrxPak->FSeek(m_handle, size, SEEK_CUR);
}

void CLiveCreateFileReader::Read8(void* pData)
{
	Read(pData, sizeof(uint64));
	SwapEndian<uint64>(*(uint64*)pData, eLittleEndian);
}

void CLiveCreateFileReader::Read4(void* pData)
{
	Read(pData, sizeof(uint32));
	SwapEndian<uint32>(*(uint32*)pData, eLittleEndian);
}

void CLiveCreateFileReader::Read2(void* pData)
{
	Read(pData, sizeof(uint16));
	SwapEndian<uint16>(*(uint16*)pData, eLittleEndian);
}

void CLiveCreateFileReader::Read1(void* pData)
{
	Read(pData, sizeof(uint8));
}

const void* CLiveCreateFileReader::GetPointer()
{
	return NULL;
}

CLiveCreateFileReader* CLiveCreateFileReader::CreateReader(const char* szNativePath)
{
	FILE* handle = gEnv->pDrxPak->FOpenRaw(szNativePath, "rb");
	if (NULL == handle)
	{
		return NULL;
	}

	CLiveCreateFileReader* ret = new CLiveCreateFileReader();
	ret->m_handle = handle;
	return ret;
}

}

#endif
