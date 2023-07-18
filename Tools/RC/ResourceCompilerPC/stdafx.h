// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <Core/Platform/platform.h>

typedef string tstring;

#include <malloc.h>			// malloc()

#include <float.h>

#include <memory>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <tchar.h>

#include <DinrusXSys/XML/IXml.h>

#include <Core/Math/Drx_Math.h>
#include <Core/Math/Drx_Geo.h>
#include <DinrusX/DinrusX3dEng/CGF/DrxHeaders.h>
#include <DinrusXPhys/primitives.h>
#include <Core/smartptr.h>
#include <DinrusXPhys/physinterface.h>
#include <Core/Memory/DrxSizer.h>

#include "ResourceCompilerPC.h"

#include <DinrusXSys/File/DrxFile.h>

#include <DinrusX/CoreX/Renderer/VertexFormats.h>

#include "..\ResourceCompiler\IRCLog.h"
#include "..\ResourceCompiler\SwapEndianness.h"
