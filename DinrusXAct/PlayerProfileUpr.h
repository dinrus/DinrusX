// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __PLAYERPROFILEMANAGER_H__
#define __PLAYERPROFILEMANAGER_H__

#if _MSC_VER > 1000
	#pragma once
#endif

#include <DinrusX/DinrusXAct/IPlayerProfiles.h>
#include <DinrusX/CoreX/DrxCrc32.h>

class CPlayerProfile;
struct ISaveGame;
struct ILoadGame;

#if defined(_DEBUG)
	#define PROFILE_DEBUG_COMMANDS 1
#else
	#define PROFILE_DEBUG_COMMANDS 0
#endif

class CPlayerProfileUpr : public IPlayerProfileUpr
{
public:
	struct IPlatformImpl;

	// profile description
	struct SLocalProfileInfo
	{
		SLocalProfileInfo()
			: m_lastLoginTime(0)
		{
			SetName(PLAYER_DEFAULT_PROFILE_NAME);
		}
		SLocalProfileInfo(const string& name)
			: m_lastLoginTime(0)
		{
			SetName(name);
		}
		SLocalProfileInfo(const char* name)
			: m_lastLoginTime(0)
		{
			SetName(name);
		}

		void SetName(const char* name)
		{
			m_name = name;
		}

		void SetName(const string& name)
		{
			m_name = name;
		}
		void SetLastLoginTime(time_t lastLoginTime)
		{
			m_lastLoginTime = lastLoginTime;
		}

		const string& GetName() const
		{
			return m_name;
		}

		const time_t& GetLastLoginTime() const
		{
			return m_lastLoginTime;
		}

		int compare(const char* name) const
		{
			return m_name.compareNoCase(name);
		}

	private:
		string m_name; // name of the profile
		time_t m_lastLoginTime;
	};

	// per user data
	struct SUserEntry
	{
		SUserEntry(const string& inUserId) : userId(inUserId), pCurrentProfile(0), pCurrentPreviewProfile(0), userIndex(0), unknownUser(false) {}
		string                         userId;
		CPlayerProfile*                pCurrentProfile;
		CPlayerProfile*                pCurrentPreviewProfile;
		unsigned int                   userIndex;
		bool                           unknownUser;
		std::vector<SLocalProfileInfo> profileDesc;
	};
	typedef std::vector<SUserEntry*> TUserVec;

	struct SSaveGameMetaData
	{
		SSaveGameMetaData()
		{
			levelName = gameRules = buildVersion = SAVEGAME_LEVEL_NAME_UNDEFINED;
			saveTime = 0;
			loadTime = 0;
			fileVersion = -1;
		}
		void CopyTo(ISaveGameEnumerator::SGameMetaData& data)
		{
			data.levelName = levelName;
			data.gameRules = gameRules;
			data.fileVersion = fileVersion;
			data.buildVersion = buildVersion;
			data.saveTime = saveTime;
			data.loadTime = loadTime;
			data.xmlMetaDataNode = xmlMetaDataNode;
		}
		string     levelName;
		string     gameRules;
		int        fileVersion;
		string     buildVersion;
		time_t     saveTime; // original time of save
		time_t     loadTime; // most recent time loaded, from file modified timestamp
		XmlNodeRef xmlMetaDataNode;
	};

	struct SThumbnail
	{
		SThumbnail() : width(0), height(0), depth(0) {}
		DynArray<uint8> data;
		int             width;
		int             height;
		int             depth;
		bool IsValid() const { return data.size() && width && height && depth; }
		void ReleaseData()
		{
			data.clear();
			width = height = depth = 0;
		}
	};

	struct SSaveGameInfo
	{
		string            name;
		string            humanName;
		string            description;
		SSaveGameMetaData metaData;
		SThumbnail        thumbnail;
		void              CopyTo(ISaveGameEnumerator::SGameDescription& desc)
		{
			desc.name = name;
			desc.humanName = humanName;
			desc.description = description;
			metaData.CopyTo(desc.metaData);
		}
	};
	typedef std::vector<SSaveGameInfo> TSaveGameInfoVec;

	enum EPlayerProfileSection
	{
		ePPS_Attribute = 0,
		ePPS_Actionmap,
		ePPS_Num  // Last
	};

	// members

	CPlayerProfileUpr(CPlayerProfileUpr::IPlatformImpl* pImpl); // CPlayerProfileUpr takes ownership of IPlatformImpl*
	virtual ~CPlayerProfileUpr();

