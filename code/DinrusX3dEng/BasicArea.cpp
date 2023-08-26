// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   statobjman.cpp
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание:    контейнер объектов, поточный, общая часть для внутренних
//               и наружных секторов.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusX3dEng/StdAfx.h>

CBasicArea::~CBasicArea()
{
	delete m_pObjectsTree;
	m_pObjectsTree = NULL;
}
