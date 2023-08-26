// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:
   Console implementation for Android, reports back to the main interface.
   -------------------------------------------------------------------------
   History:
   - Aug 26,2013:	Created by Leander Beernaert

*************************************************************************/

#ifndef _ANDROIDCONSOLE_H_
#define _ANDROIDCONSOLE_H_

#include <DinrusX/DinrusXSys/IConsole.h>
#include <DinrusX/DinrusXSys/ITextModeConsole.h>
#include <DinrusX/DinrusXNetwork/INetwork.h>

class CAndroidConsole : public ISystemUserCallback,
	                      public IOutputPrintSink,
	                      public ITextModeConsole
{

	CAndroidConsole(const CAndroidConsole&);
	CAndroidConsole& operator=(const CAndroidConsole&);

	bool m_isInitialized;
public:
	static DrxCriticalSectionNonRecursive s_lock;
public:
	CAndroidConsole();
	~CAndroidConsole();

	// Interface IOutputPrintSink /////////////////////////////////////////////
	DLL_EXPORT virtual void Print(const char* line);

	// Interface ISystemUserCallback //////////////////////////////////////////
	virtual bool          OnError(const char* errorString);
	virtual bool          OnSaveDocument()  { return false; }
	virtual void          OnProcessSwitch() {}
	virtual void          OnInitProgress(const char* sProgressMsg);
	virtual void          OnInit(ISystem*);
	virtual void          OnShutdown();
	virtual void          OnUpdate();
	virtual void          GetMemoryUsage(IDrxSizer* pSizer);
	void                  SetRequireDedicatedServer(bool) {}
	virtual void          SetHeader(const char*)          {}
	// Interface ITextModeConsole /////////////////////////////////////////////
	virtual Vec2_tpl<int> BeginDraw();
	virtual void          PutText(int x, int y, const char* msg);
	virtual void          EndDraw();

};

#endif // _ANDROIDCONSOLE_H_
