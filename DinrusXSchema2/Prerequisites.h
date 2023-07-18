// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <limits>

#include <DinrusX/DinrusXEntity/IEntity.h>
#include <DinrusX/DinrusXNetwork/INetwork.h>
#include <DinrusX/CoreX/Serialization/ClassFactory.h>
#include <DinrusX/CoreX/Serialization/Enum.h>
#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/Math.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_ArrayView.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_PreprocessorUtils.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_ScopedConnection.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_TypeWrapper.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_Signal.h>
#include <DinrusX/DinrusXSchema2/GUID.h>

#ifdef _RELEASE
#define SCHEMATYC2_DEBUG_BREAK
#else
#define SCHEMATYC2_DEBUG_BREAK DrxDebugBreak();
#endif

#define SCHEMATYC2_ASSERTS_ENABLED 1

#define SCHEMATYC2_FILE_NAME __FILE__

#if defined(SCHEMATYC2_EXPORTS)
#define SCHEMATYC2_API __declspec(dllexport)
#else
#define SCHEMATYC2_API __declspec(dllimport)
#endif

// Copied from DrxTpeInfo.cpp.
// #SchematycTODO : Find a better home for this?
#if defined(LINUX) || defined(APPLE) || defined(ORBIS)

inline char* SCHEMATYC2_I64TOA(int64 value, char* szOutput, int32 radix)
{
	if(radix == 10)
	{
		sprintf(szOutput, "%llu", static_cast<unsigned long long>(value));
	}
	else
	{
		sprintf(szOutput, "%llx", static_cast<unsigned long long>(value));
	}
	return szOutput;
}

inline char* SCHEMATYC2_ULTOA(uint32 value, char* szOutput, int32 radix)
{
	if(radix == 10)
	{
		sprintf(szOutput, "%.d", value);
	}
	else
	{
		sprintf(szOutput, "%.x", value);
	}
	return szOutput;
}

#else

#define SCHEMATYC2_I64TOA _i64toa
#define SCHEMATYC2_ULTOA  ultoa

#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
