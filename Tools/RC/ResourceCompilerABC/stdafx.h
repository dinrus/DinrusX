// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <DinrusX/CoreX/Platform/platform.h>

#include <functional>
#include <vector>
#include <map>

#include <stdio.h>
#include <tchar.h>

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/CoreX/Math/Drx_Geo.h>

#include <DinrusXSys/File/DrxFile.h>

#include <DinrusXSys/XML/IXml.h>

#include "IRCLog.h"

#include <DinrusX/CoreX/Renderer/VertexFormats.h>

#define DRX_SUPPRESS_DRXENGINE_WINDOWS_FUNCTION_RENAMING
#include <DinrusX/CoreX/Platform/DrxWindows.h>

#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>