// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/CoreX/Serialization/yasli/decorators/FileSave.h>
#include <DinrusX/CoreX/Serialization/yasli/STL.h>
#include <DinrusX/CoreX/Serialization/yasli/Archive.h>
#include <DinrusX/CoreX/Serialization/yasli/STLImpl.h>

namespace yasli {

YASLI_INLINE void FileSave::YASLI_SERIALIZE_METHOD(Archive& ar)
{
	ar(path, "path");
	ar(filter, "filter");
	ar(relativeToFolder, "folder");
}

YASLI_INLINE bool YASLI_SERIALIZE_OVERRIDE(Archive& ar, FileSave& value, const char* name, const char* label)
{
	if (ar.isEdit())
		return ar(Serializer(value), name, label);
	else
		return ar(value.path, name, label);
}

}

