// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------

Description: Utilities for debugging input synchronization problems

-------------------------------------------------------------------------
History:
-	30:03:2007  : Created by Craig Tiller

*************************************************************************/

#ifndef __NETINPUTCHAINDEBUG_H__
#define __NETINPUTCHAINDEBUG_H__

#if !DRX_PLATFORM_LINUX && !DRX_PLATFORM_ANDROID && !DRX_PLATFORM_ORBIS && !defined(_RELEASE)
	#define ENABLE_NETINPUTCHAINDEBUG 1
#endif

void NetInputChainInitCVars();

#if ENABLE_NETINPUTCHAINDEBUG
void NetInputChainPrint( const char * name, float val );
void NetInputChainPrint( const char * name, const Vec3& val );

extern EntityId _netinputchain_debugentity;

#define NETINPUT_TRACE(ent, val) if (ent != _netinputchain_debugentity); else NetInputChainPrint(#val, val)
#else
#define NETINPUT_TRACE(ent, val) ((void*)0)
#endif

#endif
