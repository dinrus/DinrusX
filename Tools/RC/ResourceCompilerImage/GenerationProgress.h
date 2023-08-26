// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

class CImageCompiler;

class CGenerationProgress
{
public:

	explicit CGenerationProgress(CImageCompiler& rImageCompiler);

	float Get();
	void Start();
	void Finish();

	void Phase1();
	void Phase2(const uint32 dwY, const uint32 dwHeight);
	void Phase3(const uint32 dwY, const uint32 dwHeight, const int iTemp);
	void Phase4(const uint32 dwMip, const int iBlockLines);

private: // ------------------------------------------------------------------

	void Set(float fProgress);
	void Increment(float fProgress);

	float									m_fProgress;
	CImageCompiler&				m_rImageCompiler;
	
};
