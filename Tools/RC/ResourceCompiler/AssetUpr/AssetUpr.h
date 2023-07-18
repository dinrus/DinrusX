// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "IAssetUpr.h"

struct IResourceCompiler;
class IConfig;
class XmlNodeRef;

class CAssetUpr : public IAssetUpr
{
public:
	CAssetUpr(IResourceCompiler* pRc);
	virtual void RegisterDetailProvider(FnDetailsProvider detailsProvider, const char* szExt) override;
	virtual bool SaveDrxasset(const IConfig* const pConfig, const char* szSourceFilepath, size_t filesCount, const char** pFiles, const char* szOutputFolder) const override;

private:
	bool CollectMetadataDetails(XmlNodeRef& xml, const std::vector<string>& files) const;
private:
	IResourceCompiler* m_pRc;

	VectorMap<string, FnDetailsProvider> m_providers;
};