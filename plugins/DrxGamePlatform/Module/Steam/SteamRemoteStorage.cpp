#include "StdAfx.h"

<DinrusX/steam/steam_api.h>

#include "SteamRemoteStorage.h"
#include "SteamRemoteFile.h"
#include "SteamSharedRemoteFile.h"

#include "UserGeneratedContentUpr.h"

namespace Drx
{
	namespace GamePlatform
	{
		namespace Steam
		{
			CRemoteStorage::CRemoteStorage()
			{
				m_pUGCUpr = stl::make_unique<CUserGeneratedContentUpr>();
			}

			bool CRemoteStorage::IsEnabled() const
			{
				if (ISteamRemoteStorage* pSteamRemoteStorage = SteamRemoteStorage())
					return pSteamRemoteStorage->IsCloudEnabledForApp();

				return false;
			}

			std::shared_ptr<IRemoteFile> CRemoteStorage::GetFile(const char* name)
			{
				if (ISteamRemoteStorage* pSteamRemoteStorage = SteamRemoteStorage())
				{
					if (!IsEnabled())
						return nullptr;

					return std::make_shared<CRemoteFile>(name);
				}

				return nullptr;
			}

			std::shared_ptr<ISharedRemoteFile> CRemoteStorage::GetSharedFile(ISharedRemoteFile::Identifier id)
			{
				if (id == 0)
					return nullptr;

				return std::make_shared<CSharedRemoteFile>(id);
			}
		}
	}
}