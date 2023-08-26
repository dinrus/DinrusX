// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _GIMAGEINFO_XRENDER_H_
#define _GIMAGEINFO_XRENDER_H_

#pragma once

#ifdef INCLUDE_SCALEFORM_SDK

	#pragma warning(push)
	#pragma warning(disable : 6326)// Potential comparison of a constant with another constant
	#pragma warning(disable : 6011)// Dereferencing NULL pointer
	#include <DinrusX/CoreX/Platform/DrxWindows.h>
	#include <GImageInfo.h> // includes <windwows.h>
	#pragma warning(pop)

struct IFlashLoadMovieImage;

class DrxGImageInfoBase : public GImageInfoBase
{
public:
	DrxGImageInfoBase() {}
};

class GImageInfoXRender : public DrxGImageInfoBase
{
	// GImageInfoBase interface
public:
	virtual UInt      GetWidth() const;
	virtual UInt      GetHeight() const;
	#if defined(GFX_AMP_SERVER)
	virtual UPInt     GetBytes() const;
	virtual UPInt     GetExternalBytes() const;
	virtual bool      IsExternal() const;

	virtual UInt32    GetImageId() const;
	virtual GImage*   GetImage() const;
	#endif
	virtual GTexture* GetTexture(GRenderer* pRenderer);

public:
	GImageInfoXRender(GImage* pImage);
	virtual ~GImageInfoXRender();

private:
	uint32         m_width;
	uint32         m_height;
	GPtr<GImage>   m_pImg;
	GPtr<GTexture> m_pTex;
};

class GImageInfoFileXRender : public DrxGImageInfoBase
{
	// GImageInfoBase interface
public:
	virtual UInt      GetWidth() const;
	virtual UInt      GetHeight() const;
	#if defined(GFX_AMP_SERVER)
	virtual UPInt     GetBytes() const;
	virtual UPInt     GetExternalBytes() const;
	virtual bool      IsExternal() const;

	virtual UInt32    GetImageId() const;
	virtual GImage*   GetImage() const;
	#endif
	virtual GTexture* GetTexture(GRenderer* pRenderer);

public:
	GImageInfoFileXRender(const char* pImgFilePath, uint32 targetWidth, uint32 targetHeight);
	virtual ~GImageInfoFileXRender();

private:
	uint32         m_targetWidth;
	uint32         m_targetHeight;
	string         m_imgFilePath;
	GPtr<GTexture> m_pTex;
};

class GImageInfoTextureXRender : public DrxGImageInfoBase
{
	// GImageInfoBase interface
public:
	virtual UInt      GetWidth() const;
	virtual UInt      GetHeight() const;
	#if defined(GFX_AMP_SERVER)
	virtual UPInt     GetBytes() const;
	virtual UPInt     GetExternalBytes() const;
	virtual bool      IsExternal() const;

	virtual UInt32    GetImageId() const;
	virtual GImage*   GetImage() const;
	#endif
	virtual GTexture* GetTexture(GRenderer* pRenderer);

public:
	GImageInfoTextureXRender(ITexture* pTexture);
	GImageInfoTextureXRender(ITexture* pTexture, int width, int height);
	virtual ~GImageInfoTextureXRender();

private:
	uint32         m_width;
	uint32         m_height;
	int            m_texId;
	GPtr<GTexture> m_pTex;
};

class GImageInfoILMISrcXRender : public DrxGImageInfoBase
{
	// GImageInfoBase interface
public:
	virtual UInt      GetWidth() const;
	virtual UInt      GetHeight() const;
	#if defined(GFX_AMP_SERVER)
	virtual UPInt     GetBytes() const;
	virtual UPInt     GetExternalBytes() const;
	virtual bool      IsExternal() const;

	virtual UInt32    GetImageId() const;
	virtual GImage*   GetImage() const;
	#endif
	virtual GTexture* GetTexture(GRenderer* pRenderer);

public:
	GImageInfoILMISrcXRender(IFlashLoadMovieImage* pSrc);
	virtual ~GImageInfoILMISrcXRender();

private:
	uint32                m_width;
	uint32                m_height;
	IFlashLoadMovieImage* m_pSrc;
	GPtr<GTexture>        m_pTex;
};

#endif // #ifdef INCLUDE_SCALEFORM_SDK

#endif // #ifndef _GIMAGEINFO_XRENDER_H_
