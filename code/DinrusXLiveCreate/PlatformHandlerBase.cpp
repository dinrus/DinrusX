// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

//////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXLiveCreate/StdAfx.h>
#include <DinrusX/DinrusXLiveCreate/PlatformHandlerBase.h>

#if defined(LIVECREATE_FOR_PC) && !defined(NO_LIVECREATE)

namespace LiveCreate
{

// Reimplement missing functions from DinrusX
	#ifdef DRXLIVECREATEPLATFORMS_EXPORTS
namespace
{
int DrxInterlockedIncrement(volatile int* value)
{
	return (int)InterlockedIncrement((long*)value);
}

int DrxInterlockedDecrement(volatile int* value)
{
	return (int)InterlockedDecrement((long*)value);
}
}
	#endif

PlatformHandlerBase::PlatformHandlerBase(IPlatformHandlerFactory* pFactory, const char* szTargetName)
	: m_pFactory(pFactory)
	, m_targetName(szTargetName)
	, m_refCount(1)
	, m_flags(0)
{
}

void PlatformHandlerBase::AddRef()
{
	DrxInterlockedIncrement((volatile int*)&m_refCount);
}

void PlatformHandlerBase::Release()
{
	if (0 == DrxInterlockedDecrement((volatile int*)&m_refCount))
	{
		Delete();
	}
}

PlatformHandlerBase::~PlatformHandlerBase()
{
}

const char* PlatformHandlerBase::GetTargetName() const
{
	return m_targetName.c_str();
}

const char* PlatformHandlerBase::GetPlatformName() const
{
	return m_pFactory->GetPlatformName();
}

IPlatformHandlerFactory* PlatformHandlerBase::GetFactory() const
{
	return m_pFactory;
}

bool PlatformHandlerBase::IsFlagSet(uint32 aFlag) const
{
	return 0 != (m_flags & aFlag);
}

bool PlatformHandlerBase::Launch(const char* pExeFilename, const char* pWorkingFolder, const char* pArgs) const
{
	return false;
}

bool PlatformHandlerBase::Reset(EResetMode aMode) const
{
	return false;
}

bool PlatformHandlerBase::IsOn() const
{
	return false;
}

bool PlatformHandlerBase::ScanFolder(const char* pFolder, IPlatformHandlerFolderScan& outResult) const
{
	return false;
}

bool PlatformHandlerBase::ResolveAddress(char* pOutIpAddress, uint32 aMaxOutIpAddressSize) const
{
	return m_pFactory->ResolveAddress(m_targetName.c_str(), pOutIpAddress, aMaxOutIpAddressSize);
}

}

#endif
