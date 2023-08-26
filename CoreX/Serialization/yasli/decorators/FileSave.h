// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/yasli/Config.h>

namespace yasli {

class Archive;

struct FileSave
{
	string* pathPointer;
	string path;
	string filter;
	string relativeToFolder;

	// filter is defined in the following format:
	// "All Images (*.bmp *.jpg *.tga);; Bitmap (*.bmp);; Targa (*.tga)"
	FileSave(string& path, const char* filter, const char* relativeToFolder = "")
	: pathPointer(&path)
	, filter(filter)
	, relativeToFolder(relativeToFolder)
	{
		this->path = path;
	}

	FileSave() : pathPointer(0) { }

	FileSave& operator=(const FileSave& rhs)
	{
		path = rhs.path;
		if (rhs.pathPointer) {
			path = rhs.path;
			filter = rhs.filter;
			relativeToFolder = rhs.relativeToFolder;
		}
		return *this;
	}

	~FileSave()
	{
		if (pathPointer)
			*pathPointer = path;
	}

	void YASLI_SERIALIZE_METHOD(Archive& ar);
};

bool YASLI_SERIALIZE_OVERRIDE(Archive& ar, FileSave& value, const char* name, const char* label);

}

#if YASLI_INLINE_IMPLEMENTATION
#include <DinrusX/CoreX/Serialization/yasli/decorators/FileSaveImpl.h>
#endif

