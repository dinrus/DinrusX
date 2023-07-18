// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/yasli/Config.h>
#include <DinrusX/CoreX/Serialization/Forward.h>

namespace Serialization
{

struct OutputFilePath
{
	string* path;
	//! If we don't use dynamic filters we could replace following two with const char*.
	string  filter;
	string  startFolder;

	//! Filters are defined in the following format:
	//! "All Images (bmp, jpg, tga)|*.bmp;*.jpg;*.tga|Targa (tga)|*.tga".
	explicit OutputFilePath(string& path, const char* filter = "All files|*.*", const char* startFolder = "")
		: path(&path)
		, filter(filter)
		, startFolder(startFolder)
	{
	}

	//! This function should stay virtual to ensure cross-dll calls are using right heap.
	virtual void SetPath(const char* path) { *this->path = path; }
};

bool Serialize(Serialization::IArchive& ar, Serialization::OutputFilePath& value, const char* name, const char* label);

}

#include "OutputFilePathImpl.h"
