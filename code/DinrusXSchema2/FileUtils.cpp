// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema2/StdAfx.h>

#include <DinrusX/DinrusXSchema2/FileUtils.h>

#include <DinrusX/DinrusXSys/IDrxPak.h>
#include <DinrusX/DinrusXSchema2/ILog.h>

namespace Schematyc2
{
	namespace FileUtils
	{
		//////////////////////////////////////////////////////////////////////////
		void EnumFilesInFolder(const char* szFolderName, const char* szExtension, FileEnumCallback callback, EFileEnumFlags flags)
		{
			LOADING_TIME_PROFILE_SECTION_ARGS(szFolderName);
			SCHEMATYC2_SYSTEM_ASSERT(szFolderName);
			if(szFolderName)
			{
				SCHEMATYC2_SYSTEM_ASSERT(callback);
				if(callback)
				{
					if((flags & EFileEnumFlags::Recursive) != 0)
					{
						stack_string	searchPath = szFolderName;
						searchPath.append("/*.*");
						_finddata_t	findData;
						intptr_t		handle = gEnv->pDrxPak->FindFirst(searchPath.c_str(), &findData);
						if(handle >= 0)
						{
							do
							{
								if((findData.name[0] != '.') && ((findData.attrib & _A_SUBDIR) != 0))
								{
									if(((flags & EFileEnumFlags::IgnoreUnderscoredFolders) == 0) || (findData.name[0] != '_'))
									{
										stack_string	subName = szFolderName;
										subName.append("/");
										subName.append(findData.name);
										EnumFilesInFolder(subName.c_str(), szExtension, callback, flags);
									}
								}
							} while(gEnv->pDrxPak->FindNext(handle, &findData) >= 0);
						}
					}
					stack_string	searchPath = szFolderName;
					searchPath.append("/");
					searchPath.append(szExtension ? szExtension : "*.*");
					_finddata_t	findData;
					intptr_t		handle = gEnv->pDrxPak->FindFirst(searchPath.c_str(), &findData);
					if(handle >= 0)
					{
						do
						{
							if((findData.name[0] != '.') && ((findData.attrib & _A_SUBDIR) == 0))
							{
								stack_string	subName = szFolderName;
								subName.append("/");
								subName.append(findData.name);
								callback(subName.c_str(), findData.attrib);
							}
						} while(gEnv->pDrxPak->FindNext(handle, &findData) >= 0);
					}
				}
			}
		}
	}
}
