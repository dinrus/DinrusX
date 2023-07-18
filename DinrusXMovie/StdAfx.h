// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0B7BFEE0_95B3_4DD3_956A_33AD2ADB212D__INCLUDED_)
#define AFX_STDAFX_H__0B7BFEE0_95B3_4DD3_956A_33AD2ADB212D__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_Movie

#define DRXMOVIE_EXPORTS

#include <DinrusX/CoreX/Platform/platform.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include <DinrusX/CoreX/Math/Drx_Math.h>
#include <DinrusX/DinrusXSys/IXml.h>
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXEntity/IEntitySystem.h>
#include <DinrusX/DinrusXMovie/IMovieSystem.h>
#include <DinrusX/CoreX/smartptr.h>
#include <DinrusX/CoreX/Memory/DrxSizer.h>

#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/IntrusiveFactory.h>
#include <DinrusX/CoreX/Serialization/Enum.h>
#include <DinrusX/CoreX/Serialization/Color.h>
#include <DinrusX/CoreX/Serialization/Math.h>
#include <DinrusX/CoreX/Serialization/SmartPtr.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0B7BFEE0_95B3_4DD3_956A_33AD2ADB212D__INCLUDED_)
