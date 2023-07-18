// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Math/Drx_Geo.h>
#include <DinrusX/CoreX/DrxTypeInfo.h>
//#include "../DinrusX/DinrusX3dEng/CGF/ChunkFile.h"
#include <DinrusX/DinrusX3dEng/CGFContent.h>

class CChunkData;
struct SVClothInfoCGF;

class CSaverVCloth
{
public:
	CSaverVCloth(CChunkData& chunkData, const SVClothInfoCGF* pVClothInfo, bool swapEndian);

	void WriteChunkHeader();
	void WriteChunkVertices();
	void WriteTriangleData();
	void WriteNndcNotAttachedOrdered();
	void WriteLinks();

private:

	CChunkData* m_pChunkData;
	const SVClothInfoCGF* m_pVClothInfo;
	bool m_bSwapEndian;

};

