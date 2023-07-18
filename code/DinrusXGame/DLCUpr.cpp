// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Responsible for loading downloadable content into the game
and informing the game of what content is loaded.

-------------------------------------------------------------------------
History:
- 18:05:2010  : Created by Martin Sherburn
- 21:08:2012  : Maintained by Andrew Blackwell

*************************************************************************/

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/UI/WarningsUpr.h>
#include <DinrusX/DinrusXGame/GameCVars.h>
#include <DinrusX/DinrusXGame/Network/Lobby/GameLobby.h>
#include <DinrusX/DinrusXGame/Network/Squad/SquadUpr.h>

#include <DinrusX/DinrusXGame/PlayerProgression.h>

#include <ILevelSystem.h>
#include <DinrusX/CoreX/TypeInfo_impl.h>
#include <GameXmlParamReader.h>

#include <DinrusX/DinrusXGame/DLCUpr.h>
#include <DinrusX/DinrusXGame/PlaylistUpr.h>
#include <DinrusX/CoreX/String/UnicodeFunctions.h>

static AUTOENUM_BUILDNAMEARRAY(s_standardLevelNames, NOTDLCLevelList);
static AUTOENUM_BUILDNAMEARRAY(s_standardGameModes, NOTDLCGameModeList);

#define MAX_VERSION_STRING 64

#define PRESALE_ID_OFFSET 8
#define CLIENT_SET_ENTITLEMENT_OFFSET 5

//Presale and digi pack flags (reference shipped xmls for contents)
#define PRESALE_DLC_FLAGS 0xFF00 // second 8 bits

#define STANDARD_DLC_FLAGS 0xFF	// lowest 8 bits

#define DLC_WARN_LOWEST_PRIO 10


const int k_optin_ent_index = 5;
const int k_demo_ent_index = 6;
const int k_alpha_ent_index = 7;

int nInitedPresales = 8;
const char* k_entitlementTags[] =
{
	"C3:Preorder_Stalker",
	"C3:Preorder_Overkill",
	"C3:Preorder_Predator",
	"C3:Hunter",
	"C3_EGC_Unlocks",
	"C3:OPTIN:REWARD",
};

//not sure whether our actual dlc will end up being id 0 or 1 yet
const char* k_dlcEntitlementTags[] =
{
	"C3:DLC1",
	"C3:DLC1"
};

CDLCUpr::CDLCUpr()
	: m_loadedDLCs(0)
	, m_allowedDLCs(0)
	, m_entitledDLCs(0)
	, m_warningPriority(DLC_WARN_LOWEST_PRIO)
	, m_requiredDLCs(0)
	, m_dlcLoaded(false)
	, m_allowedDLCUpToDate(false)
	, m_allowedDLCCheckFailed(false)
	, m_onlineAttributesRead(false)
	, m_bContentRemoved(false)
	, m_bContentAvailable(true)
	, m_appliedDLCStat(0)
	, m_DLCXPToAward(0)
	, m_bNewDLCAdded(false)
	, m_queueEventEntitlement(false)
	, m_entitlementTask(DrxLobbyInvalidTaskID)
{
	gEnv->pSystem->GetPlatformOS()->AddListener(this, "CDLCUpr");

#if defined(DLC_LOAD_ON_CONSTRUCTION)
	LoadDownloadableContent( 0 );
#endif
}

CDLCUpr::~CDLCUpr()
{
	gEnv->pSystem->GetPlatformOS()->RemoveListener(this);
}

void CDLCUpr::LoadDownloadableContent( uint32 userIdOveride /*= INVALID_CONTROLLER_INDEX*/ )
{
	if( m_dlcLoaded || !m_bContentAvailable )
	{
		return;
	}

	const ICmdLineArg *pNoDLCArg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "nodlc");
	if (pNoDLCArg != NULL)
	{
		return;
	}

