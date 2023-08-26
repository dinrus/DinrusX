// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

#define VC_EXTRALEAN
<DinrusX/afxwin.h>
<DinrusX/afxext.h>

<DinrusX/CoreX/Platform/platform.h>
<DinrusX/CoreX/Platform/platform_impl.inl>

<DinrusX/IEditor.h>
#include "IPlugin.h"
#include "IEditorClassFactory.h"

#include "QtViewPane.h"

#include "Editor/MainEditorWindow.h"

#define UQS_EDITOR_NAME "UQS History"

REGISTER_VIEWPANE_FACTORY_AND_MENU(CMainEditorWindow, UQS_EDITOR_NAME, "Game", true, "Universal Query System")

class CUqsEditorHistoryInespectorPlugin : public IPlugin
{
	enum
	{
		Version = 1,
	};

public:
	CUqsEditorHistoryInespectorPlugin()
	{
	}

	~CUqsEditorHistoryInespectorPlugin()
	{
	}

	virtual int32       GetPluginVersion() override                          { return DWORD(Version); }
	virtual const char* GetPluginName() override                             { return UQS_EDITOR_NAME; }
	virtual const char* GetPluginDescription() override						 { return ""; }
	// ~IPlugin
};

REGISTER_PLUGIN(CUqsEditorHistoryInespectorPlugin)

