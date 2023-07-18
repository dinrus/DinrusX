// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/SettingsUpr.h>

#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/StackString.h>

namespace Schematyc
{
	bool CSettingsUpr::RegisterSettings(const char* szName, const ISettingsPtr& pSettings)
	{
		SCHEMATYC_CORE_ASSERT_FATAL(szName && szName[0] && pSettings);
		if(szName && szName[0] && pSettings)
		{
			// #SchematycTODO : Validate name and check for collision/overlap!
			return m_settings.insert(Settings::value_type(szName, pSettings)).second;
		}
		return false;
	}

	ISettings* CSettingsUpr::GetSettings(const char* szName) const
	{
		Settings::const_iterator itSettings = m_settings.find(szName);
		return itSettings != m_settings.end() ? itSettings->second.get() : nullptr;
	}

	void CSettingsUpr::VisitSettings(const SettingsVisitor& visitor) const
	{
		SCHEMATYC_CORE_ASSERT(visitor);
		if(visitor)
		{
			for(const Settings::value_type& settings : m_settings)
			{
				if(visitor(settings.first, settings.second) == EVisitStatus::Stop)
				{
					break;
				}
			}
		}
	}

	void CSettingsUpr::LoadAllSettings()
	{
		const char* szSettingsFolder = gEnv->pSchematyc->GetSettingsFolder();
		for(const Settings::value_type& settings : m_settings)
		{
			const char*  szName = settings.first.c_str();
			CStackString fileName = szSettingsFolder;
			fileName.append("/");
			fileName.append(szName);
			fileName.append(".sc_settings");
			fileName.MakeLower();

			Serialization::LoadXmlFile(*settings.second, fileName);
		}
	}

	void CSettingsUpr::SaveAllSettings()
	{
		const char* szSettingsFolder = gEnv->pSchematyc->GetSettingsFolder();
		for(Settings::value_type& settings : m_settings)
		{
			const char*  szName = settings.first.c_str();
			CStackString fileName = szSettingsFolder;
			fileName.append("/");
			fileName.append(szName);
			fileName.append(".sc_settings");
			fileName.MakeLower();
			
			Serialization::SaveXmlFile(fileName, *settings.second, szName);
		}
	}
}