// SECRET
	uint8 keyData[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// SECRET

	uint32 userIndex;
	
	if( userIdOveride != INVALID_CONTROLLER_INDEX )
	{
		userIndex = userIdOveride;
	}
	else
	{
		IPlayerProfileUpr *pPlayerProfileUpr = gEnv->pGame->GetIGameFramework()->GetIPlayerProfileUpr();
		userIndex = pPlayerProfileUpr ? pPlayerProfileUpr->GetExclusiveControllerDeviceIndex() : INVALID_CONTROLLER_INDEX;
	}

	if( userIndex != INVALID_CONTROLLER_INDEX )
	{
		gEnv->pSystem->GetPlatformOS()->MountDLCContent(this, userIndex, keyData);
		m_dlcLoaded = true;
	}
}

void CDLCUpr::DisableDownloadableContent()
{
	m_dlcLoaded = false;
	m_allowedDLCs = 0;
	m_allowedDLCUpToDate = false;
	m_allowedDLCCheckFailed = false;
	m_onlineAttributesRead = false;

	m_dlcWarning.clear();
	m_warningPriority = DLC_WARN_LOWEST_PRIO;
}

void CDLCUpr::OnPlatformEvent(const IPlatformOS::SPlatformEvent& event)
{
	switch(event.m_eEventType)
	{
		case IPlatformOS::SPlatformEvent::eET_ContentInstalled:
		{
			break;
		}

		case IPlatformOS::SPlatformEvent::eET_ContentRemoved:
		{
			OnDLCRemoved(event.m_uParams.m_contentRemoved.m_root);
			m_bContentAvailable = false;

			CErrorHandling *pErrorHandling = CErrorHandling::GetInstance();
			if (pErrorHandling)
			{
				pErrorHandling->OnFatalError(CErrorHandling::eFE_ContentRemoved);
			}
			break;
		}
	}
}

