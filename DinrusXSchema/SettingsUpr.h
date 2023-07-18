// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/ISettingsUpr.h>

namespace Schematyc
{
	class CSettingsUpr : public ISettingsUpr
	{
	private:

		typedef std::map<string, ISettingsPtr> Settings;

	public:

		// ISettingsUpr
		virtual bool RegisterSettings(const char* szName, const ISettingsPtr& pSettings) override;
		virtual ISettings* GetSettings(const char* szName) const override;
		virtual void VisitSettings(const SettingsVisitor& visitor) const override;
		virtual void LoadAllSettings() override;
		virtual void SaveAllSettings() override;
		// ~ISettingsUpr

	private:

		Settings m_settings;
	};
}
