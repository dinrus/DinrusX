// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*=============================================================================
   TextureUpr.cpp : Common texture manager implementation.

   Revision history:
* Created by Kenzo ter Elst

   =============================================================================*/

#include <DinrusX/DinrusXRend/StdAfx.h>

#include <DinrusX/DinrusXRend/TextureUpr.h>
#include <DinrusX/DinrusXRend/Texture.h>

#include <DinrusXSys/ISystem.h>

//////////////////////////////////////////////////////////////////////////

CTextureUpr::~CTextureUpr()
{
	ReleaseDefaultTextures();
}

//////////////////////////////////////////////////////////////////////////

void CTextureUpr::PreloadDefaultTextures()
{

	if (m_DefaultTextures.size())
		return;

	uint32 nDefaultFlags = FT_DONT_STREAM;

	XmlNodeRef root = GetISystem()->LoadXmlFromFile("%ENGINE%/EngineAssets/defaulttextures.xml");
	if (root)
	{
		for (int i = 0; i < root->getChildCount(); i++)
		{
			XmlNodeRef entry = root->getChild(i);
			if (!entry->isTag("entry"))
				continue;

			uint32 nFlags = nDefaultFlags;

			// check attributes to modify the loading flags
			int nNoMips = 0;
			if (entry->getAttr("nomips", nNoMips) && nNoMips)
				nFlags |= FT_NOMIPS;

			CTexture* pTexture = CTexture::ForName(entry->getContent(), nFlags, eTF_Unknown);
			if (pTexture)
			{
				CDrxNameTSCRC nameID(entry->getContent());
				m_DefaultTextures[nameID] = pTexture;
			}
		}
	}

}

void CTextureUpr::ReleaseDefaultTextures()
{
	int n = 0;
	for (TTextureMap::iterator it = m_DefaultTextures.begin(); it != m_DefaultTextures.end(); ++it)
	{
		SAFE_RELEASE(it->second);
		n++;
	}
	m_DefaultTextures.clear();
}

//////////////////////////////////////////////////////////////////////////

const CTexture* CTextureUpr::GetDefaultTexture(
  const string& sTextureName) const
{
	CDrxNameTSCRC nameID(sTextureName.c_str());
	return GetDefaultTexture(nameID);
}

const CTexture* CTextureUpr::GetDefaultTexture(
  const CDrxNameTSCRC& sTextureNameID) const
{
	TTextureMap::const_iterator it = m_DefaultTextures.find(sTextureNameID);
	if (it == m_DefaultTextures.end())
		return 0;

	return it->second;
}

//////////////////////////////////////////////////////////////////////////