void CDLCUpr::OnDLCMounted(const XmlNodeRef &rootNode, const char* sDLCRootFolder)
{
	DrxLog( "OnDLCMounted: '%s'", sDLCRootFolder);
	XmlString minVersion;
	XmlString sName;
	int dlcId;
	if (rootNode->getAttr("minversion", minVersion) &&
			rootNode->getAttr("name", sName) &&
			rootNode->getAttr("id", dlcId))
	{
		DrxLog( "DLC Name = %s, ID = %d", sName.c_str(), dlcId );

		if (dlcId	>= 0 && dlcId < MAX_DLC_COUNT)
		{
#if (! ENTITLEMENTS_AUTHORATIVE) || defined(DEDICATED_SERVER)
			//whenever we load a dlc, it is automatically allowed
			m_allowedDLCs |= BIT(dlcId);
#endif
			if (!IsDLCReallyLoaded(dlcId))
			{
				SFileVersion currentVersion = gEnv->pSystem->GetProductVersion();
				SFileVersion minimumVersion = SFileVersion(minVersion.c_str());
				if (currentVersion < minimumVersion)
				{
					char currentVersionString[MAX_VERSION_STRING];
					currentVersion.ToString(currentVersionString);
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Unable to load DLC \"%s\" because it requires version %s and current version is %s", sName.c_str(), minVersion.c_str(), currentVersionString);
					RequestDLCWarning("DLCVersionMismatch",2, true);
				}
				else
				{
					XmlNodeRef crcNode = rootNode->findChild("crcs");

					PopulateDLCContents(rootNode, dlcId, sName.c_str() );

					//insist that CRCs are present and level folders match listed contents
					if ( !crcNode || !VerifyCRCs(crcNode, sDLCRootFolder) || !CheckLevels( dlcId, sDLCRootFolder ) )
					{
						ClearDLCContents( dlcId );
						RequestDLCWarning("DLCFileCorrupt",4, true);
						DrxLog("DLC \"%s\" not loaded successfully", sName.c_str());
					}
					else
					{
						DrxLog("DLC \"%s\" loaded successfully", sName.c_str());
						m_loadedDLCs |= BIT(dlcId);

						m_dlcContents[ dlcId ].root.Format( "%s", sDLCRootFolder );
				
						XmlNodeRef unlocksXml = rootNode->findChild("Unlocks");
						if(unlocksXml)
						{
							DoDLCUnlocks( unlocksXml, dlcId);
						}						

						DrxFixedStringT<IDrxPak::g_nMaxPath> path;

						//Level Extras pak contains things which need to be accessed relative to the Level Path
						//eg. Level meta data, icons and mini maps
						//also contains Level Names and Rich Presence mappings
						path.Format("%s/dlcLevelExtras.pak", sDLCRootFolder);
						DrxLog( "DLC: Opening %s as %s", path.c_str(), sDLCRootFolder );
						bool success = gEnv->pDrxPak->OpenPack( sDLCRootFolder, path );

						//Data pak contains things which need to be accessed relative to the Game Root
						//eg. Objects and Textures for new entities
						path.Format("%s/dlcData.pak", sDLCRootFolder);
						string gamePath = PathUtil::GetGameFolder();
						DrxLog( "DLC: Opening %s as %s", path.c_str(), gamePath.c_str() );
						success &= gEnv->pDrxPak->OpenPack( gamePath.c_str(), path );

						if (success == false)
						{
							DRX_ASSERT_MESSAGE(success, "Failed to open DLC packs");
							DrxLog("Failed to open DLC packs '%s'",path.c_str());
						}
						else
						{
							//Only DLCs with data paks can have strings or levels

							path.Format("%s/", sDLCRootFolder);
							DrxLog( "DLCUpr: Adding %s to Mod paths", path.c_str() );
							gEnv->pDrxPak->AddMod(path.c_str());

							//load string mappings for level names in this DLC
							path.Format( "%s/scripts/dlc%dnames.xml", sDLCRootFolder, dlcId );
							g_pGame->LoadMappedLevelNames( path.c_str() );

							//and load the actual localized strings
							ILocalizationUpr *pLocMan = GetISystem()->GetLocalizationUpr();
							path.Format( "%s/scripts/dlc%d%s.xml", sDLCRootFolder, dlcId, pLocMan->GetLanguage() );
							pLocMan->LoadExcelXmlSpreadsheet( path );

							//see if the pack has a description
							DrxFixedStringT<32> descriptionKey;
							descriptionKey.Format( "dlc%d_pack_description", dlcId );
							SLocalizedInfoGame		tempInfo;
							if( pLocMan->GetLocalizedInfoByKey( descriptionKey.c_str(), tempInfo ) )
							{
								m_dlcContents[ dlcId ].descriptionStr.Format( "@%s", descriptionKey.c_str() );
							}

							//and load the Rich Presence mappings
							path.Format( "%s/scripts/dlc%dpresence.xml", sDLCRootFolder, dlcId );
							g_pGame->AddRichPresence( path.c_str() );

							//and get the Score Rewards Path
							m_dlcContents[ dlcId ].scoreRewardsPath.Format( "%s/scripts/dlc%drewards.xml", sDLCRootFolder, dlcId );

							//and the Playlists Path
							m_dlcContents[ dlcId ].playlistsPath.Format( "%s/scripts/dlc%dplaylists", sDLCRootFolder, dlcId );

							ILevelSystem *pLevelSystem = g_pGame->GetIGameFramework()->GetILevelSystem();
							path.Format("%s/levels", sDLCRootFolder);
							DrxLog("DLC Levelsystem rescan '%s'", path.c_str());
							const uint32 dlcTag = 'DLC0';
							pLevelSystem->Rescan(path.c_str(), dlcTag);
						}
					}
				}
			}
			else
			{
				DrxLog("DLC %d already loaded, OK if from re-sign in", dlcId );
			}
		}
		else
		{
			DRX_ASSERT_MESSAGE(false, "DLC id is not within range");
		}
	}
	else
	{
		RequestDLCWarning("DLCXmlError",4, true);
	}
}



void CDLCUpr::OnDLCMountFailed(IPlatformOS::EDLCMountFail reason)
{
	DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Unable to load DLC, error code: %d", reason);
	const char* sWarning = NULL;
	switch (reason)
	{
	case IPlatformOS::eDMF_FileCorrupt:
		sWarning = "DLCFileCorrupt";
		break;
	case IPlatformOS::eDMF_DiskCorrupt:
		sWarning = "DLCDiskCorrupt";
		break;
	case IPlatformOS::eDMF_XmlError:
		sWarning = "DLCXmlError";
		break;
	}
	if (sWarning)
	{
		RequestDLCWarning(sWarning, 4, true);
	}
	else
	{
		DRX_ASSERT_MESSAGE(false, "Unrecognised DLC error");
	}
}

