// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	Dummy font implementation (dedicated server)
   -------------------------------------------------------------------------
   History:
   - Jun 30,2006:	Created by Sascha Demetrio

*************************************************************************/

#ifndef __NULLFONT_H__
#define __NULLFONT_H__

#pragma once

#if defined(USE_NULLFONT)

	#include <DinrusX/DinrusXFont/IFont.h>

class CNullFont : public IFFont
{
public:
	CNullFont() {}
	virtual ~CNullFont() {}

	virtual void         Release()                                                                                                           {}
	virtual bool         Load(const char* pFontFilePath, unsigned int width, unsigned int height, unsigned int flags)                        { return true; }
	virtual bool         Load(const char* pXMLFile)                                                                                          { return true; }
	virtual void         Free()                                                                                                              {}

	virtual void         DrawString(float x, float y, const char* pStr, const bool asciiMultiLine, const STextDrawContext& ctx)              {}
	virtual void         DrawString(float x, float y, float z, const char* pStr, const bool asciiMultiLine, const STextDrawContext& ctx)     {}

	virtual void         DrawStringW(float x, float y, const wchar_t* pStr, const bool asciiMultiLine, const STextDrawContext& ctx)          {}
	virtual void         DrawStringW(float x, float y, float z, const wchar_t* pStr, const bool asciiMultiLine, const STextDrawContext& ctx) {}

	virtual Vec2         GetTextSize(const char* pStr, const bool asciiMultiLine, const STextDrawContext& ctx)                               { return Vec2(0.0f, 0.0f); }
	virtual Vec2         GetTextSizeW(const wchar_t* pStr, const bool asciiMultiLine, const STextDrawContext& ctx)                           { return Vec2(0.0f, 0.0f); }

	virtual size_t       GetTextLength(const char* pStr, const bool asciiMultiLine) const                                                    { return 0; }
	virtual size_t       GetTextLengthW(const wchar_t* pStr, const bool asciiMultiLine) const                                                { return 0; }

	virtual void         WrapText(string& result, float maxWidth, const char* pStr, const STextDrawContext& ctx)                             { result = pStr; }
	virtual void         WrapTextW(wstring& result, float maxWidth, const wchar_t* pStr, const STextDrawContext& ctx)                        { result = pStr; }

	virtual void         GetMemoryUsage(IDrxSizer* pSizer) const                                                                             {}

	virtual void         GetGradientTextureCoord(float& minU, float& minV, float& maxU, float& maxV) const                                   {}

	virtual unsigned int GetEffectId(const char* pEffectName) const                                                                          { return 0; }
	virtual void         RenderCallback(float x, float y, float z, const char* pStr, const bool asciiMultiLine, const STextDrawContext& ctx, IRenderAuxGeom *pAux) {}
};

class CDrxNullFont : public IDrxFont
{
public:
	virtual void    Release()                                   {}
	virtual IFFont* NewFont(const char* pFontName)              { return &ms_nullFont; }
	virtual IFFont* GetFont(const char* pFontName) const        { return &ms_nullFont; }
	virtual void    SetRendererProperties(IRenderer* pRenderer) {}
	virtual void    GetMemoryUsage(IDrxSizer* pSizer) const     {}
	virtual string  GetLoadedFontNames() const                  { return ""; }

private:
	static CNullFont ms_nullFont;
};

#endif // USE_NULLFONT

#endif
