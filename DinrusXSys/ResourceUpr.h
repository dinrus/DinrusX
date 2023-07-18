// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   ResourceUpr.cpp
//  Version:     v1.00
//  Created:     8/02/2010 by Timur.
//  Описание: Interface to the Resource Upr
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _DRX_RESOURCE_MANAGER_H_
#define _DRX_RESOURCE_MANAGER_H_

#pragma once

#include <DinrusX/DinrusXSys/IResourceUpr.h>
#include <DinrusX/DinrusXSys/AsyncPakUpr.h>

//////////////////////////////////////////////////////////////////////////
// IResource manager interface
//////////////////////////////////////////////////////////////////////////
class CResourceUpr : public IResourceUpr, public ISystemEventListener, public IDrxPakFileAcesssSink
{
public:
	CResourceUpr();

	void Init();
	void Shutdown();

	bool IsStreamingCachePak(const char* filename) const;

	//////////////////////////////////////////////////////////////////////////
	// IResourceUpr interface implementation.
	//////////////////////////////////////////////////////////////////////////
	void           PrepareLevel(const char* sLevelFolder, const char* sLevelName);
	void           UnloadLevel();
	IResourceList* GetLevelResourceList();
	bool           LoadLevelCachePak(const char* sPakName, const char* sBindRoot, bool bOnlyDuringLevelLoading);
	void           UnloadLevelCachePak(const char* sPakName);
	bool           LoadModeSwitchPak(const char* sPakName, const bool multiplayer);
	void           UnloadModeSwitchPak(const char* sPakName, const char* sResourceListName, const bool multiplayer);
	bool           LoadMenuCommonPak(const char* sPakName);
	void           UnloadMenuCommonPak(const char* sPakName, const char* sResourceListName);
	bool           LoadPakToMemAsync(const char* pPath, bool bLevelLoadOnly);
	void           UnloadAllAsyncPaks();
	bool           LoadLayerPak(const char* sLayerName);
	void           UnloadLayerPak(const char* sLayerName);
	void           UnloadAllLevelCachePaks(bool bLevelLoadEnd);
	void           GetMemoryStatistics(IDrxSizer* pSizer);
	bool           LoadFastLoadPaks(bool bToMemory);
	void           UnloadFastLoadPaks();
	CTimeValue     GetLastLevelLoadTime() const;
	void           GetLayerPakStats(SLayerPakStats& stats, bool bCollectAllStats) const;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ISystemEventListener interface implementation.
	//////////////////////////////////////////////////////////////////////////
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// IDrxPakFileAcesssSink interface implementation.
	//////////////////////////////////////////////////////////////////////////
	virtual void ReportFileOpen(FILE* in, const char* szFullPath);
	//////////////////////////////////////////////////////////////////////////

	// Per frame update of the resource manager.
	void          Update();

	DrxPathString GetCurrentLevelCacheFolder() const { return m_currentLevelCacheFolder; };
	void          SaveRecordedResources(bool bTotalList = false);

private:

	//////////////////////////////////////////////////////////////////////////

	DrxPathString m_currentLevelCacheFolder;

	struct SOpenedPak
	{
		DrxFixedStringT<IDrxPak::g_nMaxPath> filename;
		bool bOnlyDuringLevelLoading;
	};
	std::vector<SOpenedPak>                           m_openedPaks;

	CAsyncPakUpr                                  m_AsyncPakUpr;

	string                                            m_sLevelFolder;
	string                                            m_sLevelName;
	bool                                              m_bLevelTransitioning;

	bool                                              m_bRegisteredFileOpenSink;
	bool                                              m_bOwnResourceList;

	CTimeValue                                        m_beginLevelLoadTime;
	CTimeValue                                        m_lastLevelLoadTime;

	_smart_ptr<IResourceList>                         m_pSequenceResourceList;

	DrxCriticalSection                                recordedFilesLock;
	std::vector<string>                               m_recordedFiles;

	std::vector<DrxFixedStringT<IDrxPak::g_nMaxPath>> m_fastLoadPakPaths;
};

#endif //_DRX_RESOURCE_MANAGER_H_