void CDLCUpr::OnDLCMountFinished(int nPacksFound)
{
	DrxLog( "OnDLCMountFinished nPacksFound:%d", nPacksFound);

	DrxLog( "DLC: Loaded DLCs flags are 0x%x", GetLoadedDLCs() );

	if( nPacksFound > 0 )
	{
		//we should rescan for any levels added by the DLCs
		ILevelSystem *pLevelSystem = g_pGame->GetIGameFramework()->GetILevelSystem();
		pLevelSystem->Rescan("levels", ILevelSystem::TAG_MAIN);
	}
	
#if ! ENTITLEMENTS_AUTHORATIVE
	//on consoles, after DLC is loaded, we know about what DLC is allowed
	m_allowedDLCUpToDate = true;
#endif

}

void CDLCUpr::OnDLCRemoved(const char* sDLCRootFolder)
{
	//clear all the data
	for( int iDLC = 0; iDLC < MAX_DLC_COUNT; iDLC++ )
	{
		if( IsDLCLoaded( iDLC ) )
		{
			if( strcmpi( m_dlcContents[iDLC].root.c_str(), sDLCRootFolder ) == 0 )
			{
				m_loadedDLCs &= ~BIT(iDLC);
				m_allowedDLCs &= ~BIT(iDLC);

				//close the paks
				DrxFixedStringT<IDrxPak::g_nMaxPath> path;

				path.Format("%s/dlcLevelExtras.pak", sDLCRootFolder);
				DrxLog( "DLC: Closing %s", path.c_str() );
				gEnv->pDrxPak->ClosePack( path.c_str() );

				path.Format("%s/dlcData.pak", sDLCRootFolder);
				DrxLog( "DLC: Closing %s", path.c_str() );
				gEnv->pDrxPak->ClosePack( path.c_str() );
			}

		}
	}
}

bool CDLCUpr::VerifyCRCs(const XmlNodeRef &crcNode, const char* sDLCRootFolder)
{
	bool success = true;
	DrxFixedStringT<IDrxPak::g_nMaxPath> path;
	int numFiles = crcNode->getChildCount();
	XmlString fileName;
	uint32 storedCrc;
	for (int i=0; i<numFiles; ++i)
	{
		XmlNodeRef fileNode = crcNode->getChild(i);
		if (fileNode->getAttr("name", fileName) &&
			fileNode->getAttr("crc", storedCrc))
		{
			
#if DRX_PLATFORM_WINDOWS
			path.Format("%s/%s", sDLCRootFolder, fileName.c_str());
			bool useDrxFile = true;
#else
			DrxWarning( VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "No Platform defined in DLCUpr" );
			bool useDrxFile = false;
#endif
			DrxLog( "CRC: Checking CRC of %s", path.c_str() );

			success = gEnv->pDrxPak->OpenPack( path.c_str() );

			if( !success )
			{
					DrxLog( "CRC: Failed to open pack" );
			}

			uint32 computedCrc = gEnv->pDrxPak->ComputeCachedPakCDR_CRC( path.c_str(), useDrxFile );
			if (computedCrc != storedCrc)
			{
				DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CRC on file %s (%u) does not match stored value (%u)", path.c_str(), computedCrc, storedCrc);
				success = false;
			}
			gEnv->pDrxPak->ClosePack( path.c_str() );
		}
	}
	return success;
}