	// IPlayerProfileUpr
	virtual bool                  Initialize();
	virtual bool                  Shutdown();
	virtual int                   GetUserCount();
	virtual bool                  GetUserInfo(int index, IPlayerProfileUpr::SUserInfo& outInfo);
	virtual const char*           GetCurrentUser();
	virtual int                   GetCurrentUserIndex();
	virtual void                  SetExclusiveControllerDeviceIndex(unsigned int exclusiveDeviceIndex);
	virtual unsigned int          GetExclusiveControllerDeviceIndex() const;
	virtual bool                  LoginUser(const char* userId, bool& bOutFirstTime);
	virtual bool                  LogoutUser(const char* userId);
	virtual int                   GetProfileCount(const char* userId);
	virtual bool                  GetProfileInfo(const char* userId, int index, IPlayerProfileUpr::SProfileDescription& outInfo);
	virtual bool                  CreateProfile(const char* userId, const char* profileName, bool bOverrideIfPresent, IPlayerProfileUpr::EProfileOperationResult& result);
	virtual bool                  DeleteProfile(const char* userId, const char* profileName, IPlayerProfileUpr::EProfileOperationResult& result);
	virtual bool                  RenameProfile(const char* userId, const char* newName, IPlayerProfileUpr::EProfileOperationResult& result);
	virtual bool                  SaveProfile(const char* userId, IPlayerProfileUpr::EProfileOperationResult& result, unsigned int reason);
	virtual bool                  SaveInactiveProfile(const char* userId, const char* profileName, EProfileOperationResult& result, unsigned int reason);
	virtual bool                  IsLoadingProfile() const;
	virtual bool                  IsSavingProfile() const;
	virtual IPlayerProfile*       ActivateProfile(const char* userId, const char* profileName);
	virtual IPlayerProfile*       GetCurrentProfile(const char* userId);
	virtual bool                  ResetProfile(const char* userId);
	virtual void                  ReloadProfile(IPlayerProfile* pProfile, unsigned int reason);
	virtual IPlayerProfile*       GetDefaultProfile();
	virtual const IPlayerProfile* PreviewProfile(const char* userId, const char* profileName);
	virtual void                  SetSharedSaveGameFolder(const char* sharedSaveGameFolder);
	virtual const char*           GetSharedSaveGameFolder()
	{
		return m_sharedSaveGameFolder.c_str();
	}
	virtual void   GetMemoryStatistics(IDrxSizer* s);

	virtual void   AddListener(IPlayerProfileListener* pListener, bool updateNow);
	virtual void   RemoveListener(IPlayerProfileListener* pListener);
	virtual void   AddOnlineAttributesListener(IOnlineAttributesListener* pListener);
	virtual void   RemoveOnlineAttributesListener(IOnlineAttributesListener* pListener);
	virtual void   EnableOnlineAttributes(bool enable);
	virtual bool   HasEnabledOnlineAttributes() const;
	virtual bool   CanProcessOnlineAttributes() const;
	virtual void   SetCanProcessOnlineAttributes(bool enable);
	virtual bool   RegisterOnlineAttributes();
	virtual void   GetOnlineAttributesState(const IOnlineAttributesListener::EEvent event, IOnlineAttributesListener::EState& state) const;
	virtual bool   IsOnlineOnlyAttribute(const char* name);
	virtual void   LoadOnlineAttributes(IPlayerProfile* pProfile);
	virtual void   SaveOnlineAttributes(IPlayerProfile* pProfile);
	virtual int    GetOnlineAttributesVersion() const;
	virtual int    GetOnlineAttributeIndexByName(const char* name);
	virtual void   GetOnlineAttributesDataFormat(SDrxLobbyUserData* pData, uint32 numData);
	virtual uint32 GetOnlineAttributeCount();
	virtual void   ClearOnlineAttributes();
	virtual void   SetProfileLastLoginTime(const char* userId, int index, time_t lastLoginTime);
	// ~IPlayerProfileUpr

	void LoadGamerProfileDefaults(IPlayerProfile* pProfile);

	// maybe move to IPlayerProfileUpr i/f
	virtual ISaveGameEnumeratorPtr CreateSaveGameEnumerator(const char* userId, CPlayerProfile* pProfile);
	virtual ISaveGame*             CreateSaveGame(const char* userId, CPlayerProfile* pProfile);
	virtual ILoadGame*             CreateLoadGame(const char* userId, CPlayerProfile* pProfile);
	virtual bool                   DeleteSaveGame(const char* userId, CPlayerProfile* pProfile, const char* name);

	virtual ILevelRotationFile*    GetLevelRotationFile(const char* userId, CPlayerProfile* pProfile, const char* name);

	virtual bool                   ResolveAttributeBlock(const char* userId, const char* attrBlockName, IResolveAttributeBlockListener* pListener, int reason);
	virtual bool                   ResolveAttributeBlock(const char* userId, const SResolveAttributeRequest& attrBlockNameRequest, IResolveAttributeBlockListener* pListener, int reason);

