// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <Core/Platform/platform.h>

#include <DinrusXSys/XML/IXml.h>

// Standard C headers.
#include <direct.h>

// STL headers.
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <map>
#include <set>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <tchar.h>

#define DRX_SUPPRESS_DRXENGINE_WINDOWS_FUNCTION_RENAMING
#include <Core/Platform/DrxWindows.h>

//#include <StlDbgAlloc.h>
// to make smoother transition back from drx to std namespace...
#define drx std
#define DRX_AS_STD

#include <Core/smartptr.h>

//////////////////////////////////////////////////////////////////////////
#include "ConvertContext.h"

#include <Core/Math/Drx_Math.h>
#include <DinrusXPhys/primitives.h>
#include <DinrusX/DinrusX3dEng/CGF/DrxHeaders.h>
#include <DinrusXSys/DrxVersion.h>

#include <Core/StlUtils.h>
#include <Core/Containers/VectorMap.h>
