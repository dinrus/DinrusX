// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   DrxPlatform.h
//  Version:     v1.00
//  Created:     31/01/2013 by Christopher Bolte.
//  Compilers:   Visual Studio.NET
//  Описание: Interface for the platform specific function libraries
//               Include this file instead of windows.h and similar platform specific header files
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef _DRX_PLATFORM_H_
#define _DRX_PLATFORM_H_

#include <DinrusX/CoreX/Platform/DrxPlatformDefines.h>

////////////////////////////////////////////////////////////////////////////
//! This define allows including the detail headers which are setting platform specific settings.
#define DRXPLATFROM_ALLOW_DETAIL_INCLUDES

////////////////////////////////////////////////////////////////////////////
// some ifdef selection to include the correct platform implementation
#if DRX_PLATFORM_WINDOWS && DRX_PLATFORM_32BIT
	#include <DinrusX/CoreX/Platform/DrxPlatform.Win32.h>
#elif DRX_PLATFORM_WINDOWS && DRX_PLATFORM_64BIT
	#include <DinrusX/CoreX/Platform/DrxPlatform.Win64.h>
#elif DRX_PLATFORM_DURANGO
	#include <DinrusX/CoreX/Platform/DrxPlatform.Durango.h>
#elif DRX_PLATFORM_ORBIS
	#include <DinrusX/CoreX/Platform/DrxPlatform.Orbis.h>
#elif DRX_PLATFORM_LINUX || DRX_PLATFORM_ANDROID
	#include <DinrusX/CoreX/Platform/DrxPlatform.Linux.h>
#elif DRX_PLATFORM_APPLE
	#include <DinrusX/CoreX/Platform/DrxPlatform.Darwin.h>
#else
	#error Current Platform not supported by DrxPlatform. Please provide a concrete implementation library.
#endif

///////////////////////////////////////////////////////////////////////////
// Verify that all required macros are set.
///////////////////////////////////////////////////////////////////////////
#if !defined(__DETAIL__LINK_THIRD_PARTY_LIBRARY)
	#error __DETAIL__LINK_THIRD_PARTY_LIBRARY not defined for current platform
#endif

#if !defined(__DETAIL__LINK_SYSTEM_PARTY_LIBRARY)
	#error __DETAIL__LINK_SYSTEM_PARTY_LIBRARY not defined for current platform
#endif

#ifdef DRX_NO_PRAGMA_LIB
	#define LINK_THIRD_PARTY_LIBRARY(name)
	#define LINK_SYSTEM_LIBRARY(name)
#else

////////////////////////////////////////////////////////////////////////////
//! Include a third party library. The path has to be specified relative to the Code/ folder.
//! In addition the path has to be specified as a literal, not as a string, and forward slashes have to be used.
//! For example: LINK_THIRD_PARTY_LIBRARY("SDK/MyLib/lib/MyLib.a").
	#define LINK_THIRD_PARTY_LIBRARY(name) __DETAIL__LINK_THIRD_PARTY_LIBRARY(name)

////////////////////////////////////////////////////////////////////////////
//! Include a platform library.
	#define LINK_SYSTEM_LIBRARY(name) __DETAIL__LINK_SYSTEM_PARTY_LIBRARY(name)

#endif //DRX_NO_PRAGMA_LIB

////////////////////////////////////////////////////////////////////////////
//! Disallow including of detail header.
#undef DRXPLATFROM_ALLOW_DETAIL_INCLUDES

#endif // _DRX_PLATFORM_H_
