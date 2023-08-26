// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

class ITexture;
namespace LODGenerator 
{
	class CAutoGeneratorTextureCreator
	{
	public:
		CAutoGeneratorTextureCreator();
		~CAutoGeneratorTextureCreator();

	public:
		bool Generate(int level);
		void SetWidth(int iWidth);
		void SetHeight(int iHeight);

	private:
		CString GetPreset(const int texturetype);
		void AssignToMaterial(const int nLod, const bool bAlpha, const bool bSaveSpec, const CString& exportPath, const CString& fileNameTemplate);
		bool SaveTexture(ITexture* pTex,const bool bAlpha,const CString& fileName,const CString& texturePreset);
		bool VerifyNoOverlap(const int nSubMatIdx);
		bool SaveTextures(const int level);
		bool GenerateTexture(int level);

	private:
		int m_iWidth;
		int m_iHeight;
	};
}