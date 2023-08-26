// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/FileUtils.h>
#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/StackString.h>

namespace Schematyc
{
namespace FileUtils
{
void EnumFilesInFolder(const char* szFolderName, const char* szExtension, FileEnumCallback callback, const FileEnumFlags& flags)
{
	SCHEMATYC_CORE_ASSERT(szFolderName);
	if (szFolderName)
	{
		SCHEMATYC_CORE_ASSERT(callback);
		if (callback)
		{
			if (flags.Check(EFileEnumFlags::Recursive))
			{
				CStackString searchPath = szFolderName;
				searchPath.append("/*.*");

				_finddata_t findData;
				intptr_t handle = gEnv->pDrxPak->FindFirst(searchPath.c_str(), &findData);
				if (handle >= 0)
				{
					do
					{
						if ((findData.name[0] != '.') && ((findData.attrib & _A_SUBDIR) != 0))
						{
							if (!flags.Check(EFileEnumFlags::IgnoreUnderscoredFolders) || (findData.name[0] != '_'))
							{
								CStackString subName = szFolderName;
								subName.append("/");
								subName.append(findData.name);
								EnumFilesInFolder(subName.c_str(), szExtension, callback, flags);
							}
						}
					}
					while (gEnv->pDrxPak->FindNext(handle, &findData) >= 0);
				}
			}

			CStackString searchPath = szFolderName;
			searchPath.append("/");
			searchPath.append(szExtension ? szExtension : "*.*");

			_finddata_t findData;
			intptr_t handle = gEnv->pDrxPak->FindFirst(searchPath.c_str(), &findData);
			if (handle >= 0)
			{
				do
				{
					if ((findData.name[0] != '.') && ((findData.attrib & _A_SUBDIR) == 0))
					{
						CStackString subName = szFolderName;
						subName.append("/");
						subName.append(findData.name);
						callback(subName.c_str(), findData.attrib);
					}
				}
				while (gEnv->pDrxPak->FindNext(handle, &findData) >= 0);
			}
		}
	}
}
} // FileUtils
} // Schematyc