void CDLCUpr::PatchMenu(CMenuData& menu)
{
#if 0 // old frontend
	// This function is used to add additional elements to the menus that should be only available in the DLC
	// e.g. used to add additional levels and game modes
	for (int i=0; i<MAX_DLC_COUNT; ++i)
	{
		if (IsDLCLoaded(i))
		{
			// e.g. Replace file.xml with file_dlc0.xml
			DrxFixedStringT<32> newExtension;
			newExtension.Format("_dlc%d.xml", i);
			stack_string path = menu.GetPath();
			PathUtil::RemoveExtension(path);
			path.append(newExtension);
			XmlNodeRef xml = GetISystem()->LoadXmlFromFile(path);
			if(xml)
			{
				CGameXmlParamReader xmlReader(xml);

				const int childXmlNodeCount = xmlReader.GetUnfilteredChildCount();
				for(int childIndex = 0; childIndex < childXmlNodeCount; ++childIndex)
				{
					const XmlNodeRef childNode = xmlReader.GetFilteredChildAt(childIndex);
					if ((childNode != NULL) && !stricmp(childNode->getTag(), "Screen"))
					{
						const char* name = childNode->getAttr("name");
						int screenIndex = menu.GetScreen(name);
						if (screenIndex >= 0)
						{
							CMenuScreen *pScreen = menu.GetScreen(screenIndex);
							pScreen->Initialize(childNode);
						}
					}
				}
			}
		}
	}
#endif
}

void CDLCUpr::PopulateDLCContents(const XmlNodeRef &rootNode, int dlcId, const char* name )
{
	Unicode::Convert(m_dlcContents[dlcId].name, name);

	XmlNodeRef levelsNode = rootNode->findChild("levels");
	if (levelsNode)
	{
		XmlString levelName;
		int numLevels = levelsNode->getChildCount();

		DrxLog( "Found %d levels in the DLC", numLevels );
		
		m_dlcContents[dlcId].levels.reserve(numLevels);
		for (int i=0; i<numLevels; ++i)
		{
			XmlNodeRef levelNode = levelsNode->getChild(i);
			if (levelNode->getAttr("name", levelName))
			{
				DrxLog( "Found level %s and added to the DLC manager", levelName.c_str() );
				m_dlcContents[dlcId].levels.push_back(levelName);
			}
		}
	}

	XmlNodeRef bonusNode = rootNode->findChild("bonus");
	if( bonusNode )
	{
		DrxLog( "DLC pak includes a pre-sale bonus" );
		uint32 bonusID = 0;
		bonusNode->getAttr("id", bonusID );
		m_dlcContents[dlcId].bonusID = bonusID;
	}

	XmlNodeRef uniqueIdNode = rootNode->findChild("uniqueId");
	if( uniqueIdNode )
	{
		uint32 uniqueID = 0;
		uniqueIdNode->getAttr("id", uniqueID );
		m_dlcContents[dlcId].uniqueID = uniqueID;
	}

	XmlNodeRef uniqueTagNode = rootNode->findChild("uniqueTag");
	if( uniqueTagNode )
	{
		const char* str =	uniqueTagNode->getAttr( "tag" );
		m_dlcContents[dlcId].uniqueTag.Format( str );
	}
}

void CDLCUpr::ClearDLCContents( int dlcId )
{
	wcscpy( m_dlcContents[dlcId].name, L"" );
	m_dlcContents[dlcId].levels.clear();
	m_dlcContents[dlcId].bonusID = 0;
}

uint32 CDLCUpr::GetRequiredDLCs()
{
	stack_string sCurrentLevelName;
	CGameLobby* pGameLobby = g_pGame->GetGameLobby();
	if (pGameLobby)
	{
		// This is the map currently selected in the game lobby
		sCurrentLevelName = pGameLobby->GetCurrentLevelName();
	}
	else
	{
		// This is the fallback if the lobby can't be found
		sCurrentLevelName = gEnv->pConsole->GetCVar("sv_map")->GetString();
	}
	if (strcmp(sCurrentLevelName.c_str(), m_prevLevelName.c_str()) != 0)
	{
		m_prevLevelName = sCurrentLevelName.c_str();
		m_requiredDLCs = GetRequiredDLCsForLevel(sCurrentLevelName.c_str());
	}
	return m_requiredDLCs;
}

