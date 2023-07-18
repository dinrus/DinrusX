// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusSystem\File\DrxFile.h>  // Includes DrxPath.h in correct order. 

struct IResourceCompiler;
class IConfig;
class XmlNodeRef;


// Fills xmlnode with details metatada properties for given asset data file.
// \return true if successfully completed, false if the file could not be recognized.
typedef bool(*FnDetailsProvider)(XmlNodeRef& xmlnode, const char* szFilename, IResourceCompiler* pRc);

struct IAssetUpr
{
	virtual ~IAssetUpr() {}

	//! Bind the file extension to the details provider.
	virtual void RegisterDetailProvider(FnDetailsProvider detailsProvider, const char* szExt) = 0;

	//! Save .drxasset file. The implementation calls the appropriate provider(s) to collect details of the asset data files.
	virtual bool SaveDrxasset(const IConfig* const pConfig, const char* szSourceFilepath, size_t filesCount, const char** pFiles, const char* szOutputFolder) const = 0;

	bool SaveDrxasset(const IConfig* const pConfig, const string& sourceFilepath, const std::vector<string>& files, const char* szOutputFolder = nullptr)
	{
		std::unique_ptr<const char*> pFiles(new const char*[files.size()]);
		for (size_t i = 0, n = files.size(); i < n; ++i)
		{
			pFiles.get()[i] = files[i].c_str();
		}
		return SaveDrxasset(pConfig, sourceFilepath.c_str(), files.size(), pFiles.get(), szOutputFolder);
	}

	static const char* GetExtension()
	{
		return "drxasset";
	}

	// Builds .drxasset file name for the data filename.
	static string GetMetadataFilename(const string& filename)
	{
		//  Special handling of RC intermediate files: filename.$ext -> filename.$ext.$drxasset
		const char* szExt = PathUtil::GetExt(filename);
		if (szExt && szExt[0] == '$')
		{
			return  filename + ".$" + GetExtension();
		}

		return  filename + "." + GetExtension();
	}
};
