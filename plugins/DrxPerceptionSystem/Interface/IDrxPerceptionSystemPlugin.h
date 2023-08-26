// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>

struct IPerceptionUpr;

struct IDrxPerceptionSystemPlugin : public Drx::IEnginePlugin
{
	DRXINTERFACE_DECLARE_GUID(IDrxPerceptionSystemPlugin, "30c9bf81-e5fd-42df-ac3c-bc5fc1e6f0b4"_drx_guid);

public:
	virtual IPerceptionUpr& GetPerceptionUpr() const = 0;
};
