#pragma once
<DinrusX/DinrusXSys/IDrxPlugin.h>
#include "DefaultComponents/Cameras/ICameraUpr.h"

class IPlugin_DrxDefaultEntities
	: public Drx::IEnginePlugin
	, public ISystemEventListener
{
public:
	virtual ~IPlugin_DrxDefaultEntities() { };

	virtual ICameraUpr* GetICameraUpr() = 0;
};