// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

namespace Serialization
{

struct ResourceFolderPath
{
	string* path;
	string  startFolder;

	explicit ResourceFolderPath(string& path, const char* startFolder = "")
		: path(&path)
		, startFolder(startFolder)
	{
	}

	//! The function should stay virtual to ensure cross-dll calls are using right heap.
	virtual void SetPath(const char* path) { *this->path = path; }
};

bool Serialize(Serialization::IArchive& ar, Serialization::ResourceFolderPath& value, const char* name, const char* label);

}

#include "ResourceFolderPathImpl.h"
