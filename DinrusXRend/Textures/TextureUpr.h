// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*=============================================================================
   TextureUpr.h : Common texture manager declarations.

   Revision history:
* Created by Kenzo ter Elst
   =============================================================================*/

#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H

#include <DinrusX/CoreX/String/DrxName.h>

class CTexture;

class CTextureUpr
{
public:

	CTextureUpr() {}
	virtual ~CTextureUpr();

	void            PreloadDefaultTextures();
	void            ReleaseDefaultTextures();

	const CTexture* GetDefaultTexture(const string& sTextureName) const;
	const CTexture* GetDefaultTexture(const CDrxNameTSCRC& sTextureNameID) const;

private:

	typedef std::map<CDrxNameTSCRC, CTexture*> TTextureMap;
	TTextureMap m_DefaultTextures;
};

#endif
