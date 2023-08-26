// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <DinrusX/CoreX/Platform/platform.h>

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
#include <DinrusX/CoreX/Platform/DrxWindows.h>

//#include <StlDbgAlloc.h>
// to make smoother transition back from drx to std namespace...
#define drx std
#define DRX_AS_STD

#include <DinrusX/CoreX/smartptr.h>

//////////////////////////////////////////////////////////////////////////
#include "ConvertContext.h"

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusXPhys/primitives.h>
#include <DinrusX/DinrusX3dEng/CGF/DrxHeaders.h>
#include <DinrusXSys/DrxVersion.h>

#include <DinrusX/CoreX/StlUtils.h>
#include <DinrusX/CoreX/Containers/VectorMap.h>
