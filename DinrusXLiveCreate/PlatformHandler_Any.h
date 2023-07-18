// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _LIVECREATE_PLATFORMHANDLER_ANY_H_
#define _LIVECREATE_PLATFORMHANDLER_ANY_H_

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXLiveCreate/ILiveCreateCommon.h>
#include "Platform_Common/PlatformHandlerBase.h"

#if defined(LIVECREATE_FOR_PC) && !defined(NO_LIVECREATE)

namespace LiveCreate
{

// General handler than can handle any DinrusX game
class PlatformHandler_Any : public PlatformHandlerBase
{
public:
	PlatformHandler_Any(IPlatformHandlerFactory* pFactory, const char* szTargetName);
	virtual ~PlatformHandler_Any();

	// IPlatformHandler interface implementation
	virtual void        Delete();
	virtual bool        IsOn() const;
	virtual bool        Launch(const char* pExeFilename, const char* pWorkingFolder, const char* pArgs) const;
	virtual bool        Reset(EResetMode aMode) const;
	virtual bool        ScanFolder(const char* pFolder, IPlatformHandlerFolderScan& outResult) const;
	virtual const char* GetRootPath() const;
};

// General handler than can handle any DinrusX game (requires only connection)
class PlatformHandlerFactory_Any : public IPlatformHandlerFactory
{
public:
	// IPlatformHandlerFactory interface implementation
	virtual const char*       GetPlatformName() const;
	virtual IPlatformHandler* CreatePlatformHandlerInstance(const char* pTargetMachineName);
	virtual uint32            ScanForTargets(TargetInfo* outTargets, const uint maxTargets);
	virtual bool              ResolveAddress(const char* pTargetMachineName, char* pOutIpAddress, uint32 aMaxOutIpAddressSize);
};
}

#endif
#endif