uint32 CDLCUpr::GetRequiredDLCsForLevel(const char* pLevelName)
{
	if (pLevelName[0] == 0)
	{
		// Server hasn't set a level yet, no dlc required as yet
		return 0;
	}

	int req = 0;

	// Strip off the first part from the level name (e.g. "Wars/")
	const char* pTrimmedLevelName = strrchr(pLevelName, '/');
	if (pTrimmedLevelName)
	{
		pTrimmedLevelName++;
	}
	else
	{
		pTrimmedLevelName = pLevelName;
	}

	for (int i=0; i<MAX_DLC_COUNT; ++i)
	{
		for (size_t j=0; j<m_dlcContents[i].levels.size(); ++j)
		{
			if (stricmp(m_dlcContents[i].levels[j].c_str(), pTrimmedLevelName) == 0)
			{
				req |= BIT(i);
			}
		}
	}

	//TODO: Add explioit prevention DLC level list here if needed


	if (req == 0 && !LevelExists(pLevelName))
	{
		// this means we know the level's DLC but we can't work out *which* DLC it needs
		// (probably due to this being called on a client without the requisite DLC) - so for sanity lets say it needs all of them!
		req = 0xFFffFFff;
	}

	return req;
}

bool CDLCUpr::LevelExists(const char* pLevelName)
{
	ILevelSystem *pLevelSystem = g_pGame->GetIGameFramework()->GetILevelSystem();
	return pLevelSystem && pLevelSystem->GetLevelInfo(pLevelName);
}

bool CDLCUpr::IsLevelStandard(const char * levelname)
{
#if defined(IS_EAAS)
	// You can test the level name here, and return false if the map should not be available
	return true;
#else
	// This function is currently only used for achievements and shouldn't be used as a robust way
	// to check if a level is DLC or not
	for ( int level = 0; level < eNOTDLC_NUM_LEVELS; level++ )
	{
		if ( stricmp(&s_standardLevelNames[level][8],levelname) == 0 )
		{
			return true;
		}
	}
	return false;
#endif
}

bool CDLCUpr::IsGameModeStandard(const char * gamemode)
{
#if defined(IS_EAAS)
	// You can test the game mode here, and return false if the game mode should not be available
	return true;
#else
	// This function is currently only used for achievements and shouldn't be used as a robust way
	// to check if a game mode is DLC or not
	for ( int gmidx = 0; gmidx < eNOTDLC_NUM_GAMEMODES; gmidx++ )
	{
		if ( stricmp(&s_standardGameModes[gmidx][8],gamemode) == 0 )
		{
			return true;
		}
	}
	return false;
#endif
}

uint32 CDLCUpr::GetSquadCommonDLCs()
{
	/* Squad User data is not yet functional
	CSquadUpr* pSquadUpr = g_pGame->GetSquadUpr();
	if (pSquadUpr)
	{
		uint32 commonDLCs;		
		if (pSquadUpr->GetSquadCommonDLCs(commonDLCs))
		{
			return commonDLCs;
		}
	}
	*/
	// Fall back to just the local loaded and allowed DLCs if it can't be obtained from the squad manager;
	return GetLoadedDLCs();
}

int CDLCUpr::GetNamesStringOfPlayersMissingDLCsForLevel(const char* pLevelName, stack_string* pPlayersString)
{
	CGameLobby*  pGameLobby = g_pGame->GetGameLobby();
	DRX_ASSERT(pGameLobby);
	DRX_ASSERT(pGameLobby->IsServer());

	int  count = 0;

	uint32  requiredDLCs = GetRequiredDLCsForLevel(pLevelName);

	const SSessionNames&  lobbySessNames = pGameLobby->GetSessionNames();

	const int  nameSize = lobbySessNames.Size();
	for (int i=0; i<nameSize; ++i)
	{
		const SSessionNames::SSessionName&  player = lobbySessNames.m_sessionNames[i];
		const uint32  loadedDLC = (uint32) player.m_userData[eLUD_LoadedDLCs];
		if (!MeetsDLCRequirements(requiredDLCs, loadedDLC))
		{
			DrxLog("CDLCUpr::GetNamesStringOfPlayersMissingDLCsForLevel: '%s' does not meet DLC requirements for level '%s'", player.m_name, pLevelName);
			count++;
			if (!pPlayersString->empty())
			{
				pPlayersString->append(", ");
			}
			pPlayersString->append(player.m_name);
		}
	}

	return count;
}

