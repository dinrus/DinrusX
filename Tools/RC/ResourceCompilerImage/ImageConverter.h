// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "IConverter.h"  // IConverter
#include <DinrusX/CoreX/StlUtils.h>    // stl::less_stricmp

struct IResourceCompiler;

class CImageConverter
	: public IConverter
{
public:
	// Maps aliases of presets to real names of presets
	typedef std::map<string, string, stl::less_stricmp<string> > PresetAliases;

public:
	explicit CImageConverter(IResourceCompiler* pRC);
	~CImageConverter();

	// interface IConverter ----------------------------------------------------
	virtual void Release();
	virtual void Init(const ConverterInitContext& context);
	virtual ICompiler* CreateCompiler();
	virtual bool SupportsMultithreading() const;
	virtual const char* GetExt(int index) const;
	virtual bool IsCacheable() const override { return true; }
	// -------------------------------------------------------------------------

private:
	PresetAliases m_presetAliases;
};
