// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/System.h>
#if DRX_PLATFORM_ANDROID && defined(DEDICATED_SERVER)
	#include <DinrusX/DinrusXSys/AndroidConsole.h>
#endif

#include <DinrusX/DinrusXSys/UnixConsole.h>
#ifdef USE_UNIXCONSOLE
extern __attribute__((visibility("default"))) CUNIXConsole* pUnixConsole;
#endif
