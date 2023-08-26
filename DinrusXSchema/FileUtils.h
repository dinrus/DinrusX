// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>

namespace Schematyc
{
namespace FileUtils
{
enum class EFileEnumFlags
{
	None                     = 0,
	IgnoreUnderscoredFolders = BIT(0),
	Recursive                = BIT(1),
};

typedef CEnumFlags<EFileEnumFlags>              FileEnumFlags;

typedef std::function<void (const char*, unsigned)> FileEnumCallback;

void EnumFilesInFolder(const char* szFolderName, const char* szExtension, FileEnumCallback callback, const FileEnumFlags& flags = EFileEnumFlags::Recursive);
} // FileUtils
} // Schematyc
