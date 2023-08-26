// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#if !DRX_PLATFORM
	#error DrxPlatformDefines.h is not included.
#endif

#if DRX_PLATFORM_WINDOWS && defined(DRX_ENABLE_RC_HELPER)

	#include "SettingsUprHelpers.h"

//! Manages storage and loading of all information for tools and DRXENGINE, by either registry or an INI file.
//! Information can be read and set by key-to-value functions.
//! Specific information can be set by a dialog application called by this class.
//! If the engine root path is not found, a fall-back dialog is opened.
class CEngineSettingsUpr
{
	friend class CResourceCompilerHelper;

public:
	//! Prepares CEngineSettingsUpr to get requested information either from registry or an INI file,
	//! if existent as a file with name an directory equal to the module, or from registry.
	CEngineSettingsUpr(const wchar_t* moduleName = NULL, const wchar_t* iniFileName = NULL);

	void RestoreDefaults();

	//! Stores/loads user specific information for modules to/from registry or INI file.
	bool GetModuleSpecificStringEntryUtf16(const char* key, SettingsUprHelpers::CWCharBuffer wbuffer);
	bool GetModuleSpecificStringEntryUtf8(const char* key, SettingsUprHelpers::CCharBuffer buffer);
	bool GetModuleSpecificIntEntry(const char* key, int& value);
	bool GetModuleSpecificBoolEntry(const char* key, bool& value);

	bool SetModuleSpecificStringEntryUtf16(const char* key, const wchar_t* str);
	bool SetModuleSpecificStringEntryUtf8(const char* key, const char* str);
	bool SetModuleSpecificIntEntry(const char* key, const int& value);
	bool SetModuleSpecificBoolEntry(const char* key, const bool& value);

	bool GetValueByRef(const char* key, SettingsUprHelpers::CWCharBuffer wbuffer) const;
	bool GetValueByRef(const char* key, bool& value) const;
	bool GetValueByRef(const char* key, int& value) const;

	void SetKey(const char* key, const wchar_t* value);
	void SetKey(const char* key, bool value);
	void SetKey(const char* key, int value);

	bool StoreData(bool bForceFileOutput);
	void CallSettingsDialog(void* hParent);
	void CallRootPathDialog(void* hParent);

	void SetRootPath(const wchar_t* szRootPath);

	//! \return Path determined either by registry or by INI file.
	void GetRootPathUtf16(SettingsUprHelpers::CWCharBuffer wbuffer);
	void GetRootPathAscii(SettingsUprHelpers::CCharBuffer buffer);

	bool GetInstalledBuildRootPathUtf16(const int index, SettingsUprHelpers::CWCharBuffer name, SettingsUprHelpers::CWCharBuffer path);

	void SetParentDialog(unsigned long window);

	long HandleProc(void* pWnd, long uMsg, long wParam, long lParam);

private:
	bool HasKey(const char* key);

	void LoadEngineSettingsFromRegistry();
	bool StoreEngineSettingsToRegistry();
	bool StoreLicenseSettingsToRegistry();

	//! Parses a file and stores all flags in a private key-value-map.
	bool LoadValuesFromConfigFile(const wchar_t* szFileName);

	bool SetRegValue(void* key, const char* valueName, const wchar_t* value);
	bool SetRegValue(void* key, const char* valueName, bool value);
	bool SetRegValue(void* key, const char* valueName, int value);
	bool GetRegValue(void* key, const char* valueName, SettingsUprHelpers::CWCharBuffer wbuffer);
	bool GetRegValue(void* key, const char* valueName, bool& value);
	bool GetRegValue(void* key, const char* valueName, int& value);

private:
	SettingsUprHelpers::CFixedString<wchar_t, 256> m_sModuleName;         //!< Name to store key-value pairs of modules in (registry) or to identify INI file.
	SettingsUprHelpers::CFixedString<wchar_t, 256> m_sModuleFileName;     //!< Used in case of data being loaded from INI file.
	bool m_bGetDataFromRegistry;
	SettingsUprHelpers::CKeyValueArray<30>         m_keyValueArray;

	void*         m_hBtnBrowse;
	unsigned long m_hWndParent;
};

#endif
