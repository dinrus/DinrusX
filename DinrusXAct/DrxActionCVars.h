// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------

   Rip off NetworkCVars, general DinrusAction CVars should be here...

   -------------------------------------------------------------------------
   History:
   - 6:6:2007   Created by Benito G.R.

*************************************************************************/
#ifndef __DRXACTIONCVARS_H__
#define __DRXACTIONCVARS_H__

#pragma once

#include <DinrusX/DinrusXSys/IConsole.h>

class CDrxActionCVars
{
public:

	float playerInteractorRadius;  //Controls CInteractor action radius
	int   debugItemMemStats;       //Displays item mem stats

	int   g_debug_stats;
	int   g_statisticsMode;
	int   useCurrentUserNameAsDefault;

#if !defined(_RELEASE)
	int g_userNeverAutoSignsIn;
#endif

#ifdef AI_LOG_SIGNALS
	int   aiLogSignals;
	float aiMaxSignalDuration;
#endif
	int   aiFlowNodeAlertnessCheck;

	int g_gameplayAnalyst;
	int g_multiplayerEnableVehicles;

	// Cooperative Animation System
	int co_coopAnimDebug;
	int co_usenewcoopanimsystem;
	int co_slideWhileStreaming;
	// ~Cooperative Animation System

	int g_syncClassRegistry;

	int g_allowSaveLoadInEditor;
	int g_saveLoadBasicEntityOptimization;
	int g_debugSaveLoadMemory;
	int g_saveLoadUseExportedEntityList;
	int g_useXMLCPBinForSaveLoad;
	int g_XMLCPBGenerateXmlDebugFiles;
	int g_XMLCPBAddExtraDebugInfoToXmlDebugFiles;
	int g_XMLCPBSizeReportThreshold;
	int g_XMLCPBUseExtraZLibCompression;
	int g_XMLCPBBlockQueueLimit;
	int g_saveLoadExtendedLog;

	int g_debugDialogBuffers;

	int g_allowDisconnectIfUpdateFails;

	int g_useSinglePosition;
	int g_handleEvents;

	int g_enableMergedMeshRuntimeAreas;

	// AI stances
	ICVar* ag_defaultAIStance;

	int    sw_gridSize;
	int    sw_debugInfo;

	static ILINE CDrxActionCVars& Get()
	{
		DRX_ASSERT(s_pThis);
		return *s_pThis;
	}

private:
	friend class CDrxAction; // Our only creator

	CDrxActionCVars(); // singleton stuff
	~CDrxActionCVars();

	static CDrxActionCVars* s_pThis;

	static void DumpEntitySerializationData(IConsoleCmdArgs* pArgs);
	static void DumpClassRegistry(IConsoleCmdArgs* pArgs);
};

#endif // __DRXACTIONCVARS_H__