bool CDLCUpr::OnWarningReturn(THUDWarningId id, const char* returnValue)
{
	return true;
}

void CDLCUpr::OnlineAttributesRead()
{
	m_onlineAttributesRead = true;
}

bool CDLCUpr::IsNewDLC(const int index) const
{
	return ! (m_appliedDLCStat & ( BIT(index) ) );
}

void CDLCUpr::AddNewDLCApplied(const int index)
{
	DRX_ASSERT(IsNewDLC(index));
	m_appliedDLCStat |= ( BIT(index) );
}

void CDLCUpr::ActivatePreSaleBonuses( bool showPopup, bool fromSuitReboot /*= false*/ )
{
	m_DLCXPToAward = 0;

	bool anyNew = false;

	if( m_onlineAttributesRead && m_allowedDLCUpToDate )
	{
		if( CPlayerProgression *pPlayerProgression = CPlayerProgression::GetInstance() )
		{
			for( int iDLC = 0; iDLC < MAX_DLC_COUNT; ++iDLC )
			{
				if( IsDLCLoaded( iDLC ) )
				{
					bool isNew = IsNewDLC( iDLC );

					anyNew |= isNew;
					
					if( m_dlcContents[ iDLC].bonusID != 0 )
					{
						int xpFromThis = pPlayerProgression->UnlockPresale( m_dlcContents[ iDLC ].bonusID, showPopup, isNew || fromSuitReboot );

						if( isNew || fromSuitReboot )
						{
							m_DLCXPToAward += xpFromThis;
						}
					}

					if( isNew )
					{
						AddNewDLCApplied( iDLC );

						if( ! m_dlcContents[ iDLC ].descriptionStr.empty()  )
						{
							//show new unlocked DLC description
							const char* pDescription = CHUDUtils::LocalizeString( m_dlcContents[ iDLC ].descriptionStr.c_str() );

							g_pGame->GetWarnings()->AddGameWarning( "RedeemNewDLC", pDescription, this );

						}
					}
				}
			}
		}
	}
	else
	{
		DrxLog( "DLCWarning: Calling activate presale bonuses too soon" );
	}

}

bool CDLCUpr::CheckLevels( int dlcId, const char* sDLCRootFolder )
{
	//TODO: Enable on all platforms

	return true;
}

void CDLCUpr::RequestDLCWarning( const char* warningName, uint32 priority, bool alwaysDelay )
{
	if( gEnv->bMultiplayer && !alwaysDelay )
	{
		g_pGame->GetWarnings()->AddGameWarning( warningName, NULL );
	}
	else
	{
		//Check we don't already have a warning
		if( m_dlcWarning.empty() || priority < m_warningPriority )
		{
			m_dlcWarning.Format( warningName );
			m_warningPriority = priority;
		}
		else
		{
			DrxLog( "DLC: reporting warning %s but already have warning %s waiting, ignoring", warningName, m_dlcWarning.c_str() );
		}

	}
}

void CDLCUpr::ProcessDelayedWarnings()
{
	if( !m_dlcWarning.empty() )
	{
		g_pGame->GetWarnings()->AddGameWarning( m_dlcWarning.c_str(), NULL );

		m_dlcWarning.clear();
		m_warningPriority = DLC_WARN_LOWEST_PRIO;
	}
}

const char* CDLCUpr::ScoreRewardsFilename( const char* pLevelName )
{
	int dlcID = DlcIdForLevel( pLevelName );

	if( dlcID != -1 )
	{
		return m_dlcContents[ dlcID ].scoreRewardsPath.c_str();
	}
	
	return NULL;
}