	const string&                  GetSharedSaveGameFolder() const
	{
		return m_sharedSaveGameFolder;
	}

	bool IsSaveGameFolderShared() const
	{
#if DRX_PLATFORM_DESKTOP
		return m_sharedSaveGameFolder.empty() == false;
#else
		// TCR/TRC - don't use profile name in file names.
		// Also, console saves are already keyed to a user profile.
		return false;
#endif
	}

	ILINE CPlayerProfile* GetDefaultCPlayerProfile() const
	{
		return m_pDefaultProfile;
	}

	// helper to move file or directory -> should eventually go into DinrusXSys/DrxPak
	// only implemented for Windows
	bool           MoveFileHelper(const char* existingFileName, const char* newFileName);

	virtual void   SetOnlineAttributes(IPlayerProfile* pProfile, const SDrxLobbyUserData* pData, const int32 onlineDataCount);
	virtual uint32 GetOnlineAttributes(SDrxLobbyUserData* pData, uint32 numData);
	virtual uint32 GetDefaultOnlineAttributes(SDrxLobbyUserData* pData, uint32 numData);

	void           ApplyChecksums(SDrxLobbyUserData* pData, uint32 numData);
	bool           ValidChecksums(const SDrxLobbyUserData* pData, uint32 numData);

public:
	struct IProfileXMLSerializer
	{
		virtual ~IProfileXMLSerializer(){}
		virtual bool       IsLoading() = 0;

		virtual void       SetSection(CPlayerProfileUpr::EPlayerProfileSection section, XmlNodeRef& node) = 0;
		virtual XmlNodeRef CreateNewSection(CPlayerProfileUpr::EPlayerProfileSection section, const char* name) = 0;
		virtual XmlNodeRef GetSection(CPlayerProfileUpr::EPlayerProfileSection section) = 0;

		/*
		   virtual XmlNodeRef AddSection(const char* name) = 0;
		   virtual XmlNodeRef GetSection(const char* name) = 0;
		 */
	};

	struct IPlatformImpl
	{
		typedef CPlayerProfileUpr::SUserEntry               SUserEntry;
		typedef CPlayerProfileUpr::SLocalProfileInfo        SLocalProfileInfo;
		typedef CPlayerProfileUpr::SThumbnail               SThumbnail;
		typedef CPlayerProfileUpr::SResolveAttributeRequest SResolveAttributeRequest;
		virtual ~IPlatformImpl(){}
		virtual bool                Initialize(CPlayerProfileUpr* pMgr) = 0;
		virtual void                Release() = 0; // must delete itself
		virtual bool                LoginUser(SUserEntry* pEntry) = 0;
		virtual bool                LogoutUser(SUserEntry* pEntry) = 0;
		virtual bool                SaveProfile(SUserEntry* pEntry, CPlayerProfile* pProfile, const char* name, bool initialSave = false, int reason = ePR_All) = 0;
		virtual bool                LoadProfile(SUserEntry* pEntry, CPlayerProfile* pProfile, const char* name) = 0;
		virtual bool                DeleteProfile(SUserEntry* pEntry, const char* name) = 0;
		virtual bool                RenameProfile(SUserEntry* pEntry, const char* newName) = 0;
		virtual bool                GetSaveGames(SUserEntry* pEntry, TSaveGameInfoVec& outVec, const char* altProfileName = "") = 0; // if altProfileName == "", uses current profile of SUserEntry
		virtual ISaveGame*          CreateSaveGame(SUserEntry* pEntry) = 0;
		virtual ILoadGame*          CreateLoadGame(SUserEntry* pEntry) = 0;
		virtual bool                DeleteSaveGame(SUserEntry* pEntry, const char* name) = 0;
		virtual ILevelRotationFile* GetLevelRotationFile(SUserEntry* pEntry, const char* name) = 0;
		virtual bool                GetSaveGameThumbnail(SUserEntry* pEntry, const char* saveGameName, SThumbnail& thumbnail) = 0;
		virtual void                GetMemoryStatistics(IDrxSizer* s) = 0;
		virtual bool                ResolveAttributeBlock(SUserEntry* pEntry, CPlayerProfile* pProfile, const char* attrBlockName, IResolveAttributeBlockListener* pListener, int reason)                            { return false; }
		virtual bool                ResolveAttributeBlock(SUserEntry* pEntry, CPlayerProfile* pProfile, const SResolveAttributeRequest& attrBlockNameRequest, IResolveAttributeBlockListener* pListener, int reason) { return false; }

	};

	SUserEntry* FindEntry(const char* userId) const;

protected:

	SUserEntry*        FindEntry(const char* userId, TUserVec::iterator& outIter);

