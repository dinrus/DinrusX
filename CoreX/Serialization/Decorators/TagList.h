// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <vector>
#include <DinrusX/CoreX/Serialization/yasli/Config.h>
#include <DinrusX/CoreX/Serialization/Forward.h>

struct ITagSource
{
	virtual void         AddRef() = 0;
	virtual void         Release() = 0;
	virtual unsigned int TagCount(unsigned int group) const = 0;
	virtual const char*  TagValue(unsigned int group, unsigned int index) const = 0;
	virtual const char*  TagDescription(unsigned int group, unsigned int index) const = 0;
	virtual const char*  GroupName(unsigned int group) const = 0;
	virtual unsigned int GroupCount() const = 0;
};

struct TagList
{
	std::vector<Serialization::string>* tags;

	TagList(std::vector<Serialization::string>& tags)
		: tags(&tags)
	{
	}
};

bool Serialize(Serialization::IArchive& ar, TagList& tagList, const char* name, const char* label);

#include "TagListImpl.h"
