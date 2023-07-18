// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusX3dEng/CGFSaver.h>

class CSkinSaver
{
public:
	static bool SaveUncompiledSkinningInfo(CSaverCGF& cgfSaver, const CSkinningInfo& skinningInfo, const bool bSwapEndian, const float unitSizeInCentimeters);

	static int SaveBoneInitialMatrices(CSaverCGF& cgfSaver, const DynArray<DrxBoneDescData>& bonesDesc, const bool bSwapEndian, const float unitSizeInCentimeters);
	static int SaveBoneNames(CSaverCGF& cgfSaver, const DynArray<DrxBoneDescData>& bonesDesc, const bool bSwapEndian);
};


