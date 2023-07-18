// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "Metadata/Metadata.h"

class IConfig;
struct IResourceCompiler;
class CAssetUpr;

namespace AssetUpr
{

class CAsset
{
	friend CAssetUpr;
public:
	//! Returns true if successful, otherwise returns false.
	bool SetMetadata(const SAssetMetadata& metadata);

	//! Returns false if metadata does not exist or an error occurs, otherwise returns true.
	bool GetMetadata(SAssetMetadata& metadata) const;
private:
	//! Returns instance of CAsset if successful or metadata file does not exist, otherwise returns nullptr.
	static std::unique_ptr<CAsset> Create(IResourceCompiler* pRc, const string& metadataFilename);

	//! Returns metadata root node.
	XmlNodeRef GetMetadataRoot();

	bool Save(const string& filename);

	CAsset(IResourceCompiler* pRC);
	bool New();
	bool Read(const string& filename);
	static bool IsXml(const string& filename);
private:
	IResourceCompiler* m_pRC;
	XmlNodeRef m_xml;
};

}
