// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "stdafx.h"
#include "IResCompiler.h"
#include "XMLConverter.h"
#include "IRCLog.h"
#include "XML/xml.h"

#include <Core/Platform/DrxWindows.h>

// Must be included only once in DLL module.
#include <platform_implRC.inl>

static HMODULE g_hInst;

XmlStrCmpFunc g_pXmlStrCmp = &stricmp;

BOOL APIENTRY DllMain(
	HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = (HMODULE)hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

IDrxXML* LoadIDrxXML()
{
	HMODULE hXMLLibrary = LoadLibraryA("DrxXML.dll");
	if (NULL == hXMLLibrary)
	{
		RCLogError("Unable to load xml library (DrxXML.dll).");
		return 0;
	}

	FnGetIDrxXML pfnGetIDrxXML = (FnGetIDrxXML)GetProcAddress(hXMLLibrary, "GetIDrxXML");
	if (pfnGetIDrxXML == 0)
	{
		RCLogError("Unable to load xml library (DrxXML.dll) - cannot find exported function GetIDrxXML().");
		return 0;
	}

	return pfnGetIDrxXML();
}

void __stdcall RegisterConverters( IResourceCompiler *pRC )
{
	SetRCLog(pRC->GetIRCLog());

	IDrxXML* pDrxXML = LoadIDrxXML();
	if (pDrxXML == 0)
	{
		RCLogError("Loading xml library failed - not registering xml converter.");
		return;
	}

	pDrxXML->AddRef();

	pRC->RegisterConverter("XMLConverter", new XMLConverter(pDrxXML));

	pRC->RegisterKey("xmlFilterFile", "specify file with special commands to filter out unneeded XML elements and attributes");

	pDrxXML->Release();
}

