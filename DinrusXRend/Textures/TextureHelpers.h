// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   TextureHelpers.h
//  Version:     v1.00
//  Created:     12/6/2014 by NielsF.
//  Compilers:   Visual Studio 2012
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __TextureHelpers_h__
#define __TextureHelpers_h__
#pragma once

#include <DinrusX/DinrusXRend/Texture.h>

namespace TextureHelpers
{
//////////////////////////////////////////////////////////////////////////
EEfResTextures FindTexSlot(const char* texSemantic);
bool           VerifyTexSuffix(EEfResTextures texSlot, const char* texPath);
bool           VerifyTexSuffix(EEfResTextures texSlot, const string& texPath);
const char*    LookupTexSuffix(EEfResTextures texSlot);
int8           LookupTexPriority(EEfResTextures texSlot);
CTexture*      LookupTexDefault(EEfResTextures texSlot);
CTexture*      LookupTexNeutral(EEfResTextures texSlot);
}

#endif
