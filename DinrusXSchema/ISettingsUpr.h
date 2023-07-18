// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Should we be using guids to identify settings?

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/Delegate.h>

namespace Schematyc
{
struct ISettings
{
	virtual ~ISettings() {}

	virtual void Serialize(Serialization::IArchive& archive) = 0;
};

DECLARE_SHARED_POINTERS(ISettings)

typedef std::function<EVisitStatus (const char*, const ISettingsPtr&)> SettingsVisitor;

struct ISettingsUpr
{
	virtual ~ISettingsUpr() {}

	virtual bool       RegisterSettings(const char* szName, const ISettingsPtr& pSettings) = 0;
	virtual ISettings* GetSettings(const char* szName) const = 0;
	virtual void       VisitSettings(const SettingsVisitor& visitor) const = 0;
	virtual void       LoadAllSettings() = 0;
	virtual void       SaveAllSettings() = 0;
};
} // Schematyc
