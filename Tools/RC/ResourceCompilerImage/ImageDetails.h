// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

struct IResourceCompiler;
class XmlNodeRef;

namespace AssetUpr
{

bool CollectDDSImageDetails(XmlNodeRef& xmlnode, const char* szFilename, IResourceCompiler* pRc);

}