	SLocalProfileInfo* GetLocalProfileInfo(SUserEntry* pEntry, const char* profileName) const;
	SLocalProfileInfo* GetLocalProfileInfo(SUserEntry* pEntry, const char* profileName, std::vector<SLocalProfileInfo>::iterator& outIter) const;

public:
	static int         sUseRichSaveGames;
	static int         sRSFDebugWrite;
	static int         sRSFDebugWriteOnLoad;
	static int         sLoadOnlineAttributes;

	static const char* FACTORY_DEFAULT_NAME;
	static const char* PLAYER_DEFAULT_PROFILE_NAME;

protected:
	bool        SetUserData(SDrxLobbyUserData* data, const TFlowInputData& value);
	bool        ReadUserData(const SDrxLobbyUserData* data, TFlowInputData& val);
	uint32      UserDataSize(const SDrxLobbyUserData* data);
	bool        SetUserDataType(SDrxLobbyUserData* data, const char* type);
	void        GetDefaultValue(const char* type, XmlNodeRef attributeNode, SDrxLobbyUserData* pOutData);

	bool        RegisterOnlineAttribute(const char* name, const char* type, const bool onlineOnly, const SDrxLobbyUserData& defaultValue, CCrc32& crc);
	void        ReadOnlineAttributes(IPlayerProfile* pProfile, const SDrxLobbyUserData* pData, const uint32 numData);
	static void ReadUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, SDrxLobbyUserData* pData, uint32 numData, void* pArg);
	static void RegisterUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);
	static void WriteUserDataCallback(DrxLobbyTaskID taskID, EDrxLobbyError error, void* pArg);

	void        SetOnlineAttributesState(const IOnlineAttributesListener::EEvent event, const IOnlineAttributesListener::EState newState);
	void        SendOnlineAttributeState(const IOnlineAttributesListener::EEvent event, const IOnlineAttributesListener::EState newState);

	//Online attributes use checksums to check the validity of data loaded
	int  Checksum(const int checksum, const SDrxLobbyUserData* pData, uint32 numData);
	int  ChecksumHash(const int hash, const int value) const;
	int  ChecksumHash(const int value0, const int value1, const int value2, const int value3, const int value4) const;
	int  ChecksumConvertValueToInt(const SDrxLobbyUserData* pData);

	bool LoadProfile(SUserEntry* pEntry, CPlayerProfile* pProfile, const char* name, bool bPreview = false);

	const static int k_maxOnlineDataCount = 1500;
	const static int k_maxOnlineDataBytes = 2976; //reduced from 3000 for encryption
	const static int k_onlineChecksums = 2;

#if PROFILE_DEBUG_COMMANDS
	static void DbgLoadOnlineAttributes(IConsoleCmdArgs* args);
	static void DbgSaveOnlineAttributes(IConsoleCmdArgs* args);
	static void DbgTestOnlineAttributes(IConsoleCmdArgs* args);

	static void DbgLoadProfile(IConsoleCmdArgs* args);
	static void DbgSaveProfile(IConsoleCmdArgs* args);

	SDrxLobbyUserData m_testData[k_maxOnlineDataCount];
	int               m_testFlowData[k_maxOnlineDataCount];
	int               m_testingPhase;
#endif

	typedef VectorMap<string, uint32>            TOnlineAttributeMap;
	typedef std::vector<IPlayerProfileListener*> TListeners;

	SDrxLobbyUserData                 m_defaultOnlineData[k_maxOnlineDataCount];
	SDrxLobbyUserData                 m_onlineData[k_maxOnlineDataCount];
	bool                              m_onlineOnlyData[k_maxOnlineDataCount];
	TOnlineAttributeMap               m_onlineAttributeMap;
	TListeners                        m_listeners;
	TUserVec                          m_userVec;
	string                            m_curUserID;
	string                            m_sharedSaveGameFolder;
	int                               m_curUserIndex;
	unsigned int                      m_exclusiveControllerDeviceIndex;
	uint32                            m_onlineDataCount;
	uint32                            m_onlineDataByteCount;
	uint32                            m_onlineAttributeAutoGeneratedVersion;
	IPlatformImpl*                    m_pImpl;
	CPlayerProfile*                   m_pDefaultProfile;
	IPlayerProfile*                   m_pReadingProfile;
	IOnlineAttributesListener::EState m_onlineAttributesState[IOnlineAttributesListener::eOAE_Max];
	IOnlineAttributesListener*        m_onlineAttributesListener;
	int                               m_onlineAttributeDefinedVersion;
	DrxLobbyTaskID                    m_lobbyTaskId;
	bool                              m_registered;
	bool                              m_bInitialized;
	bool                              m_enableOnlineAttributes;
	bool                              m_allowedToProcessOnlineAttributes;
	bool                              m_loadingProfile;
	bool                              m_savingProfile;
};

#endif
