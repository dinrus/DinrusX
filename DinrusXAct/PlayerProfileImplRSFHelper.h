// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __PLAYERPROFILEIMPLFSF_H__
#define __PLAYERPROFILEIMPLFSF_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include "PlayerProfileImplFS.h"

class CRichSaveGameHelper
{
public:
	CRichSaveGameHelper(ICommonProfileImpl* pImpl) : m_pImpl(pImpl) {}
	bool       GetSaveGames(CPlayerProfileUpr::SUserEntry* pEntry, CPlayerProfileUpr::TSaveGameInfoVec& outVec, const char* profileName);
	ISaveGame* CreateSaveGame(CPlayerProfileUpr::SUserEntry* pEntry);
	ILoadGame* CreateLoadGame(CPlayerProfileUpr::SUserEntry* pEntry);
	bool       DeleteSaveGame(CPlayerProfileUpr::SUserEntry* pEntry, const char* name);
	bool       GetSaveGameThumbnail(CPlayerProfileUpr::SUserEntry* pEntry, const char* saveGameName, CPlayerProfileUpr::SThumbnail& thumbnail);
	bool       MoveSaveGames(CPlayerProfileUpr::SUserEntry* pEntry, const char* oldProfileName, const char* newProfileName);

protected:
	bool FetchMetaData(XmlNodeRef& root, CPlayerProfileUpr::SSaveGameMetaData& metaData);
	ICommonProfileImpl* m_pImpl;
};

#endif
