// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

//#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/IArchiveHost.h>

namespace Serialization
{

IArchiveHost* CreateArchiveHost();
void RegisterArchiveHostCVars();
}
