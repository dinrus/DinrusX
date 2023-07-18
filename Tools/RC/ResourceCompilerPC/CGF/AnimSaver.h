// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Copyright (C), Dinrus Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   AnimSaver.h
//  Version:     v1.00
//  Created:     27/9/2007 by Norbert
//  Compilers:   Visual Studio.NET
//  Описание: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __AnimSaver_h__
#define __AnimSaver_h__
#pragma once

#include <Core/String/DrxPath.h>
#include <Core/DrxTypeInfo.h>
#include "../DinrusX/DinrusX3dEng/CGF/ChunkFile.h"
#include <DinrusX/DinrusX3dEng/CGF/CGFContent.h>
#include "../CGA/Controller.h"
#include "../CGA/ControllerPQ.h"
#include "../CGA/ControllerPQLog.h"
#include "LoaderCAF.h"


class CSaverAnim
{
public:
	CSaverAnim(const char* filename,CChunkFile& chunkFile);
	virtual void Save(const CContentCGF *pCGF,const CInternalSkinningInfo *pSkinningInfo) = 0;

	static int SaveTCB3Track(CChunkFile *pChunkFile, const CInternalSkinningInfo *pSkinningInfo, int trackIndex);
	static int SaveTCBQTrack(CChunkFile *pChunkFile, const CInternalSkinningInfo *pSkinningInfo, int trackIndex);
	static int SaveTiming(CChunkFile *pChunkFile, int startSample, int endSample);

protected:
	int SaveExportFlags(const CContentCGF *pCGF);
	int SaveTiming(const CInternalSkinningInfo *pSkinningInfo);

	string m_filename;
	CChunkFile* m_pChunkFile;
};

#endif //__AnimSaver_h__