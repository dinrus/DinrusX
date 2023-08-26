// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   HTTPDownloader.h
//  Version:     v1.00
//  Created:     24/9/2004.
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#if DRX_PLATFORM_WINDOWS
	#include <DinrusX/CoreX/Platform/DrxWindows.h>
	#include <wininet.h> // requires <windows.h>
	#include <DinrusX/DinrusXScript/IScriptSystem.h>
	#include <dbghelp.h>

	#define HTTP_BUFFER_SIZE (16384)

enum
{
	HTTP_STATE_WORKING = 0,
	HTTP_STATE_COMPLETE,
	HTTP_STATE_CANCELED,
	HTTP_STATE_ERROR,
	HTTP_STATE_NONE,
};

class CDownloadUpr;

class CHTTPDownloader // : public _ScriptableEx<CHTTPDownloader>
{
public:
	CHTTPDownloader();
	virtual ~CHTTPDownloader();

	int           Create(ISystem* pISystem, CDownloadUpr* pParent);
	int           Download(const char* szURL, const char* szDestination);
	void          Cancel();
	int           GetState()             { return m_iState; };
	int           GetFileSize() const    { return m_iFileSize; };
	const string& GetURL() const         { return m_szURL; };
	const string& GetDstFileName() const { return m_szDstFile; };
	void          Release();

	int           Download(IFunctionHandler* pH);
	int           Cancel(IFunctionHandler* pH);
	int           Release(IFunctionHandler* pH);

	int           GetURL(IFunctionHandler* pH);
	int           GetFileSize(IFunctionHandler* pH);
	int           GetFileName(IFunctionHandler* pH);

	void          OnError();
	void          OnComplete();
	void          OnCancel();

private:

	static DWORD  DownloadProc(CHTTPDownloader* _this);
	void          CreateDownloadThread();
	DWORD         DoDownload();
	void          PrepareBuffer();
	IScriptTable* GetScriptObject() { return 0; };

	string            m_szURL;
	string            m_szDstFile;
	HANDLE            m_hThread;
	HINTERNET         m_hINET;
	HINTERNET         m_hUrl;

	unsigned char*    m_pBuffer;
	int               m_iFileSize;

	volatile int      m_iState;
	volatile bool     m_bContinue;

	ISystem*          m_pSystem;
	CDownloadUpr* m_pParent;

	IScriptSystem*    m_pScriptSystem;
};

#endif