int CDLCUpr::DlcIdForLevel( const char* pLevelName )
{
	int retVal = -1;
	// Strip off all directories from the level name (e.g. "Wars/")
	const char* pTrimmedLevelName = pLevelName;
	const char* pSlashPoint;
	while( (pSlashPoint = strrchr(pTrimmedLevelName, '/') ) != NULL )
	{
		pTrimmedLevelName = pSlashPoint+1;
	}

	for (int i=0; i<MAX_DLC_COUNT; ++i)
	{
		if (IsDLCReallyLoaded(i))	//probably ok to find out if a DLC we have installed is required, even if we don't own it
		{
			for (size_t j=0; j<m_dlcContents[i].levels.size(); ++j)
			{
				if (stricmp(m_dlcContents[i].levels[j].c_str(), pTrimmedLevelName) == 0)
				{
					DRX_ASSERT_MESSAGE( retVal == -1, "DLC level in multiple DLC packages" );
					retVal = i;
				}
			}
		}
	}

	return retVal;
}

void CDLCUpr::DoDLCUnlocks( XmlNodeRef unlocksXml, int dlcId )
{
	//handle any direct unlocks
	const int unlockCount = unlocksXml->getChildCount();

	for (int iUnlock = 0; iUnlock < unlockCount; ++iUnlock)
	{
		XmlNodeRef unlockNode = unlocksXml->getChild(iUnlock);
		SUnlock unlock(unlockNode, 0);
		unlock.Unlocked(true);
		m_itemUnlocks.push_back( unlock );
		m_itemUnlockDLCids.push_back( dlcId );

		DrxLog( "DLC: Found a dlc item unlock, %s", unlockNode->getAttr("name") );
	}
}

bool CDLCUpr::HaveUnlocked( EUnlockType type, const char* name, SPPHaveUnlockedQuery & results )
{
	results.exists = false;
	results.unlocked = false;

	for(unsigned int i = 0; i < m_itemUnlocks.size(); i++)
	{
		if( IsDLCLoaded(m_itemUnlockDLCids[i]) )
		{
			const SUnlock& unlock = m_itemUnlocks[i];
			if(unlock.m_type == type )
			{
				if(strcmpi(name, unlock.m_name) == 0)
				{
					DrxLog( "DLC: dlc has a node to unlock %s", name );
					results.available = true;
					results.exists = true;
					results.reason = eUR_None;
					if (unlock.m_unlocked)
					{
						results.unlocked = true;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void CDLCUpr::AddPlaylists()
{
	CPlaylistUpr* pPlaylistUpr = g_pGame->GetPlaylistUpr();

	if( pPlaylistUpr )
	{
		for( int dlcId = 0; dlcId < MAX_DLC_COUNT; dlcId++ )
		{
			if( IsDLCLoaded( dlcId) )
			{
				pPlaylistUpr->AddPlaylistsFromPath( m_dlcContents[ dlcId ].playlistsPath );
			}
		}
	}
}

bool CDLCUpr::IsEntitlementSet( int entitlementIndex, bool presale ) const
{
	const char* pTag = NULL;
	
	if( presale )
	{
		pTag = k_entitlementTags[ entitlementIndex ];
	}
	else
	{
		pTag = k_dlcEntitlementTags[ entitlementIndex ];
	}

	for( int dlcId = 0; dlcId < MAX_DLC_COUNT; dlcId++ )
	{
		if( ! m_dlcContents[ dlcId ].uniqueTag.empty() )
		{
			if( strcmp( m_dlcContents[ dlcId ].uniqueTag, pTag ) == 0 )
			{
				//this is the entitlement we are looking for
#if ENTITLEMENTS_AUTHORATIVE
				return IsDLCLoaded( dlcId );
#else
				return (m_entitledDLCs & BIT(dlcId)) != 0;
#endif
			}
		}
	}

	DrxLog( "CDLCUpr: No record of entitlement %d %s", entitlementIndex, pTag );
	return false;
}

int CDLCUpr::SetEntitlement( int entitlementIndex, bool presale )
{
	int dlcID = -1;

	// TODO: michiel

	return dlcID;
}

void CDLCUpr::Update()
{
	// TODO: michiel
}

bool CDLCUpr::IsPIIEntitlementSet() const
{
	return IsEntitlementSet(k_optin_ent_index, true);
}
